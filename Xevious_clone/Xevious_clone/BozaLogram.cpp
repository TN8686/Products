#include "BozaLogram.h"

BozaLogram::BozaLogram(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, Enemy** ppBozaLogram[])
	: Enemy(static_cast<int>(EnemyId::BARRA), x, y, 0, 1, imageHandleP, scoreManagerP) {

	m_Score = 200;			// �X�R�A
	m_isCalcFiast = true;	// ���񏈗��t���O

	for (int i = 0; i < BOZA_LOGRAM_NUM; i++) {
		m_ppBozaLogram[i] = ppBozaLogram[i];		// �_�u���|�C���^����
	}
}

void BozaLogram::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// ��e��񏈗�

	switch (m_Status)			// ��Ԃŕ���
	{
	case static_cast<int>(StatusId::ACTIVE):				// �A�N�e�B�u�ȏꍇ
		ActiveCalc(groundMove);
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// �j��v�Z
		break;

	default:
		break;
	}
}

// �`��
void BozaLogram::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][62], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// �j��`��
		break;

	default:
		break;
	}
}

void BozaLogram::ActiveCalc(float groundMove) {
	if (m_isCalcFiast) {								// ���񏈗�
		Enemy* tempP = *m_ppBozaLogram[0];				// �G�l�~�[�}�l�[�W�����ł̖{�̂Əネ�O�����̊i�[�ꏊ�����ւ���
		*m_ppBozaLogram[0] = *m_ppBozaLogram[4];
		*m_ppBozaLogram[4] = tempP;

		m_isCalcFiast = false;							// ���񏈗��t���O��false��
	}


	if (m_Score != M_LOGRAM_BROKEN_SCORE) {													// ���O�����j���̃X�R�A�łȂ�������
		for (int i = 0; i < BOZA_LOGRAM_NUM - 1; i++) {										// �q���O�����̏�Ԃ��`�F�b�N����
			if (m_ppBozaLogram[i] != NULL && (*m_ppBozaLogram[i])->GetStatus() == static_cast<int> (StatusId::BROKEN)) {	// �j���ԂɂȂ��Ă�����
				m_Score = M_LOGRAM_BROKEN_SCORE;											// ���g�̃X�R�A��ύX
			}
		}
	}

	m_Y += groundMove;		// �w�i���x�ňړ�
}