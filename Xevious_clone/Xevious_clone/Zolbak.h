#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"
#include "Barra.h"

class Zolbak : public Enemy
{
	static constexpr int LEVEL_DOWN_VALUE = 2;	// ”j‰ó‚ÌƒŒƒxƒ‹ƒ_ƒEƒ“”
	int* m_pAerialLevel;
public:
	Zolbak(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP,int* pAerialLevel);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};