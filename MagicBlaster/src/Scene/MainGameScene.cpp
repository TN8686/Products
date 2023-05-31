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
	//�n�_�̍��W(����)				  �I�_�̍��W					  �摜�T�C�Y
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
* �V�[����ǂݍ���.
*/
bool MainGameScene::Load()
{
	Graphics& graphics = Graphics::Get();

	// �w�i. ��ʂ̒���.
	const XMFLOAT2 center(graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f);

	ScoreManager::GetIncetance().Init();

	_blackResource.Register(&_spriteBlack);
	_spriteBlack.rectOffset.right = graphics.viewport.Width;
	_spriteBlack.rectOffset.bottom = graphics.viewport.Height;
	_spriteBlack.addColor.w = 1.0f;

	// �e�N�X�`�����[�_�[�͉��P�\�� �F��ȏꏊ�ŌĂ�ł���薳���Ȃ�΁ATexture�^���N���X���A�I�u�W�F�N�g���ɊǗ��ł���
	//  �� �T���v�������ɉ��P�ς݁@TODO Sprite�Ǘ��V�X�e�������������̂Ő����ڍs.
	graphics.texMap.Begin();
	// �G�l�~�[�t�@�N�g��.
	_pEnemyFactory = EnemyFactory::pGetInstance();
	_pEnemyFactory->pSetSpriteResourceList(&_spriteResourceList);
	_pEnemyFactory->Load();
	_pEnemyFactory->pSetBlockList(&_blockObj);
	_pEnemyFactory->pSetThroughBlockList(&_throughBlockObj);

	// �W�F�l���[�^.
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
	// ���u��.
	_enemyGeneratorList.at(4).get()->SetLocalPosition({ 100, clientHeight - 176 - 64 * 0 });
	_enemyGeneratorList.at(3).get()->SetLocalPosition({ (clientWidth * 2) - 100, clientHeight - 176 - 64 * 0 });

	_enemyGeneratorList.at(2).get()->SetLocalPosition({ clientWidth, clientHeight - 176 - 64 * 10 });	// �Z���^�[.

	_enemyGeneratorList.at(1).get()->SetLocalPosition({ 100, clientHeight - 176 - 64 * 20 });
	_enemyGeneratorList.at(0).get()->SetLocalPosition({ (clientWidth * 2) - 100, clientHeight - 176 - 64 * 20 });

	bulletQueue.reset(new MagicBulletQueue(_MagicBulletObj));

	// TODO _spriteResourceList�͕K�v�������m�ɂȂ����玖�O��reserve����.
	Player::pSetSpriteResource(pCreateObject(_spriteResourceList));
	Player::Load();
	_player.SetEnemyList(&_pEnemyFactory->GetEnemyList());
	_player.SetSquareSprite(&sprSquare);	// ���̏��Ԃ͂悭�Ȃ����ǈ�U�e�X�g.
	_player.SetWorldPosition({ clientWidth * 0.5,  clientHeight - 64 * 7 });
	_player.Init();
	_player.SetBulletQue(bulletQueue.get());
	_player.SetBlockList(&_blockObj);
	_player.SetThroughBlockList(&_throughBlockObj);
	_player.pSetCamera(&_camera);
	_player.SetMagicBlast(&_magicBlast);

	// �Ö�.
	if (!graphics.texMap.CreateRect(_blackResource.GetTexture(), 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}
	// UI�e�L�X�g�֘A.
	if (!graphics.texMap.LoadFromFile(_textResouce.GetTexture(), L"Res/scoreText.png")) {
		return false;
	}

	// �����[�h�֘A�@�V�[�����Œ��ڂ��K�v�̂��镨�ȊO�͊e�N���X���ɏ����ڍs. 
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
	_numDraw.Load(32);	// ����32�P�^��.

	ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
	graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	graphics.WaitForGpu();
	graphics.texMap.ResetLoader();


	// AABB2DObj���o�������ŉ����������A�L�^�Ƃ��Ďc���Ă���.
	// Vector���Ń������̍Ċ��蓖�Ă��s��ꂽ�ꍇ�A�Ⴄ�ꏊ�Ɋm�ۂ��������߁A�e�v�f�ւ̃|�C���^��ێ����Ă���ƈُ�ȏꏊ���Q�Ƃ��Ă��܂��ꍇ������B.
	// �Ċm�ۂ��ꂽ�ꍇ�ɂ̓V�[�P���X���̗v�f���w���S�Ă̎Q�ƁA�|�C���^�A�C�e���[�^�������ɂȂ�B.
	// reserve()���Ă΂ꂽ��́A�}���ɂ����vector�̗v�f����capacity()�̒l�����傫���Ȃ�܂ł́A�}���ɂ���čĊm�ۂ��s���Ȃ����Ƃ��ۏ؂���Ă���B.

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

	sprBlock.resize(256 + 160, Sprite(nullptr, XMFLOAT3(0, 0, 0), 0.0f, { 4.0f,4.0f }));	// ���ɃX�v���C�g�̐��𑝂₷.
	_player.SetSprMagicCircle(&sprMagicCircle);

	// �e�e�X�g.

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


	// �n�`�e�X�g.
	_blockObj.resize(256);
	num = 0;
	// �Ƃ肠�����x�^�����ŎG�ɕ��ׂ�.
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
	
	// �Ƃ肠�������Ɉʒu�����t����.	

	/*
	_blockObj[248].SetLocalPosition({ 64.0f * 9, clientHeight - 64 * 10 });
	_blockObj[249].SetLocalPosition({ 64.0f * 10, clientHeight - 64 * 10 });
	_blockObj[250].SetLocalPosition({ 64.0f * 11, clientHeight - 64 * 10 });
	_blockObj[251].SetLocalPosition({ 64.0f * 12, clientHeight - 64 * 10 });
	*/
	_blockObj[252].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 5 });
	_blockObj[247].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 4 });	// ���Ȃ̂ŕςȏ���.
	_blockObj[253].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 3 });
	_blockObj[254].SetLocalPosition({ 64.0f * 20, clientHeight - 64 * 2 });
	_blockObj[255].SetLocalPosition({ 64.0f * 20, clientHeight - 64 });

	// �����ڂ�t���Ă݂�.
	sprBlock[252].SetCellIndex(9);
	sprBlock[247].SetCellIndex(25);
	sprBlock[253].SetCellIndex(25);
	sprBlock[254].SetCellIndex(25);
	sprBlock[255].SetCellIndex(41);

	
	_throughBlockObj.resize(160);	// �������e�X�g.

	num = 0;
	int x = 0;
	int y = 1;
	// �Ƃ肠�����G�ɕ��ׂ�.
	for (auto& e : _throughBlockObj) {
		e.SetCanThrough(true);
		e.SetCollision({{0,-20}, {32, 12}});
		e.SetLocalPosition({ 32.0f + 64.0f * x, clientHeight - (64.0f * 5 * y) });
		e.pSetSprite(&sprBlock[256 + num]);
		sprBlock[256 + num].SetCellIndex(226 - 48);	// �������e�X�g.
		++num;
		++x;
		if (x % 40 == 0) {
			++y;
			x = 0;
		}
	}

	// ���@�w�e�X�g
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

	// ��UI.
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
	_spriteText.at(4).animeController.SetCellIndex(5);	// �u.�v.
	_spriteText.at(5).animeController.SetCellIndex(7);	// Combo.

	_spriteText.at(3).SetLocalPosition({ clientWidth * 0.5 + 7, 30 });
	_spriteText.at(3).scale = { 2.0f, 2.0f };

	_spriteText.at(0).SetLocalPosition({ 1115, 25 });
	_spriteText.at(5).SetLocalPosition({ 1120, 90 });

	_spriteText.at(1).SetLocalPosition({ 1165, 115 + 5 });
	_spriteText.at(2).SetLocalPosition({ 1208, 115 + 5 });
	_spriteText.at(4).SetLocalPosition({ 1228, 116 + 5 });

	_spriteText.at(5).scale = { 2, 2 };

	// �R�E�����e�X�g.
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
* �^�C�g���V�[���I�u�W�F�N�g���쐬����.
*
* @return �쐬�����^�C�g���V�[���I�u�W�F�N�g�ւ̃|�C���^.
*/
ScenePtr MainGameScene::Create()
{
	return ScenePtr(new MainGameScene);
}

/**
* �R���X�g���N�^.
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
* �V�[���̍X�V.
*
* @param delta �O��̌Ăяo������̌o�ߎ���.
*
* @return �I���R�[�h.
*/
int MainGameScene::Update(double delta)
{
	auto& tm = TimeManager::GetInstance();
	tm.Update(delta);
	double cDelta = tm.GetCorrectedDelta();	// �␳��Delta���擾.

	if (!_startTimer.IsReached()) {
		_startTimer.Update(delta);	// �␳�Odelta�ŃJ�E���g.

		if (_startTimer.GetTime() < 1) {
			tm.SetNextMagnification(1);	// �J�n.
		}
	}

	if (_mainTimer.IsReached()) {	// ���C���^�C�}�[�����B���Ă������̃t���[���Ń��U���g�ֈڍs.
	//�U��������.
		XINPUT_VIBRATION _viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);
	
		return ExitCode_Resoult;
	}

	_mainTimer.Update(cDelta);
	_blackTimer.Update(delta);
	_spriteBlack.addColor.w = 1.0 - _blackTimer.GetRatio();

	// ���̃U�R����.
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

	// �\�񂳂�Ă���G�𐶐�.
	_pEnemyFactory->Create();

	// �n�`�e�X�g.
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

	// ���ɔw�i�����ɂ�����ۂ�������.
	static Point2D pos[2] = { {0, clientHeight * 0.25f }, {clientWidth, clientHeight * 0.25f} };
	sprBackground[0].pos.x = pos[0].x + _camera.GetWorldPosition().x * 0.5f;
	sprBackground[0].pos.y = pos[0].y + _camera.GetWorldPosition().y * 0.5f;
	sprBackground[1].pos.x = pos[1].x + _camera.GetWorldPosition().x * 0.5f;
	sprBackground[1].pos.y = pos[1].y + _camera.GetWorldPosition().y * 0.5f;

	// �X�R�A���A�b�v�f�[�g.
	auto& sm = ScoreManager::GetIncetance();

	sm.Update(delta);

	// ���̃X�R�A�\��.
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

	_numDraw.ReadyToDraw();	// �`�揀������.

	// �V�[���J�ڃe�X�g.
	const GamePad gamepad = GetGamePad(GamePadId_1P);
	if (gamepad.buttonDown & GamePad::START) {
		XINPUT_VIBRATION _viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);

		return ExitCode_Pause;
	}
	return ExitCode_Continue;
}


/**
* �V�[���̕`��.
*/
void MainGameScene::Draw() const
{

	Graphics& graphics = Graphics::Get();

	// �`��̂��߂̎d�g�݂̐ݒ�
	RenderingInfo renderingInfo;
	renderingInfo.rtvHandle = graphics.GetRTVHandle();
	renderingInfo.dsvHandle = graphics.GetDSVHandle();
	renderingInfo.viewport = graphics.viewport;
	renderingInfo.scissorRect = graphics.scissorRect;
	renderingInfo.texDescHeap = graphics.csuDescriptorHeap.Get();
	renderingInfo.matViewProjection = graphics.matViewProjection;

	// �`��ɕK�v�Ȏ菇���ЂƂ܂Ƃ߂ɂ����p�C�v���C��
	const PSO& pso = GetPSO(PSOType_Sprite);

	// �`��.
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
