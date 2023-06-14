#include "SPFlag.h"
SPFlag::SPFlag(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::SPFLAG), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 100;
	m_isDraw = false;
}

void SPFlag::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {				// �q�b�g����
		if (m_isCollision) {
			AddScore();				// �X�R�A���Z
			m_isCollision = false;	// ����𖳂���
			m_isDraw = true;
		}
		else {
			m_Status = static_cast<int>(StatusId::NON_EXISTENT);
		}

	}
	m_Y += groundMove;		// �w�i���x�ňړ�
}

// �e�������������ɕ`��
void SPFlag::Draw(int colorID) {
	if (m_isDraw && m_Status == static_cast<int>(StatusId::ACTIVE)) {
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][63], TRUE);
	}
}