#include "DxLib.h"
#include "Utility/Reference.h"
#include "Engine/Application.h"

#include "Utility/MyArray.h"
#include <algorithm>

using namespace Utility;

Engine::Application app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// DxLib_Init()前の処理
	ChangeWindowMode(true);	// trueでウインドウモードで起動 falseでフルスクリーンで起動
	SetGraphMode(WindowSize::WIDTH, WindowSize::HEIGHT, 16);	// ウインドウを作成 
	SetMainWindowText("SpringMotion");	// ウインドウタイトルを設定

	// DXライブラリの初期化
	if (DxLib_Init() < 0) {
		return -1;
	}
	
	app.initialize();

	app.run();

	app.shutdown();

	// DXライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}