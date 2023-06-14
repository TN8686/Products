#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"

class Spario : public Enemy
{
private:
	bool m_IsMove;
public:
	Spario(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP);
	void Calc() override;
	void Draw(int colorID) override;
};