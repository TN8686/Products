#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"

class Giddospario : public Enemy
{
public:	
	Giddospario(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;


	void GiddosparioBrokenCalc();	// �M�h�X�p���I�j��v�Z
	void GiddosparioBrokenDraw();	// �M�h�X�p���I�j��`��
};