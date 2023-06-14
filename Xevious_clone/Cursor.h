#pragma once

#include "Xevious_clone.h"

class Cursor
{
private:
	static constexpr int CURSOR_RANGE = 96;

	float m_X;								// X座標
	float m_Y;								// Y座標

	int m_ImageId;

	int* m_Image;							// 画像ハンドルポインタ

public:
	Cursor(int* imageHandle);

	void Calc(float playerX, float playerY, bool onCursor, bool isShotAfter, int frameCount);
	void Draw();

	float GetX();
	float GetY();
};

