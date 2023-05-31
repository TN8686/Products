#include "GameClearScene.h"
#include "../Engine/Graphics.h"
#include "../Engine/GamePad.h"
#include "../Engine/PSO.h"

using namespace DirectX;

const Cell bgCellList[] = {
  { { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 800, 600 }, { 0, 0 }, 0 } };

/**
* タイトルシーンオブジェクトを作成する.
*
* @return 作成したタイトルシーンオブジェクトへのポインタ.
*/
ScenePtr GameClearScene::Create()
{
	return ScenePtr(new GameClearScene);
}

/**
* コンストラクタ.
*/
GameClearScene::GameClearScene() : Scene(L"GameClear")
{
}

/**
* シーンを読み込む.
*/
bool GameClearScene::Load()
{
    Graphics& graphics = Graphics::Get();

    //TextureLoader loader;
    //loader.Begin(graphics.csuDescriptorHeap);
    graphics.texMap.Begin();
    if (!graphics.texMap.LoadFromFile(texture[0], L"Res/UnknownPlanet.png")) {
        return false;
    }
    if (!graphics.texMap.LoadFromFile(texture[1], L"Res/FontPhenomena.png")) {
        return false;
    }
    ID3D12CommandList* ppCommandLists[] = { graphics.texMap.End() };
    graphics.commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    fontCellList = LoadFontFromFile(L"Res/FontPhenomena.fnt");
    if (fontCellList.list.empty()) {
        return false;
    }
    graphics.WaitForGpu();

    // 背景.
    const XMFLOAT2 center(graphics.viewport.Width * 0.5f, graphics.viewport.Height * 0.5f);
    sprBg.push_back(Sprite(nullptr, XMFLOAT3(center.x, center.y, 1.0f)));

    const char text[] = "GAME CLEAR";
    const char len = GetTextWidth(fontCellList, text);
    XMFLOAT3 pos(center.x - len * 0.5f, center.y, 0.8f);
    XMVECTOR color[2] = { { 0.2f, 1, 1, 1 }, { 0.1f, 0.1f, 0.8f, 1 } };
    const XMVECTOR colorFactor = { 0.9f, 0.9f, 0.9f, 1 };
    for (const char c : text) {
        sprFont.push_back(Sprite(nullptr, pos));
        sprFont.back().SetCellIndex(c);
        color[0] *= colorFactor;
        color[1] *= colorFactor;
        XMStoreFloat4(&sprFont.back().color[0], color[0]);
        XMStoreFloat4(&sprFont.back().color[1], color[1]);
        pos.x += fontCellList.list[c].xadvance;
    }
    waitTimer = 1;
    return true;
}


/**
* シーンの更新.
*
* @param delta 前回の呼び出しからの経過時間.
*
* @return 終了コード.
*/
int GameClearScene::Update(double delta)
{
    if (waitTimer > 0) {
        waitTimer -= delta;
    }
    else {
        const GamePad gamepad = GetGamePad(GamePadId_1P);
        if (gamepad.buttonDown & (GamePad::A | GamePad::B | GamePad::START)) {
            return ExitCode_Exit;
        }
    }
    return ExitCode_Continue;

}

/**
* シーンの描画.
*/
void GameClearScene::Draw() const
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
    graphics.spriteRenderer.Draw(sprBg, bgCellList, pso, texture[0], renderingInfo);
    graphics.spriteRenderer.Draw(sprFont, fontCellList.list.data(), pso, texture[1], renderingInfo);

}
