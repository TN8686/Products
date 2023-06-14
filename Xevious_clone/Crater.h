#pragma once
#include "Enemy.h"
class Crater :
	public Enemy
{
public:
	Crater(float x, float y, IMAGE_HANDLE_STRUCT* image_handle_p);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;

};

