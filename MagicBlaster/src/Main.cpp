#include <Windows.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <cmath>
#include <string>
#include <time.h>

#include "Reference.h"
#include "Engine/D3dx12.h"
#include "Engine/Texture.h"
#include "Engine/PSO.h"
#include "Engine/Sprite.h"
#include "Engine/Timer.h"
#include "Engine/GamePad.h"
#include "Engine/Graphics.h"
#include "Engine/Audio.h"

#include "Scene/TitleScene.h"
#include "Scene/MainGameScene.h"
#include "Scene/PauseScene.h"
#include "Scene/GameClearScene.h"
#include "Scene/ResoultScene.h"
#include "Scene/TutorialScene.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

const wchar_t windowClassName[] = L"DX12TutorialApp";
const wchar_t windowTitle[] = L"DirectX12 Magic Blaster";

HWND hwnd = nullptr;

std::unique_ptr<TransitionController> sceneController(new TransitionController);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparan);

using Microsoft::WRL::ComPtr;
unsigned int frameCount = 0;
bool isDebugPouse = false;

bool Render();
bool Update(double delta);

/**
* シーンID.
*/
enum SceneId
{
	SceneId_Title,
	SceneId_MainGame,
	SceneId_Pause,
	SceneId_GameClear,
	SceneId_Resoult,
	SceneId_Tutorial
};

/**
* シーン作成情報.
*/
const SceneCreator creatorList[] = {
  { SceneId_Title, TitleScene::Create },
  { SceneId_MainGame, MainGameScene::Create },
  { SceneId_Pause, PauseScene::Create },
  { SceneId_GameClear, GameClearScene::Create },
  { SceneId_Resoult, ResoultScene::Create },
  { SceneId_Tutorial, TutorialScene::Create }
};

/**
* シーン遷移情報.
*/
const Transition transitionList[] = {
  { SceneId_Title,
	{ TitleScene::ExitCode_Exit, TransitionType::Jump,SceneId_Tutorial } },

  { SceneId_MainGame,
	{ MainGameScene::ExitCode_GameClear, TransitionType::Jump, SceneId_GameClear } },
  { SceneId_MainGame,
	{ MainGameScene::ExitCode_Pause, TransitionType::Push, SceneId_Pause } },
  { SceneId_MainGame,
	{ MainGameScene::ExitCode_Title, TransitionType::Jump, SceneId_Title } },
  { SceneId_MainGame,
	{ MainGameScene::ExitCode_Resoult, TransitionType::Push, SceneId_Resoult } },

  { SceneId_GameClear,
	{ Scene::ExitCode_Exit, TransitionType::Jump, SceneId_Title } },

  { SceneId_Pause,
	{ Scene::ExitCode_Exit, TransitionType::Pop, 0 } },
  { SceneId_Pause,
	{ PauseScene::ExitCode_Title, TransitionType::Jump, SceneId_Title} },


  { SceneId_Resoult,
	{ PauseScene::ExitCode_Title, TransitionType::Jump, SceneId_Title} },

  { SceneId_Tutorial,
	{TutorialScene::ExitCode_Exit, TransitionType::Jump,SceneId_MainGame} }

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	if (FAILED(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {// WIC用にCOMライブラリ初期化 正確には初期化ではなく、現スレッドに設定して使用可能にしている、らしい.
		CoUninitialize();	// 失敗したら後始末をして終了.
		return 0;
	}	

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
	wc.lpszClassName = windowClassName;
	RegisterClassEx(&wc);
	RECT windowRect = { 0, 0, clientWidth, clientHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	hwnd = CreateWindow(
		windowClassName,
		windowTitle,
		WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);


	srand((unsigned int)time(NULL));	// 乱数の初期化.

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	if (!Graphics::Get().Initialize(hwnd, clientWidth, clientHeight)) {
		return 0;
	}

	if (!AudioEngine::Get().Initialize()) {
		return 0;
	}

	InitializeGamePad();

	if (!sceneController->Initialize(
		transitionList, _countof(transitionList), creatorList, _countof(creatorList))) {
		return 0;
	}
	if (!sceneController->Start(SceneId_Title)) {
		return 0;
	}
	
	Timer timer;

	// 時間系の値　簡易的に可変フレームレートやfps計測を実装.
	constexpr double SECONDS_PER_FRAME = 1.0 / 60.0;
	double delta = 0;
	double time = 0;
	double cumulativeTime = 0;
	int skipCount = 0;
	int preFps = 0;
	int	fps = 0;
	bool isFrameSkip = false;

	wchar_t str[256];	// デバッグ出力用.

	MSG msg = {};
	delta = timer.GetFrameDelta();
	while (true) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {
			break;
		}

		// 試験的fps測定.
		if (cumulativeTime >= 1.0) {
			// 出力.
			swprintf_s(str, L"%1f\n", static_cast<float>(fps + preFps) / 2);
			//swprintf_s(str, L"fps%1f\n", static_cast<float>(fps));
			OutputDebugString(str);

			preFps = fps;
			fps = 0;
			cumulativeTime -= 1.0;
		}

		if (!Update(SECONDS_PER_FRAME)) {	// 1フレーム分の時間が経過したものとする.
			break;	// 失敗したら終了.
		}

		if (!isFrameSkip) {	// フレームスキップが発生した場合、描画は行わない.
			skipCount = 0;

			if (!Render()) {	// 描画.
				break;			// 失敗したら終了.
			}

			fps++;
		}
		else {
			skipCount++;	// 一応スキップした回数を数えておく.
		}
		frameCount++;
		// 処理は経過時間基準で可変フレームレートにするための処理.
		// サンプルのTimerクラスはこの使い方を想定していないため、そちらのfps計測が機能していない点に注意 後で改造予定.

		delta = timer.GetFrameDelta();
		time += delta;
		if (time < SECONDS_PER_FRAME) {
			Sleep(static_cast<DWORD>(SECONDS_PER_FRAME * 1000 - time * 1000));
			isFrameSkip = false;
		}
		else {
			//swprintf_s(str, L"frame skip\n");
			//OutputDebugString(str);
			isFrameSkip = true;
		}
		// 1フレーム分の時間が経過したものとする.
		time -= SECONDS_PER_FRAME;
		cumulativeTime += SECONDS_PER_FRAME;
		//swprintf_s(str, L"%1f\n", static_cast<float>(time));
		//OutputDebugString(str);
	}

	sceneController->Stop();
	AudioEngine::Get().Destroy();
	Graphics::Get().Finalize();

	CoUninitialize();	// COMライブラリの後始末.
	return 0;
}




LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	GamePad& gamepad = GetGamePad(GamePadId_1P);
	switch (msg) {
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE) {
			PostQuitMessage(0);
			DestroyWindow(hwnd);
			return 0;
		}
		switch (wparam) {
		case 'W': gamepad.buttons |= GamePad::DPAD_UP; break;
		case 'A': gamepad.buttons |= GamePad::DPAD_LEFT; break;
		case 'S': gamepad.buttons |= GamePad::DPAD_DOWN; break;
		case 'D': gamepad.buttons |= GamePad::DPAD_RIGHT; break;
		case 'J': gamepad.buttons |= GamePad::X; break;
		case 'K': gamepad.buttons |= GamePad::Y; break;
		case VK_SPACE: gamepad.buttons |= GamePad::A; break;
		case VK_RETURN: gamepad.buttons |= GamePad::START; break;
		case VK_CONTROL: gamepad.buttons |= GamePad::SELECT; break;
		case 'H': gamepad.buttons |= GamePad::L; break;
		case 'L': gamepad.buttons |= GamePad::R; break;
		case 'U': gamepad.buttons |= GamePad::LT; break;
		case 'O': gamepad.buttons |= GamePad::RT; break;
		}
		break;

	case WM_KEYUP:
		switch (wparam) {
		case 'W': gamepad.buttons &= ~GamePad::DPAD_UP; break;
		case 'A': gamepad.buttons &= ~GamePad::DPAD_LEFT; break;
		case 'S': gamepad.buttons &= ~GamePad::DPAD_DOWN; break;
		case 'D': gamepad.buttons &= ~GamePad::DPAD_RIGHT; break;
		case 'J': gamepad.buttons &= ~GamePad::X; break;
		case 'K': gamepad.buttons &= ~GamePad::Y; break;
		case VK_SPACE: gamepad.buttons &= ~GamePad::A; break;
		case VK_RETURN: gamepad.buttons &= ~GamePad::START; break;
		case VK_CONTROL: gamepad.buttons &= ~GamePad::SELECT; break;
		case 'H': gamepad.buttons &= ~GamePad::L; break;
		case 'L': gamepad.buttons &= ~GamePad::R; break;
		case 'U': gamepad.buttons &= ~GamePad::LT; break;
		case 'O': gamepad.buttons &= ~GamePad::RT; break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		DestroyWindow(hwnd);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

// 描画.
bool Render() {

	Graphics& graphics = Graphics::Get();
	if (!graphics.BeginRendering()) {
		return false;
	}
	graphics.spriteRenderer.Begin(graphics.currentFrameIndex);
	sceneController->Draw();

	graphics.spriteRenderer.End();
	if (!graphics.EndRendering()) {
		return false;
	}
	return true;
}

/**
* アプリケーションの状態を更新する.
*/
bool Update(double delta)
{
	bool ret = true;
#ifndef NDEBUG
	// デバッグモードでは、ゲームの動作停止、コマ送り機能がONに.
	static uint32_t buttonDown = 0;
	static uint32_t prevButtons = 0;
	static bool addOneFrame = false;

	GamePad& gamepad = GetGamePad(GamePadId_1P);
	if (gamepad.buttonDown & GamePad::SELECT) {
		if (isDebugPouse) {
			isDebugPouse = false;
			gamepad.prevButtons = prevButtons;

		}
		else {
			isDebugPouse = true;
			prevButtons = gamepad.prevButtons;
		}
	}

	if (gamepad.buttonDown & GamePad::LT) {
		AABB2DObj::SetIsDraw(!AABB2DObj::GetIsDraw());
	}

	if (isDebugPouse && gamepad.buttonDown & GamePad::RT) {
		addOneFrame = true;
		gamepad.prevButtons = prevButtons | GamePad::RT;
	}

	UpdateGamePad(delta);
	if (!isDebugPouse || addOneFrame) {
		AudioEngine::Get().Update();
		ret = sceneController->Update(delta);
	}

	if (addOneFrame) {
		addOneFrame = false;
		prevButtons = gamepad.prevButtons | GamePad::SELECT;
	}
#else
	UpdateGamePad(delta);
	AudioEngine::Get().Update();

	ret = sceneController->Update(delta);
#endif
	return ret;
}
