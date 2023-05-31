#include <cmath>
#include "TutorialScene.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"
#include "../Reference.h"

namespace {
	constexpr int anyButton = (GamePad::A | GamePad::B | GamePad::X | GamePad::Y | GamePad::DPAD_RIGHT);
}

// 諸々仮置き.

using namespace DirectX;

/**
* シーンを読み込む.
*/
bool TutorialScene::Load()
{
	// PtrVector関連テスト.
	Graphics& graphics = Graphics::Get();

	// 画面の中央.
	Point2D center = { graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f };

	_parent.SetLocalPosition(center);
	_sprBack.SetLocalPosition(center);

	// リサイズ.
	_spr.resize(SPR_NUM);

	// 1:1で登録　また、スプライトの親オブジェクトを登録.
	_resBack.Register(&_sprBack);
	for (int i = 0; i < SPR_NUM; ++i) {
		_res[i].Register(&_spr.at(i));	// 管理者登録.
		_spr.at(i).SetParent(&_parent);		// 親登録.
		_spr.at(i).SetLocalPosition({ graphics.viewport.Width * i, 0 });	// X座標をズラす.
	}


	// 暗幕.
	_resBlack.Register(&_sprBlack);
	_sprBlack.rectOffset.right = graphics.viewport.Width;
	_sprBlack.rectOffset.bottom = graphics.viewport.Height;
	_sprBlack.addColor.w = 1;

	// 画像ロード開始.
	graphics.texMap.Begin();

	if (!graphics.texMap.LoadFromFile(_resBack.GetTexture(), L"Res/tutorial_back.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(_res[0].GetTexture(), L"Res/tutorial_move.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(_res[1].GetTexture(), L"Res/tutorial_downJump.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(_res[2].GetTexture(), L"Res/tutorial_bullet.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(_res[3].GetTexture(), L"Res/tutorial_fixed.png")) {
		return false;
	}
	if (!graphics.texMap.LoadFromFile(_res[4].GetTexture(), L"Res/tutorial_blast.png")) {
		return false;
	}

	if (!graphics.texMap.CreateRect(_resBlack.GetTexture(), 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}

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
ScenePtr TutorialScene::Create()
{
	return ScenePtr(new TutorialScene);
}

/**
* コンストラクタ.
*/
TutorialScene::TutorialScene() : Scene(L"Tutorial")
, _timer(0.5)
, _blackTimer(1.0)
, _page(0)
, _toBlack(false)
, _toRight(true)
{
	//_timer.Stop();
}

/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int TutorialScene::Update(double delta)
{
	_timer.Update(delta);
	_blackTimer.Update(delta);
	const GamePad gamepad = GetGamePad(GamePadId_1P);
	
	if (_blackTimer.IsReached()) {
		if (_timer.IsReached() && gamepad.buttonDown & GamePad::START) {	// チュートリアルをスキップ.
			_toBlack = true;		// 段々黒に.
			_blackTimer.Reset();	// タイマーを再スタート.

		}
		if (_timer.IsReached() && gamepad.buttonDown & anyButton) {
			_toRight = true;
			if (_page < SPR_NUM - 1) {		// 次のページへ.
				_page++;
				_timer.Reset();			// タイマーを再スタート.
			}
			else {
				_toBlack = true;		// 段々黒に.
				_blackTimer.Reset();	// タイマーを再スタート.
			}
		}
		else if (_timer.IsReached() && _page > 0 && gamepad.buttonDown & GamePad::DPAD_LEFT) {	// 戻る操作.
			_page--;
			_toRight = false;
			_timer.Reset();
		}
	}

	auto ratio = _timer.GetRatio();
	if (_toRight) {
		_parent.SetLocalPosition({ (clientWidth * 1.5f) - (clientWidth * (static_cast<float>(_page) + ratio)), clientHeight * 0.5f });
	}
	else {
		_parent.SetLocalPosition({ (clientWidth * 1.5f) - (clientWidth * (static_cast<float>(_page) + 1 + (1.0f - ratio))), clientHeight * 0.5f });
	}
	
	auto blackRatio = _blackTimer.GetRatio();
	if (_toBlack) {
		_sprBlack.addColor.w = blackRatio;
	}
	else {
		_sprBlack.addColor.w = 1 - blackRatio;
	}

	if(_toBlack && _blackTimer.IsReached()) {	// 最終ページで暗転後.
		return ExitCode_Exit;
	}
	return ExitCode_Continue;
}

/**
* シーンの描画.
*/
void TutorialScene::Draw() const
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
	graphics.spriteRenderer.Draw(&_resBack, pso, renderingInfo, nullptr);
	for (auto& e : _res) {
		graphics.spriteRenderer.Draw(&e, pso, renderingInfo, nullptr);
	}
	graphics.spriteRenderer.Draw(&_resBlack, pso, renderingInfo, nullptr);

	
}
