#include "Zolbak.h"

Zolbak::Zolbak(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, int* pAerialLevel)
	: Enemy(static_cast<int>(EnemyId::ZOLBAK), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 20;
	m_pAerialLevel = pAerialLevel;
}

void Zolbak::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {								// �q�b�g����
		AddScore();								// �X�R�A���Z
		*m_pAerialLevel -= LEVEL_DOWN_VALUE;	// ���x����2�_�E��
		if (*m_pAerialLevel < 0) {				// 0������������0�ɂ���
			*m_pAerialLevel = 0;
		}
		m_isCollision = false;					// ����𖳂���
		m_Status++;								// �C���N�������g�ōς܂���
	}

	switch (m_Status)			// ��Ԃŕ���
	{
	case static_cast<int>(StatusId::ACTIVE):				// �A�N�e�B�u�ȏꍇ
		m_Y += groundMove;		// �w�i���x�ňړ�
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// �j��v�Z
		break;

	default:
		break;
	}
}


// �`��
void Zolbak::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][7], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();																			// �j��`��
		break;

	default:
		break;
	}
}