#pragma once
#include "Xevious_clone.h"

class Zapper
{
private:
	// �萔
	static const int M_SPEED = 6;
	static const int M_SIZE = 16;

	bool m_OnActive;	// �A�N�e�B�u���
	float m_X;			// X���W
	float m_Y;			// Y���W
	float m_Speed;		// ���x�iY�j
	int m_Width;		// ��
	int m_Height;		// ����
	int* m_Image;		// �摜�n���h���|�C���^
	int m_AnimationId;	// �A�j���[�V����ID
	bool m_IsFlip;		// ���E���]
	double m_Angle;		// ���E���]�p�̉�]�̒l

public:
	Zapper(int* ImageHandle);

	void Calc();
	void Draw();

	void SetOnActive(bool onActive);
	void SetAnimationId(int animationId);
	void SetIsTurn(bool isTurn);
	void SetX(float X);
	void SetY(float Y);

	bool GetOnActive();
	float GetX();
	float GetY();
};