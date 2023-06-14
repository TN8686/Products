#pragma once
#include "Enemy.h"

class Jara : public Enemy
{
private:
	int m_isRightAway;		// 右逃走フラグ
	int m_isLeftAway;		// 左逃走フラグ
	int m_AnimationSum;		// アニメーション加算
	int m_Flame;			// アニメーションの待つフレーム
public:
	Jara(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};