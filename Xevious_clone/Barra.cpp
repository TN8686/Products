#include "Barra.h"

// Calc�ȊO�̎d�l�͋��ʂȂ̂Ńg�[���C�h���Q�Ƃ��Ă�������
Barra::Barra(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::BARRA), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 10;
}

/* �n��G��Calc�̈����͋󒆓G�̕��ɉ����āA�J�[�\�����W�A���e�n�_���W�����܂�
* �O���u�_�[�ȊO�͎g�p���܂��񂪁A���Ȃ炸���̈�����Calc�ɂ��ĉ�����
* �����Ɍ��݂̒n��X�N���[�����x�Ɣ�e�������悤�ɂ��܂���
* �n��X�N���[�����x���g�p����ꍇ�͂�������Q�Ƃ��Ă�������
*/
void Barra::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// ��e��񏈗�

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
void Barra::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][23], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// �j��`��
		break;

	default:
		break;
	}
}