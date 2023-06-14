#include "Derota.h"

Derota::Derota(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::DEROTA), x, y, 0, 1, imageHandleP, scoreManagerP) {
	//�����_���ȃt���[���擾
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, shotIntervalMax);
	m_seed = rand;
	m_rand = RandFlame;

	this->m_RandFlame = this->m_rand(this->m_seed);	// �������擾
	this->m_ShotInterval = this->m_RandFlame * 8;	// 8�{���Ĉ���
	this->m_Score = 100;

}

void Derota::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {

	DamageFunction(isHit);		// ��e��񏈗�

	switch (m_Status) {					// ��Ԃ��A�N�e�B�u

	case static_cast<int>(StatusId::ACTIVE):				// �A�N�e�B�u�ȏꍇ
		if (this->m_Y <= TEMP_SCREEN_HEIGHT - TEMP_SCREEN_HEIGHT / 4 &&
			this->m_ShotInterval <= 0) {					// �Ƃ肠�������ɉ��3/4�ȏゾ������@���A�V���b�g�C���^�[�o����0��������

			this->m_Shot();								// �e�𔭎�
			this->m_RandFlame = this->m_rand(this->m_seed);	// �������擾
			this->m_ShotInterval = m_RandFlame * 8;			// �V���b�g�C���^�[�o�������Z�b�g

		}

		this->m_ShotInterval--;								// �V���b�g�C���^�[�o�����f�N�������g

		m_Y += groundMove;
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// �j��v�Z
		break;

	default:
		break;
	}
}

// �`��
void Derota::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][31], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();              // �j��`��
		break;

	default:
		break;
	}
}