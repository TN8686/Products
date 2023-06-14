#include "Crater.h"
Crater::Crater(float x, float y,  IMAGE_HANDLE_STRUCT* imageHandleP)
	: Enemy(static_cast<int>(EnemyId::CRATER), x, y, 0, 0, imageHandleP, NULL){
	m_isCollision = false;
	
}

void Crater::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (m_Status == static_cast<int>(StatusId::ACTIVE)) {
		m_Y += groundMove;
	}
}

//•`‰æ
void Crater::Draw(int colorID) {
	if (m_Status == static_cast<int>(StatusId::ACTIVE)) {
		DrawGraph((int)this->m_Y, (int)this->m_X, this->m_imageHandleP->little[0][78 + colorID], TRUE);
	}
}