#include "Application.h"
#include "DxLib.h"
#include "HitChecker.h"
#include "../Scene/GameScene.h"
#include "../Utility/Reference.h"

#include <string>

using namespace Utility;
using namespace Scene;

namespace Engine {

	// コンストラクタ.
	Application::Application()
		: pScene_(nullptr)
		, firstTime_(0)
	{
	}

	// 初期化.
	bool Application::initialize() {
		
		// 描画先画面を裏画面にセット
		SetDrawScreen(DX_SCREEN_BACK);

		HitChecker::getInstance()->initialize();	// コリジョンマネージャを初期化.

		pScene_ = new GameScene();

		return true;
	}

	// 実行.
	bool Application::run() {

		// ホントは諸々SceneManagerを介する.

		// ロード.
		pScene_->load();

		pScene_->init();

		// TODO フレームレート計測周りをクラス化.
		static bool isSyorioti;
		static long long fpsTime, fpsFirstTime, fps, fpsCount;
		fpsFirstTime = GetNowHiPerformanceCount();
		fps = 0;
		std::string str;
		// とりあえずのゲームループ.
		int ret = Scene::EXIT_CODE_CONTINUE;
		while (ProcessMessage() == 0 && ret == Scene::EXIT_CODE_CONTINUE) {

			// 現在のカウントを取得する
			firstTime_ = GetNowHiPerformanceCount();

			// fps計測用.
			if (firstTime_ - fpsFirstTime >= Frame::MICROSECONDS_PER_FRAME * 60) {
				fpsFirstTime = GetNowHiPerformanceCount();
				fps = fpsCount;
				fpsCount = 0;
			}

			// 更新.
			ret = pScene_->update();

			// 描画.
			ClearDrawScreen();	// 画面を初期化.
			pScene_->render();	// 描画.

			// fps表示.
			str = std::to_string(fps);
			DrawString(WindowSize::WIDTH - 16, WindowSize::HEIGHT - 16, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

			// 裏画面の内容を表画面に反映させる
			ScreenFlip();

			// 1フレーム分の時間が経過するまで待機.
			while (GetNowHiPerformanceCount() - firstTime_ < Frame::MICROSECONDS_PER_FRAME) {
				Sleep(0);
			}

			++fpsCount;
		}

		// 終了処理.
		pScene_->unload();

		return true;
	}

	// 終了.
	bool Application::shutdown() {
		delete pScene_;

		return true;
	}

}
