#pragma once
#include "Enemy.h"

class Torkan :
    public Enemy
{
private:
	int m_Flame;				//�A�j���[�V�����t���[��
	int m_RandFlame;			//�v���C���[�ɋ߂Â������_���t���[��
	int m_IsStop;				//��~����m_X��m_Y���~�߂�t���O
	int m_IsReverse;			//�A���Ă����p�̃t���O
	int m_AnimationSum;			//�A�j���[�V�����p�̉��Z
	int m_IsShotAfter;			//�e���ˏI���t���O

	static const int MIN = 64;		//���@�ɋ߂Â������_���̍ŏ��l
	static const int MAX = 127;		//���@�ɋ߂Â������_���̍ő�l

public:
	Torkan(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};

