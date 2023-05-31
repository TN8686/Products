#include <cmath>
#include "TitleScene.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"
#include "../ScoreManager.h"

namespace {
	int anyButton = (GamePad::A | GamePad::B | GamePad::X | GamePad::Y | GamePad::START);
}

// 諸々仮置き.

using namespace DirectX;

/**
* シーンを読み込む.
*/
bool TitleScene::Load()
{
	// PtrVector関連テスト.
	Graphics& graphics = Graphics::Get();

	// 新しいSpriteResourceの実験中.
	_spriteResource2.Register(&_sprite);
	// 画面の中央を登録.
	_sprite.SetWorldPosition({ graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f });
	//_spriteResource2.CreateCellList(4, 4, 1280 / 4, 720 / 4);

	//_spriteResource.Register(_spriteHandle);
	// 画面の中央を登録.
	//_spriteHandle.pGetSprite()->pos = { graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f, 0 };

	_blackResource.Register(&_spriteBlack);
	_spriteBlack.rectOffset.right = graphics.viewport.Width;
	_spriteBlack.rectOffset.bottom = graphics.viewport.Height;


	graphics.texMap.Begin();
	if (!graphics.texMap.LoadFromFile(_spriteResource2.GetTexture(), L"Res/title.png")) {
		return false;
	}
	if (!graphics.texMap.CreateRect(_blackResource.GetTexture(), 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}

	_numDraw.Load();

	ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
	graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	graphics.WaitForGpu();
	graphics.texMap.ResetLoader();

	return true;
}

/**
* タイトルシーンオブジェクトを作成する.
*
* @return 作成したタイトルシーンオブジェクトへのポインタ.
*/
ScenePtr TitleScene::Create()
{
	return ScenePtr(new TitleScene);
}

/**
* コンストラクタ.
*/
TitleScene::TitleScene() : Scene(L"Title")
, _timer(1.0)
, _sprite(nullptr)
{
	_timer.Stop();
}

/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int TitleScene::Update(double delta)
{

	const GamePad gamepad = GetGamePad(GamePadId_1P);
	if (gamepad.buttonDown & anyButton) {
		_timer.Start();
	}
	auto ratio = _timer.GetRatio();

	//_spriteHandle.pGetSprite()->addColor = { -ratio, -ratio, -ratio, 0 };

	//_sprite.addColor = { -ratio, -ratio, -ratio, 0 };
	//_sprite.animeController.Update(delta);

	_spriteBlack.addColor.w = ratio;

	//ScoreManager::GetIncetance().AddScore(1);
	// NumDrawテスト関連.
	
	static NumDraw::Param param;
	static bool test = true;
	param.position = { 1280 * 0.5, 500 };
	param.num = ScoreManager::GetIncetance().GetHighScore();
	param.digit = 10;
	param.scale = { 2.0f, 2.0f };
	param.alignment = NumDraw::ALIGN_CENTER;
	param.verticalAlign = NumDraw::VER_ALIGN_TOP;
	/*
	if (param.num % 60 == 0) {
		test = !test;
	}
	if (test) {
		_numDraw.SetDrawParam(param);
	}
	*/
	param.color = { 1,1,1,1 };
	_numDraw.SetDrawParam(param);
	_numDraw.ReadyToDraw();	// 描画準備完了.
	

	if(_timer.Update(delta)) {
		return ExitCode_Exit;
	}
	return ExitCode_Continue;
}

/**
* シーンの描画.
*/
void TitleScene::Draw() const
{
	Graphics& graphics = Graphics::Get();

	RenderingInfo renderingInfo;
	renderingInfo.rtvHandle = graphics.GetRTVHandle();
	renderingInfo.dsvHandle = graphics.GetDSVHandle();
	renderingInfo.viewport = graphics.viewport;
	renderingInfo.scissorRect = graphics.scissorRect;
	renderingInfo.texDescHeap = graphics.csuDescriptorHeap.Get();
	renderingInfo.matViewProjection = graphics.matViewProjection;

	const PSO& pso = GetPSO(PSOType_Sprite);
	graphics.spriteRenderer.Draw(&_spriteResource2, pso, renderingInfo, nullptr);
	graphics.spriteRenderer.Draw(_numDraw.pGetSpriteResource(), pso, renderingInfo, nullptr);
	graphics.spriteRenderer.Draw(&_blackResource, pso, renderingInfo, nullptr);

	
}
