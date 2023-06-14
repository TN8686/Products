#pragma once
#include "Xevious_clone.h"

class Blaster
{
	const int M_FRAME_MAX = 36;				// 存在フレーム最大値

	float m_X;								// X座標
	float m_Y;								// Y座標

	float m_Speed;							// Y移動速度
	float m_Acceleration;					// Y加速度
	int m_FrameCount;						// 経過フレーム

	int m_Size;								// サイズ
	int m_ColorArray[10] = {0, 1, 2, 3, 0, 1, 0, 2, 3, 0};						// アニメーションカラー配列
	int m_ColorId;							// 現在のカラー配列のID

	int* m_Image;							// 画像ハンドルポインタ

public:
	Blaster(int* imageHndleP);

	void Calc();
	void Draw();

	void Create(float cursorX, float cursorY);
};

