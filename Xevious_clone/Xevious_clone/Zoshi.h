#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"
#include <random>
class Zoshi :
    public Enemy
{
private:
	int m_Flame;				//�A�j���[�V�����t���[��
	int m_RandFlame;			//�v���C���[�ɋ߂Â������_���t���[��
	int m_Rand;					//�����_���t���[��
	int m_AnimationSum;			//�A�j���[�V�����p�̉��Z

	static const int MIN = 64;		//���@�ɋ߂Â������_���̍ŏ��l
	static const int MAX = 127;		//���@�ɋ߂Â������_���̍ő�l
	static const int MIN2 = 1;		//�����̃����_���̍ŏ��l
	static const int MAX2 = 32;		//�����̃����_���̍ő�l

	std::default_random_engine m_seed;			// ����������̃V�[�h
	std::uniform_int_distribution<int> m_rand;	// ����������
	std::uniform_int_distribution<int> m_rand2;	// ����������2

public:
	Zoshi(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};

