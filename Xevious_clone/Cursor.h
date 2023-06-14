#pragma once

#include "Xevious_clone.h"

class Cursor
{
private:
	static constexpr int CURSOR_RANGE = 96;

	float m_X;								// X���W
	float m_Y;								// Y���W

	int m_ImageId;

	int* m_Image;							// �摜�n���h���|�C���^

public:
	Cursor(int* imageHandle);

	void Calc(float playerX, float playerY, bool onCursor, bool isShotAfter, int frameCount);
	void Draw();

	float GetX();
	float GetY();
};

