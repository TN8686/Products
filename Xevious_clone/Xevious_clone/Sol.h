#pragma once
#include "Enemy.h"
class Sol :
	public Enemy
{
private:
	bool m_isDraw;
	int m_FrameCount;
	int m_AnimationCount;
public:
	Sol(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);
	
	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};

