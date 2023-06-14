#pragma once
#include "Enemy.h"

class Jara : public Enemy
{
private:
	int m_isRightAway;		// �E�����t���O
	int m_isLeftAway;		// �������t���O
	int m_AnimationSum;		// �A�j���[�V�������Z
	int m_Flame;			// �A�j���[�V�����̑҂t���[��
public:
	Jara(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};