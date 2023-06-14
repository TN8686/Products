#pragma once
#include "Xevious_clone.h"

class Blaster
{
	const int M_FRAME_MAX = 36;				// ���݃t���[���ő�l

	float m_X;								// X���W
	float m_Y;								// Y���W

	float m_Speed;							// Y�ړ����x
	float m_Acceleration;					// Y�����x
	int m_FrameCount;						// �o�߃t���[��

	int m_Size;								// �T�C�Y
	int m_ColorArray[10] = {0, 1, 2, 3, 0, 1, 0, 2, 3, 0};						// �A�j���[�V�����J���[�z��
	int m_ColorId;							// ���݂̃J���[�z���ID

	int* m_Image;							// �摜�n���h���|�C���^

public:
	Blaster(int* imageHndleP);

	void Calc();
	void Draw();

	void Create(float cursorX, float cursorY);
};

