#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"
#include "UI.h"

class HiddenMessage : public Enemy
{
public:
	HiddenMessage(float x, float y, ScoreManager* pScoreManager, UI* pUI);
	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;

private:
	static const int DRAW_FRAME = 128;

	int m_drawFrameCount;
	UI* m_pUI;
};

