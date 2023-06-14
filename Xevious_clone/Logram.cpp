#include "Logram.h"

Logram::Logram(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, Enemy* pParentObject)
	: Enemy(static_cast<int>(EnemyId::LOGRAM), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 30;

	//�����_���ȃt���[���擾
	std::random_device rd;
	std::default_random_engine rand(rd());

	int max;
	shotIntervalMax >= MIN ? max = shotIntervalMax : max = MIN;

	std::uniform_int_distribution<int> RandFlame(MIN, max);
	m_seed = rand;
	m_rand = RandFlame;
	
	m_RandFlame = m_rand(m_seed);		// �������擾
	m_ShotInterval = m_RandFlame * 8;	// 8�{���Ĉ���
	
	m_Animation = 0;					// �A�j���[�V����ID
	m_isAnimationIncrement = false;		// �V���b�g�A�j���[�V�����C���N�������g�t���O

	m_pParentObject = pParentObject;	// �e�I�u�W�F�N�g�|�C���^
}

void Logram::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// ��e��񏈗�
	
	// �A���j�󏈗�
	if (m_pParentObject != NULL && m_pParentObject->GetStatus() == static_cast<int>(StatusId::BROKEN)) {	// �e�I�u�W�F�N�g��BROKEN��������
		m_Status = static_cast<int>(StatusId::BROKEN);														// ���g�̏�Ԃ�BROKEN��
	}

	switch (m_Status)			// ��Ԃŕ���
	{
	case static_cast<int>(StatusId::ACTIVE):				// �A�N�e�B�u�ȏꍇ
		if (m_Y < NOT_SHOT_LINE &&
			m_ShotInterval <= 0) {					// �ˌ��s���C������A���V���b�g�C���^�[�o����0��������

			m_RandFlame = m_rand(m_seed);			// �������擾
			m_ShotInterval = m_RandFlame * 8;		// �V���b�g�C���^�[�o�������Z�b�g
			m_isAnimationIncrement = true;			// �V���b�g�A�j���[�V�����C���N�������g�t���O��true��
		}

		if (m_ShotInterval % 4 == 0) {				// �V���b�g�C���^�[�o����4�̔{���̎�
			if (m_isAnimationIncrement) {			// �V���b�g�A�j���[�V�����C���N�������g�t���O��true�Ȃ�
				m_Animation++;						// �V���b�g�A�j���[�V����ID���C���N�������g
			}
			else if (m_Animation != 0) {			// �V���b�g�A�j���[�V�����C���N�������g�t���O��false�ŁA���A�j���[�V������0�ԂłȂ���
				m_Animation--;						// �V���b�g�A�j���[�V����ID���f�N�������g
			}

			if (m_Animation == 3) {					// �V���b�g�A�j���[�V������3�Ԃ̎�
				m_Shot();							// �e�𔭎�
				m_isAnimationIncrement = false;		// �V���b�g�A�j���[�V�����C���N�������g�t���O��false��
			}
		}

		m_ShotInterval--;							// �V���b�g�C���^�[�o�����f�N�������g

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
void Logram::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][36 + m_Animation], TRUE);	// �`��
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// �j��`��
		break;

	default:
		break;
	}
}
