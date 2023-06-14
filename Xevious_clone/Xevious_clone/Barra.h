#pragma once
#include "Enemy.h"
#include "Xevious_clone.h"

class Barra : public Enemy
{
public:
	Barra(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};