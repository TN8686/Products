#pragma once

#include "Enemy.h"

class Derota : public Enemy
{
private:
	int m_RandFlame;							// �V���b�g�C���^�[�o���̌��ɂȂ郉���_���Ȓl
	static const int MIN = 1;					// �����̍Œ�l
	//static const int MAX = 5;					// �����̍ő�l�@��������ݒ肷��悤�ɕύX
	std::default_random_engine m_seed;			// ����������̃V�[�h
	std::uniform_int_distribution<int> m_rand;	// ����������

public:
	Derota(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;

	void Draw(int colorID) override;
};