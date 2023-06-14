#include "HiddenMessage.h"

HiddenMessage::HiddenMessage(float x, float y, ScoreManager* pScoreManager, UI* pUI)
	: Enemy(static_cast<int>(EnemyId::HIDDEN_MESSAGE), x, y, 0, 0, NULL, pScoreManager){
	m_drawFrameCount = DRAW_FRAME;
	m_pUI = pUI;
}

void HiddenMessage::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {								// �q�b�g����
		AddScore();								// �X�R�A���Z
		m_isCollision = false;					// ����𖳂���
		m_Status++;								// �C���N�������g�ōς܂���
		m_pUI->SetIsDrawHiddenMessage(true);	// �B�����b�Z�[�W�o�̓t���O��true��
	}

	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		m_Y += groundMove;
		break;

	case static_cast<int>(StatusId::BROKEN):
		if (m_drawFrameCount == 0) {
			m_pUI->SetIsDrawHiddenMessage(false);								// �B�����b�Z�[�W�o�̓t���O��false��
			m_Status = static_cast<int>(StatusId::NON_EXISTENT);				// ��Ԃ�񑶍݂�
		}
		m_drawFrameCount--;
		break;

	default:
		break;
	}
}