#include "Sol.h"
Sol::Sol(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::SOL), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_CharcterId = static_cast<int>(EnemyId::SOL);

	m_isDraw = false;
	m_FrameCount = 0;
	m_AnimationCount = 0;

	m_Score = 200;

}

void Sol::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {				// qbg»è
		AddScore();				// XRAÁZ
		m_isCollision = false;	// »èð³­·

		if (m_isDraw) {
			m_Status++;			// óÔðBROKENÉ
		}
		else {
			m_isDraw = true;
		}
	}

	switch (m_Status)			// óÔÅªò
	{
	case static_cast<int>(StatusId::ACTIVE):				// ANeBuÈê
		if (m_isDraw) {
			m_FrameCount++;
			if (m_FrameCount % 16 == 0) {
				if (m_AnimationCount == 6)
				{
					m_isCollision = true;
				}
				else {
					m_AnimationCount++;
				}
			}
		}

		m_Y += groundMove;		// wi¬xÅÚ®
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// jóvZ
		break;

	default:
		break;
	}
}

// eª½Á½É`æ
void Sol::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		if (m_isDraw) {
			DrawGraph((int)m_Y, (int)m_X - LITTLE_SIZE, m_imageHandleP->big[colorID][7 + m_AnimationCount], TRUE);
		}
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// jó`æ
		break;

	default:
		break;
	}
}