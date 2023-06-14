#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool isDebug = true;	// ゲーム外デバッグモードフラグ　ウィンドウサイズの関係で使用

	// DxLib_Init()前の処理
	ChangeWindowMode(isDebug);	// trueでウインドウモードで起動 falseでフルスクリーンで起動
	if (isDebug) {
		SetGraphMode(WINDOW_WIDTH + TEMP_SCREEN_HEIGHT, WINDOW_HEIGHT, 16);	// 表示領域の二倍×二倍のサイズ+右側に仮画面表示枠を加えてウインドウを作成
	}
	else {
		SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 16);	// 表示領域の二倍×二倍のサイズでウインドウを作成 
	}
	SetMainWindowText("Xevious_clone");	// ウインドウタイトルを設定

	// DXライブラリの初期化
	if (DxLib_Init() < 0) return -1;

	GameManager gameManager(isDebug);	// ゲームマネージャの実体宣言

	gameManager.MainLoop();				// メインループ

	// DXライブラリの後始末
	DxLib_End();

	// ソフトの終了
	return 0;
}