#include "Grobda.h"
Grobda::Grobda(float x, float y, int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::GROBDA), x, y, type, brokenType, imageHandleP, scoreManagerP) {
	switch (m_Type)
	{
	case 0:
		m_Score = 20;
		break;
	default:
		break;
	}
}

void Grobda::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);
	{
		switch (m_Status)							// ó‘Ô‚Å•ªŠò
		{
		case static_cast<int>(StatusId::ACTIVE):	// ƒAƒNƒeƒBƒu‚Èê‡
			m_Y += groundMove;						// ”wŒi‘¬“x‚ÅˆÚ“®
			break;

		case static_cast<int>(StatusId::BROKEN):
			BrokenCalc(groundMove);	// ”j‰óŒvZ
			break;

		default:
			break;
		}
	}
}

//•`‰æ
void Grobda::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][68], TRUE);	// •`‰æ
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();																			// ”j‰ó•`‰æ
		break;

	default:
		break;
	}
}