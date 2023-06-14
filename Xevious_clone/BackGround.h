#pragma once

#include "Xevious_clone.h"
#include "AreaManager.h"

typedef struct {
	int image;	// 参照する画像ハンドル
	float x;	// X座標
	float y;	// Y座標
	int imageX;	// 参照する画像のX座標
	int imageY; // 参照する画像のY座標
}BACK_GROUND;

class BackGround
{
public:
	// コンストラクタ宣言
	BackGround();

	// 計算
	void Init();									// 背景構造体情報初期化

	void Calc(int area, int line, bool isStart);	// 全体計算

	// 描画
	void Draw();

	// ゲッター
	float GetSpeed();								// スクロール速度

private:
	static constexpr float M_SPEED_INIT = 0.5f;		// 初期スクロール速度
	static const int M_BACK_GROUND_NUM = 40;		// 背景構造体配列数
	static const int M_HEIGHT = 8;					// 背景一個あたりの高さ
	static const int M_WIDTH = TEMP_SCREEN_WIDTH;	// 背景一個あたりの幅
	static constexpr int M_IMAGE_X[AreaManager::AREA_NUM] = {
		288, 0, 672, 112, 512, 208, 800, 32, 336, 736, 112, 32, 448, 672, 208, 0 };		// エリアに応じた参照する画像のY座標

	float m_speed;									// 背景スクロール速度
	int m_startImage;								// 開始時の森画像
	int m_mapImage;									// 全体マップ画像
	int m_startImageWidth;							// 開始時の森の画像の幅（90度回転の関係で実際は高さ）
	int m_startImageHeight;							// 開始時の森の画像の高さ（実際は幅）
	int m_mapImageWidth;							// 全体マップ画像の幅（実際は高さ）
	int m_mapImageHeight;							// 全体マップ画像の高さ（実際は幅）
	BACK_GROUND m_backGround[M_BACK_GROUND_NUM];	// 背景構造体配列
};

