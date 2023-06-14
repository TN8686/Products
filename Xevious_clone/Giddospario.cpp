#include "Giddospario.h"

Giddospario::Giddospario(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::GIDDO_SPARIO), x, y, type, 0, imageHandleP, scoreManagerP) {

	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[3]);		// X�AY�ړ����x��ݒ�

	m_Score = 1;
}



void Giddospario::Calc(float playerX, float playerY, float groundMove, bool isHit) {
	DamageFunction(isHit);	// ��e��񏈗�

	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		//���x�����W�ɔ��f
		m_X += m_SpeedX;
		m_Y += m_SpeedY;

		break;

	case static_cast<int>(StatusId::BROKEN):
		GiddosparioBrokenCalc();
		break;

	default:
		break;
	}
}

// �`��
void Giddospario::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->spario[24 + colorID], true);
		break;

	case static_cast<int>(StatusId::BROKEN):
		GiddosparioBrokenDraw();
		break;

	default:
		break;
	}
}

// �M�h�X�p���I�j��`��	�U�J�[�g�摜�̐F�̂܂܂Ȃ̂ŉ�
void Giddospario::GiddosparioBrokenCalc() {
	if (m_FCount % 2 == 0 && m_FCount != 0) {	// ���������4�t���[�����@
		m_BrokenImageAnimation++;
	}
	if (m_BrokenImageAnimation >= 4) {			// 8�t���[����ɔ񑶍�
		m_Status = static_cast<int>(StatusId::NON_EXISTENT);
	}
	m_FCount++;

	//���x�����W�ɔ��f
	m_X += m_SpeedX;
	m_Y += m_SpeedY;
}

// �M�h�X�p���I�j��`��	�U�J�[�g�摜�̐F�̂܂܂Ȃ̂ŉ�
void Giddospario::GiddosparioBrokenDraw() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->zakato[4 - m_BrokenImageAnimation], true, m_IsFlip);
}