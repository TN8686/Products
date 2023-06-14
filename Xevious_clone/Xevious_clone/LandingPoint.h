#pragma once
#include "Xevious_clone.h"

class LandingPoint
{
	const int M_FRAME_MAX = 36;			// 存在フレーム最大値
	const float M_GROUND_MOVE = 0.5;	// 背景スクロール　できれば後で背景クラスを参照

	float m_X;
	float m_Y;
	int m_FrameCount;
	int* m_Image;

public:
	LandingPoint(int* imageHandle);

	void Calc();
	void Draw();
	void Create(float cursorX, float cursorY);

	float GetX();
	float GetY();
};

