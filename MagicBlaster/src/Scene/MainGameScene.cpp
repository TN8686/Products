#include <cmath>
#include "MainGameScene.h"
#include "../primitive2dObj.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"
#include "../Engine/D3dx12.h"
#include "../Engine/Texture.h"
#include "../Mummy.h"
#include "../Bat.h"
#include "../TimeManager.h"
#include "../UniquePtrTemplate.h"
#include "../ScoreManager.h"

using namespace DirectX;

namespace {
	constexpr float MAIN_TIME = 90.0f;
}
/*
const Cell bgCellList[] = {
  { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 800, 600 }, { 0, 0 }, 0 } };

const Cell sqCellList[] = {
  { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 2, 2 }, { 0, 0 }, 0 } };

const Cell cellList[] = {
	//始点の座標(左上)				  終点の座標					  画像サイズ
	{ XMFLOAT2(5 / 15.0f, 4 / 15.0f), XMFLOAT2(1 / 15.0f, 1 / 15.0f), XMFLOAT2(48, 48) } };
*/

const Cell cellMagicCircle[] = {
	{ XMFLOAT2(0, 0), XMFLOAT2(1.0f / 8.0f, 1.0f / 4.0f), XMFLOAT2(32, 32) },
	{ XMFLOAT2(1.0f / 8.0f, 0), XMFLOAT2(1.0f / 8.0f, 1.0f / 4.0f), XMFLOAT2(32, 32) },

	{ XMFLOAT2(0, 1.0f / 4.0f), XMFLOAT2(1.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(64, 32) },
	{ XMFLOAT2(1.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(1.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(64, 32) },
	{ XMFLOAT2(2.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(1.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(64, 32) },
	{ XMFLOAT2(3.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(1.0f / 4.0f, 1.0f / 4.0f), XMFLOAT2(64, 32) },

	{ XMFLOAT2(0, 1.0f / 2.0f), XMFLOAT2(1.0f / 2.0f, 1.0f / 2.0f), XMFLOAT2(128, 64) },
	{ XMFLOAT2(1.0f / 2.0f, 1.0f / 2.0f), XMFLOAT2(1.0f / 2.0f, 1.0f / 2.0f), XMFLOAT2(128, 64) },
};

/**
* シーンを読み込む.
*/
bool MainGameScene::Load()
{
	Graphics& graphics = Graphics::Get();

	// 背景. 画面の中央.
	const XMFLOAT2 center(graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f);

	ScoreManager::GetIncetance().Init();

	_blackResource.Register(&_spriteBlack);
	_spriteBlack.rectOffset.right = graphics.viewport.Width;
	_spriteBlack.rectOffset.bottom = graphics.viewport.Height;
	_spriteBlack.addColor.w = 1.0f;

	// テクスチャローダーは改善予定 色んな場所で呼んでも問題無くなれば、Texture型をクラス毎、オブジェクト毎に管理できる
	//  → サンプルを元に改善済み　TODO Sprite管理システムも完成したので随時移行.
	graphics.texMap.Begin();
	// エネミーファクトリ.
	_pEnemyFactory = EnemyFactory::pGetInstance();
	_pEnemyFactory->pSetSpriteResourceList(&_spriteResourceList);
	_pEnemyFactory->Load();
	_pEnemyFactory->pSetBlockList(&_blockObj);
	_pEnemyFactory->pSetThroughBlockList(&_throughBlockObj);

	// ジェネレータ.
	EnemyGenerator::pSetSpriteResource(pCreateObject(_spriteResourceList));
	EnemyGenerator::Load();
	_enemyGeneratorList.reserve(5);
	for (int i = 0; i < 5; ++i) {
		_enemyGeneratorList.emplace_back(new EnemyGenerator);
	}

	for (auto& e : _enemyGeneratorList) {
		e.get()->pSetPlayer(&_player);
		e.get()->pSetCamera(&_camera);
		e.get()->Init();
	}
	// 仮置き.
	_enemyGeneratorList.at(4).get()->SetLocalPosition({ 100, clientHeight - 176 - 64 * 0 });
	_enemyGeneratorList.at(3).get()->SetLocalPosition({ (clientWidth * 2) - 100, clientHeight - 176 - 64 * 0 });

	_enemyGeneratorList.at(2).get()->SetLocalPosition({ clientWidth, clientHeight - 176 - 64 * 10 });	// センター.

	_enemyGeneratorList.at(1).get()->SetLocalPosition({ 100, clientHeight - 176 - 64 * 20 });
	_enemyGeneratorList.at(0).get()->SetLocalPosition({ (clientWidth * 2) - 100, clientHeight - 176 - 64 * 20 });

	bulletQueue.reset(new MagicBulletQueue(_MagicBulletObj));

	// TODO _spriteResourceListは必要数が明確になったら事前にreserveする.
	Player::pSetSpriteResource(pCreateObject(_spriteResourceList));
	Player::Load();
	_player.SetEnemyList(&_pEnemyFactory->GetEnemyList());
	_player.SetSquareSprite(&sprSquare);	// この順番はよくないけど一旦テスト.
	_player.SetWorldPosition({ clientWidth * 0.5,  clientHeight - 64 * 7 });
	_player.Init();
	_player.SetBulletQue(bulletQueue.get());
	_player.SetBlockList(&_blockObj);
	_player.SetThroughBlockList(&_throughBlockObj);
	_player.pSetCamera(&_camera);
	_player.SetMagicBlast(&_magicBlast);

	// 暗幕.
	if (!graphics.texMap.CreateRect(_blackResource.GetTexture(), 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}
	// UIテキスト関連.
	if (!graphics.texMap.LoadFromFile(_textResouce.GetTexture(), L"Res/scoreText.png")) {
		return false;
	}

	// 旧ロード関連　シーン内で直接やる必要のある物以外は各クラス内に順次移行. 
	if (!graphics.texMap.LoadFromFile(texture[0], L"Res/bg_00a.png")) {
		return false;
	}
	if (!graphics.texMap.CreateRect(texture[3], 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(texture[4], L"Res/bgtip_01.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(texMagicCircle, L"Res/MagicCircle.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(texMagicBullet, L"Res/bullet.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(*_magicBlast.GetTexturePtr(), L"Res/Blast.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(*Bat::GetTexturePtr(), L"Res/ene_08.png")) {
		return false;
	}
	_numDraw.Load(32);	// 仮に32ケタ分.

	ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
	graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	graphics.WaitForGpu();
	graphics.texMap.ResetLoader();


	// AABB2DObjが出来た事で解決したが、記録として残しておく.
	// Vector内でメモリの再割り当てが行われた場合、違う場所に確保し直すため、各要素へのポインタを保持していると異常な場所を参照してしまう場合がある。.
	// 再確保された場合にはシーケンス中の要素を指す全ての参照、ポインタ、イテレータが無効になる。.
	// reserve()が呼ばれた後は、挿入によってvectorの要素数がcapacity()の値よりも大きくなるまでは、挿入によって再確保が行われないことが保証されている。.

	_camera.SetLocalPosition({ center.x, center.y });
	_camera.SetTarget(&_player);

	_magicBlast.Init();
	_magicBlast.pSetCamera(&_camera);
	_magicBlast.SetEnemyList(&_pEnemyFactory->GetEnemyList());
	_magicBlast.SetPlayer(&_player);

	sprBackground.push_back(Sprite(nullptr, XMFLOAT3(center.x, center.y, 1.0f), 0.0f, { 4.0f, 4.0f }));
	sprBackground.push_back(Sprite(nullptr, XMFLOAT3(center.x + clientWidth, center.y, 1.0f), 0.0f, { 4.0f, 4.0f }));
	sprMagicCircle.resize(Player::MAGIC_CIRCLE_NUM + 1, Sprite(nullptr, XMFLOAT3(center.x, center.y, 1.0f), 0.0f, { 4.0f,4.0f }));
	sprMagicBullet.resize(MAGIC_BULLET_NUM, Sprite(nullptr, XMFLOAT3(center.x, center.y, 1.0f), 0.0f, { 4.0f,4.0f }));

	sprBlock.resize(256 + 160, Sprite(nullptr, XMFLOAT3(0, 0, 0), 0.0f, { 4.0f,4.0f }));	// 仮にスプライトの数を増やす.
	_player.SetSprMagicCircle(&sprMagicCircle);

	// 弾テスト.

	Bullet::SetEnemyList(&_pEnemyFactory->GetEnemyList());
	Bullet::SetPlayer(&_player);
	Bullet::SetBlockList(&_blockObj);
	_MagicBulletObj.resize(MAGIC_BULLET_NUM);
	int num = 0;
	for (auto& e : _MagicBulletObj) {
		e.pSetSprite(&sprMagicBullet[num]);
		e.SetOnActive(false);
		e.pSetCamera(&_camera);
		num++;
	}


	// 地形テスト.
	_blockObj.resize(256);
	num = 0;
	// とりあえずベタ書きで雑に並べる.
	for (auto& e : _blockObj) {
		e.SetLocalPosition({ 32.0f + 64.0f * num, clientHeight});
		e.pSetSprite(&sprBlock[num]);
		sprBlock[num].SetCellIndex(226);
		if (num >= 40) {
			e.SetOnActive(false);
		}

		if (num >= 247) {
			e.SetOnActive(true);
		}
		num++;
	}
	
	// とりあえず仮に位置だけ付ける.	

	/*
	_blockObj[248].SetLocalPosition({ 64.0f * 9, clientHeight - 64 * 10 });
	_blockObj[249].SetLocalPosition({ 64.0f * 10, clientHeight - 64 * 10 });
	_blockObj[250].SetLocalPosition({ 64.0f * 11, clientHeight - 64 * 10 });
	_blockObj[251].SetLocalPosition({ 64.0f * 12, clientHeight - 64 * 10 });
	*/
	_blockObj[252].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 5 });
	_blockObj[247].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 4 });	// 仮なので変な順番.
	_blockObj[253].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 3 });
	_blockObj[254].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 2 });
	_blockObj[255].SetLocalPosition({ 64.0f * 20, clientHeight - 64 });

	// 見た目を付けてみる.
	sprBlock[252].SetCellIndex(9);
	sprBlock[247].SetCellIndex(25);
	sprBlock[253].SetCellIndex(25);
	sprBlock[254].SetCellIndex(25);
	sprBlock[255].SetCellIndex(41);

	
	_throughBlockObj.resize(160);	// 薄い床テスト.

	num = 0;
	int x = 0;
	int y = 1;
	// とりあえず雑に並べる.
	for (auto& e : _throughBlockObj) {
		e.SetCanThrough(true);
		e.SetCollision({{0,-20}, {32, 12}});
		e.SetLocalPosition({ 32.0f + 64.0f * x, clientHeight - (64.0f * 5 * y) });
		e.pSetSprite(&sprBlock[256 + num]);
		sprBlock[256 + num].SetCellIndex(226 - 48);	// 薄い床テスト.
		++num;
		++x;
		if (x % 40 == 0) {
			++y;
			x = 0;
		}
	}

	// 魔法陣テスト
	num = 0;
	for (auto& e : sprMagicCircle) {
		e.pos = { center.x, 128.0f * num,0 };
		e.rotation = -0.7853981633974483f * num;
		e.SetCellIndex(0);
		num++;
	}

	blockCellList.list.resize(256);
	if (CreateCellList(256, 16, 16, 16, 16, blockCellList) != 0) {
		return false;
	}

	// 仮UI.
	_textResouce.CreateCellList(2, 4, 40, 16);

	_spriteText.resize(6);
	for (auto& e : _spriteText) {
		_textResouce.Register(&e);
		e.cameraRelative = false;
	}
	_spriteText.at(0).animeController.SetCellIndex(1);	// Score.
	_spriteText.at(1).animeController.SetCellIndex(2);	// Bonus.
	_spriteText.at(2).animeController.SetCellIndex(3);	// x.
	_spriteText.at(3).animeController.SetCellIndex(4);	// Time.
	_spriteText.at(4).animeController.SetCellIndex(5);	// 「.」.
	_spriteText.at(5).animeController.SetCellIndex(7);	// Combo.

	_spriteText.at(3).SetLocalPosition({ clientWidth * 0.5 + 7, 30 });
	_spriteText.at(3).scale = { 2.0f, 2.0f };

	_spriteText.at(0).SetLocalPosition({ 1115, 25 });
	_spriteText.at(5).SetLocalPosition({ 1120, 90 });

	_spriteText.at(1).SetLocalPosition({ 1165, 115 + 5 });
	_spriteText.at(2).SetLocalPosition({ 1208, 115 + 5 });
	_spriteText.at(4).SetLocalPosition({ 1228, 116 + 5 });

	_spriteText.at(5).scale = { 2, 2 };

	// コウモリテスト.
	//_pEnemyFactory->Enqueue(EnemyID::Mummy, { _camera.GetWorldPosition().x + clientWidth / 2 + 100,  clientHeight - 180 - 64 * 4 }, &_camera, &_player);

	TimeManager::GetInstance().SetNextMagnification(0);

	return true;
}

bool MainGameScene::Unload() {
	_pEnemyFactory->Finalize();
	TimeManager::GetInstance().Reset();
	return true;
}

/**
* タイトルシーンオブジェクトを作成する.
*
* @return 作成したタイトルシーンオブジェクトへのポインタ.
*/
ScenePtr MainGameScene::Create()
{
	return ScenePtr(new MainGameScene);
}

/**
* コンストラクタ.
*/
MainGameScene::MainGameScene()
	: Scene(L"MainGame")
	, _blackTimer(0.5)
	, _mainTimer(MAIN_TIME)
	, _pEnemyFactory(nullptr)
	, _startTimer(4)
{
}

/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int MainGameScene::Update(double delta)
{
	auto& tm = TimeManager::GetInstance();
	tm.Update(delta);
	double cDelta = tm.GetCorrectedDelta();	// 補正後Deltaを取得.

	if (!_startTimer.IsReached()) {
		_startTimer.Update(delta);	// 補正前deltaでカウント.

		if (_startTimer.GetTime() < 1) {
			tm.SetNextMagnification(1);	// 開始.
		}
	}

	if (_mainTimer.IsReached()) {	// メインタイマーが到達していた次のフレームでリザルトへ移行.
	//振動を解除.
		XINPUT_VIBRATION _viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);
	
		return ExitCode_Resoult;
	}

	_mainTimer.Update(cDelta);
	_blackTimer.Update(delta);
	_spriteBlack.addColor.w = 1.0 - _blackTimer.GetRatio();

	// 仮のザコ生成.
	/*
	static double time = 0;
	static int num = 0;

	time += cDelta;
	if (time >= 8.0 / 60) {
		time -= 8.0 / 60;
		if (num == 16) {
			_pEnemyFactory->Enqueue(EnemyID::Mummy, { _camera.GetWorldPosition().x - clientWidth / 2 - 100,  clientHeight - 176 - 64 * 0 }, &_camera, &_player, true);
			num = 0;
		}
		else {
			_pEnemyFactory->Enqueue(EnemyID::Mummy, { _camera.GetWorldPosition().x + clientWidth / 2 + 100,  clientHeight - 176 - 64 * 0 }, &_camera, &_player);
		}
		num++;
	}
	*/

	// 予約されている敵を生成.
	_pEnemyFactory->Create();

	// 地形テスト.
	for (auto& e : _blockObj) {
		e.Update(cDelta);
	}
	for (auto& e : _throughBlockObj) {
		e.Update(cDelta);
	}

	_player.Update(cDelta);
	for (auto& e : _MagicBulletObj) {
		e.Update(cDelta);
	}
	_magicBlast.Update(cDelta);

	for (auto& e : _enemyGeneratorList) {
		e.get()->Update(cDelta);
	}

	for (auto& e : _pEnemyFactory->GetEnemyList()) {
		e->Update(cDelta);
	}

	_camera.Update(cDelta);

	// 仮に背景を奥にあるっぽく見せる.
	static Point2D pos[2] = { {0, clientHeight * 0.25f }, {clientWidth, clientHeight * 0.25f} };
	sprBackground[0].pos.x = pos[0].x + _camera.GetWorldPosition().x * 0.5f;
	sprBackground[0].pos.y = pos[0].y + _camera.GetWorldPosition().y * 0.5f;
	sprBackground[1].pos.x = pos[1].x + _camera.GetWorldPosition().x * 0.5f;
	sprBackground[1].pos.y = pos[1].y + _camera.GetWorldPosition().y * 0.5f;

	// スコアをアップデート.
	auto& sm = ScoreManager::GetIncetance();

	sm.Update(delta);

	// 仮のスコア表示.
	static NumDraw::Param param;
	static bool test = true;
	param.position = { 1200, 10 };
	param.num = sm.GetScore();
	param.digit = 8;
	param.scale = { 1.0f, 1.0f };
	param.alignment = NumDraw::ALIGN_CENTER;
	param.verticalAlign = NumDraw::VER_ALIGN_TOP;
	param.color = { 1,1,1,1 };
	_numDraw.SetDrawParam(param);

	param.num = sm.GetCombo();
	param.position = { 1200, 10 + 48 };
	param.scale = { 2.0f, 2.0f };
	param.digit = 3;
	_numDraw.SetDrawParam(param);

	param.num = static_cast<int>(sm.GetBonus() * 100.0f);
	param.position = { 1220, 58 + 40 + 5 };
	param.scale = { 1.0f, 1.0f };
	param.digit = 3;
	_numDraw.SetDrawParam(param);

	param.num = static_cast<int>(_mainTimer.GetTime() + 0.99);
	param.position = { clientWidth * 0.5f, 30 };
	param.scale = { 3.0f, 3.0f };
	param.digit = 0;
	if (param.num <= 5) {
		param.position = { clientWidth * 0.5f, 33 };
		param.color = { 1,0,0,1 };
		param.scale = { 4.0f, 4.0f };
	}
	_numDraw.SetDrawParam(param);

	if (!_startTimer.IsReached()) {
		param.num = static_cast<int>(_startTimer.GetTime());
		param.position = { clientWidth * 0.5f, clientHeight * 0.5f };
		param.scale = { 8.0f, 8.0f };
		param.verticalAlign = NumDraw::VER_ALIGN_CENTER;
		_numDraw.SetDrawParam(param);
	}

	_numDraw.ReadyToDraw();	// 描画準備完了.

	// シーン遷移テスト.
	const GamePad gamepad = GetGamePad(GamePadId_1P);
	if (gamepad.buttonDown & GamePad::START) {
		XINPUT_VIBRATION _viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);

		return ExitCode_Pause;
	}
	return ExitCode_Continue;
}


/**
* シーンの描画.
*/
void MainGameScene::Draw() const
{

	Graphics& graphics = Graphics::Get();

	// 描画のための仕組みの設定
	RenderingInfo renderingInfo;
	renderingInfo.rtvHandle = graphics.GetRTVHandle();
	renderingInfo.dsvHandle = graphics.GetDSVHandle();
	renderingInfo.viewport = graphics.viewport;
	renderingInfo.scissorRect = graphics.scissorRect;
	renderingInfo.texDescHeap = graphics.csuDescriptorHeap.Get();
	renderingInfo.matViewProjection = graphics.matViewProjection;

	// 描画に必要な手順をひとまとめにしたパイプライン
	const PSO& pso = GetPSO(PSOType_Sprite);

	// 描画.
	graphics.spriteRenderer.Draw(sprBackground, nullptr, pso, texture[0], renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(sprBlock, blockCellList.list.data(), pso, texture[4], renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(EnemyGenerator::pGetSpriteResource(), pso, renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(Player::pGetSpriteResource(), pso, renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(sprMagicCircle, cellMagicCircle, pso, texMagicCircle, renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(Mummy::pGetSpriteResource(), pso, renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(*Bat::GetSpriteListPtr(), Bat::GetCellListPtr()->list.data(), pso, *Bat::GetTexturePtr(), renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(sprMagicBullet, nullptr, pso, texMagicBullet, renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(*_magicBlast.GetSpriteListPtr(), _magicBlast.GetCellListPtr()->list.data(), pso, *MagicBlast::GetTexturePtr(), renderingInfo, &_camera);
	graphics.spriteRenderer.Draw(sprSquare, nullptr, pso, texture[3], renderingInfo, &_camera);
	AABB2DObj::DrawBegin();
	graphics.spriteRenderer.Draw(*AABB2DObj::pGetSprite(), nullptr, pso, texture[3], renderingInfo, &_camera);

	graphics.spriteRenderer.Draw(_numDraw.pGetSpriteResource(), pso, renderingInfo, nullptr);
	graphics.spriteRenderer.Draw(&_textResouce, pso, renderingInfo, nullptr);

	graphics.spriteRenderer.Draw(&_blackResource, pso, renderingInfo, nullptr);

	
}
