#pragma once

namespace Utility {
	struct WindowSize {
		static const int WIDTH;		// ウインドウの幅
		static const int HEIGHT;	// ウインドウの高さ
	};

	struct Frame {
		static const long long MICROSECONDS_PER_FRAME;	// 1フレームあたりの時間(マイクロ秒) 秒間60フレーム想定.
	};

}