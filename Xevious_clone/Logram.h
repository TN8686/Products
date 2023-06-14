#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"

class Logram : public Enemy
{
private:
	int m_Animation;							// �A�j���[�V����ID
	int m_RandFlame;							// �V���b�g�C���^�[�o���̌��ɂȂ郉���_���Ȓl
	bool m_isAnimationIncrement;				// �V���b�g�A�j���[�V�����C���N�������g�t���O
	static constexpr int MIN = 4;					// �����̍Œ�l
	//static const int MAX = 32;				// �����̍ő�l�@��������ݒ肷��悤�ɕύX
	std::default_random_engine m_seed;			// ����������̃V�[�h
	std::uniform_int_distribution<int> m_rand;	// ����������

public:
	Logram(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerPint, Enemy* pBozaLogram);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};