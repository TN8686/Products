#pragma once

#include <random>
#include "DxLib.h"

static constexpr int TEMP_SCREEN_WIDTH = 240;	// 仮画面を横に見た時の幅（つまり実際は高さ）
static constexpr int TEMP_SCREEN_HEIGHT = 320;	// 仮画面を横に見た時の高さ（つまり実際は幅）

static constexpr int TEMP_OFFSCREEN_WIDTH = 16;	// 仮画面を横に見た時の表示外領域の範囲の幅（つまり実際は高さ）
static constexpr int TEMP_OFFSCREEN_HEIGHT = 32;	// 仮画面を横に見た時のの表示外領域の範囲の高さ（つまり実際は幅）

static constexpr int WINDOW_WIDTH = 448;	// 実際に作成するウインドウの幅
static constexpr int WINDOW_HEIGHT = 576;	// 実際に作成するウインドウの高さ

static constexpr float PI = 3.14159265f;	// 円周率

static constexpr int CHARACTER_SIZE = 8;	// 1キャラクタのドット数

static constexpr int ZAPPER_NUM = 3;		// ザッパーの数

enum class ModeId : int {
	TITLE,
	MAIN_GAME,
	NAME_ENTRY
};