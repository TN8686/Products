#include "ResoultScene.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"
#include "../ScoreManager.h"

using namespace DirectX;

/**
* シーンを読み込む.
*/
bool ResoultScene::Load()
{
	
	Graphics& graphics = Graphics::Get();
	graphics.texMap.Begin();

	_numDraw.Load();

	if (!graphics.texMap.LoadFromFile(_textResouce.GetTexture(), L"Res/scoreText.png")) {
		return false;
	}
	if (!graphics.texMap.CreateRect(_rect, 1, 1, 0, 0, 0, 0, L"Rect01")) {
		return false;
	}

	ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
	graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	graphics.WaitForGpu();
	graphics.texMap.ResetLoader();

	_spriteBg.rectOffset.bottom = clientHeight;
	_spriteBg.rectOffset.right = clientWidth;
	_spriteBlack = _spriteBg;

	_spriteBg.addColor.w = 0.5f;


	_textResouce.CreateCellList(2, 4, 40, 16);
	
	_spriteText.resize(3);
	for (auto& e : _spriteText) {
		_textResouce.Register(&e);
		e.scale = { 3.0f, 3.0f };
		e.cameraRelative = false;
	}
	_spriteText.at(0).animeController.SetCellIndex(1);	// Score.

	_spriteText.at(1).animeController.SetCellIndex(0);	// High.
	_spriteText.at(2).animeController.SetCellIndex(1);	// Score.

	_spriteText.at(0).SetLocalPosition({ clientWidth * 0.5 - 180, clientHeight * 0.5 });

	_spriteText.at(1).SetLocalPosition({ clientWidth * 0.5 - 270, clientHeight * 0.5 + 128 });
	_spriteText.at(2).SetLocalPosition({ clientWidth * 0.5 - 180, clientHeight * 0.5 + 128 });

	return true;
}

/**
* タイトルシーンオブジェクトを作成する.
*
* @return 作成したタイトルシーンオブジェクトへのポインタ.
*/
ScenePtr ResoultScene::Create()
{
	return ScenePtr(new ResoultScene);
}

/**
* コンストラクタ.
*/
ResoultScene::ResoultScene() : Scene(L"Pause")
	, _blackTimer(1.0f)
	, _scoreTimer(1.0f)
	, _isHighScoreUpdate(false)
{
	_blackTimer.Stop();
}

/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int ResoultScene::Update(double delta)
{

	const GamePad gamepad = GetGamePad(GamePadId_1P);
	auto& scoreMgr = ScoreManager::GetIncetance();
	scoreMgr.Update(delta);
	_blackTimer.Update(delta);
	_scoreTimer.Update(delta);

	_spriteBlack.addColor.w = _blackTimer.GetRatio();

	if (_blackTimer.IsReached()) {	// タイマー到達でジャンプ.
		scoreMgr.HighScoreSet();
		return ExitCode_Title;	// タイトルへのJump.
	}

	// スコア表示.
	NumDraw::Param param;
	param.num = scoreMgr.GetScore();
	param.digit = 8;
	param.scale = { 2.0f, 2.0f };
	param.position.x = clientWidth * 0.5 + 100;
	param.position.y = clientHeight * 0.5;
	param.alignment = NumDraw::ALIGN_CENTER;
	param.verticalAlign = NumDraw::VER_ALIGN_CENTER;
	_numDraw.SetDrawParam(param);

	if (_scoreTimer.IsReachedFrame()) {
		if (scoreMgr.HighScoreSet()) {
			_isHighScoreUpdate = true;
		}
	}

	if (_isHighScoreUpdate) {
		param.scale = { 2.5f, 2.5f };
		param.color.b = 0;
	}

	param.num = scoreMgr.GetHighScore();
	param.position.y += 128;
	_numDraw.SetDrawParam(param);

	_numDraw.ReadyToDraw();

	if (_scoreTimer.IsReached() && (gamepad.buttonDown & GamePad::START || gamepad.buttonDown & GamePad::A)) {
		_blackTimer.Start();	// 暗転演出タイマースタート.
	}
	return ExitCode_Continue;
}

/**
* シーンの描画.
*/
void ResoultScene::Draw() const
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
	graphics.spriteRenderer.Draw(&_spriteBg, nullptr, pso, _rect, renderingInfo);
	graphics.spriteRenderer.Draw(&_textResouce, pso, renderingInfo);
	graphics.spriteRenderer.Draw(_numDraw.pGetSpriteResource(), pso, renderingInfo);
	graphics.spriteRenderer.Draw(&_spriteBlack, nullptr, pso, _rect, renderingInfo);
}
