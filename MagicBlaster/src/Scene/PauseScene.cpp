#include "PauseScene.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"
#include "../ScoreManager.h"

using namespace DirectX;

/**
* シーンを読み込む.
*/
bool PauseScene::Load()
{
	
	Graphics& graphics = Graphics::Get();
	
	graphics.texMap.Begin();
	if (!graphics.texMap.LoadFromFile(texture, L"Res/FontPhenomena.png")) {
		return false;
	}
	ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
	graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	fontCellList = LoadFontFromFile(L"Res/FontPhenomena.fnt");
	if (fontCellList.list.empty()) {
		return false;
	}
	
	graphics.WaitForGpu();
	graphics.texMap.ResetLoader();

	const XMFLOAT2 center(graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f);
	static const char text[] = "PAUSE";
	static const XMFLOAT2 scale(1, 1);
	const float len = GetTextWidth(fontCellList, text) * scale.x;
	XMFLOAT3 textPos(center.x - len * 0.5f, 50, 0.01f);
	for (const char c : text) {
		sprFont.push_back(Sprite(nullptr, textPos, 0, scale));
		sprFont.back().SetCellIndex(c);
		sprFont.back().color[0] = XMFLOAT4(0.5f, 1.0f, 1.0f, 1.0f);
		sprFont.back().color[1] = XMFLOAT4(0.25f, 0.125f, 1.0f, 1.0f);
		textPos.x += fontCellList.list[c].xadvance * scale.x;
	}
	
	return true;
}

/**
* タイトルシーンオブジェクトを作成する.
*
* @return 作成したタイトルシーンオブジェクトへのポインタ.
*/
ScenePtr PauseScene::Create()
{
	return ScenePtr(new PauseScene);
}

/**
* コンストラクタ.
*/
PauseScene::PauseScene() : Scene(L"Pause")
{
}

/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int PauseScene::Update(double delta)
{

	const GamePad gamepad = GetGamePad(GamePadId_1P);
	if (gamepad.buttonDown & GamePad::START) {
		return ExitCode_Exit;	// 元のシーンへ戻る.
	}
	if (gamepad.buttonDown & GamePad::L) {
		ScoreManager::GetIncetance().HighScoreSet();
		return ExitCode_Title;	// タイトルへのJump.
	}
	return ExitCode_Continue;
}

/**
* シーンの描画.
*/
void PauseScene::Draw() const
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
	graphics.spriteRenderer.Draw(sprFont, fontCellList.list.data(), pso, texture, renderingInfo);
}
