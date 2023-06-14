#include "Torkan.h"

Torkan::Torkan(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::TORKAN), x, y, type, 0, imageHandleP, scoreManagerP) {
	// �L������Ă��Ȃ����͐e�N���X�̃R���X�g���N�^�ɏ�����
	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[1]);		// X�AY�ړ����x��ݒ�
	m_Flame = 4;					//�A�j���[�V�����p�t���[���������l
	m_IsStop      = false;			//��~�t���O�����l
	m_IsReverse   = false;			//�A�҃t���O�����l
	m_IsShotAfter = false;			//�e���ˌ�t���O������
	m_AnimationSum = 0;				//�A�j���[�V�������Z�����l
	m_Score = 5;

	//�����_���ȃt���[���擾
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, MAX);
	m_RandFlame = RandFlame(rand);
}

void Torkan::Calc(float player_x, float player_y, float groundMove, bool isHit) {

	DamageFunction(isHit);	// ��e��񏈗�

	switch (m_Status) {

	case static_cast<int>(StatusId::ACTIVE) :
		m_RandFlame--;

		if (m_RandFlame <= 0 && !m_IsReverse) {		// �����_���ȃt���[����0�ŁA���A�҃t���O��false�Ȃ�
			m_IsStop = true;								// ��~�t���O��true

			//�e���ˏ���
			if (!m_IsShotAfter) {							// �e���ˌ�t���O��false�Ȃ�
				m_Shot();									// �e�𔭎�
				m_IsShotAfter = true;						// �e���ˌ�t���O��true��
			}

			// �A�j���[�V��������
			m_Flame--;
			if (m_Flame <= 0) {

				//�A�j���[�V�������Z����
				if (m_AnimationSum <= 5) {
					m_AnimationSum++;
				}
				m_Flame = 4;

				//�A�j���[�V�������I�������
				if (m_AnimationSum == 5) {
					SetSpeedFromAngle(Angle(player_x, player_y, m_X, m_Y), M_SPEED_TABLE[2]);		// X�AY�ړ����x���Đݒ�
					m_IsReverse = true;	// �A�҃t���O��true
					m_IsStop = false;		// ��~�t���O��false
				}
			}
		}

		//��~�t���O��true��������
		if (m_IsStop) {
			m_Y += groundMove;	// �~�܂�(�w�i�Ɠ����ňړ�)
		}
		else {
			//���x�����W�ɔ��f
			m_X += m_SpeedX;
			m_Y += m_SpeedY;
		}

		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}

void Torkan::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][16 + m_AnimationSum], true);	// ��ň�������J���[�ݒ�
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}