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
		switch (m_Status)							// ��Ԃŕ���
		{
		case static_cast<int>(StatusId::ACTIVE):	// �A�N�e�B�u�ȏꍇ
			m_Y += groundMove;						// �w�i���x�ňړ�
			break;

		case static_cast<int>(StatusId::BROKEN):
			BrokenCalc(groundMove);	// �j��v�Z
			break;

		default:
			break;
		}
	}
}

//�`��
void Grobda::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][68], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();																			// �j��`��
		break;

	default:
		break;
	}
}