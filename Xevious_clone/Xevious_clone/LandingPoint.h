#pragma once
#include "Xevious_clone.h"

class LandingPoint
{
	const int M_FRAME_MAX = 36;			// ���݃t���[���ő�l
	const float M_GROUND_MOVE = 0.5;	// �w�i�X�N���[���@�ł���Ό�Ŕw�i�N���X���Q��

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

