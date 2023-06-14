#pragma once
#include "Enemy.h"
#include "Xevious_clone.h"

class Grobda : public Enemy
{
public:
	Grobda(float x, float y, int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);


	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};