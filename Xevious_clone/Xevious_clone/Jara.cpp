#include "Jara.h"

Jara::Jara(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::JARA), x, y, type, 0, imageHandleP, scoreManagerP) {
	// �L������Ă��Ȃ����͐e�N���X�̃R���X�g���N�^�ɏ�����
	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[2]);		// X�AY�ړ����x��ݒ�
	m_AccelerationX = 0.0625;				// X�����x
	m_isRightAway = false;					// �E�����t���O
	m_isLeftAway = false;					// �������t���O
	m_AnimationSum = 0;
	m_Flame = 2;
	m_Score = 15;
}

void Jara::Calc(float player_x, float player_y, float groundMove, bool isHit) {

	DamageFunction(isHit);	// ��e��񏈗�

	switch(m_Status){

		case static_cast<int>(StatusId::ACTIVE):
		// ��������
		if (!m_isRightAway && !m_isLeftAway &&									// �����t���O���ǂ���������Ă��Ȃ�
			((int)player_x / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) < 6 &&		// �����@�Ƃ̍��W����6�L�����N�^����
			((int)player_x / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) >= -6) {	// �����@�Ƃ̍��W����-6�L�����N�^�ȉ�	����ł̍��E�ł̃Y�����Č�
			if (player_x >= m_X) {
				m_isRightAway = true;		// �E���痈�Ă鎞�͉E�ɓ�����t���O��ON
				if (m_Type == 1) {		// �^�C�v��1��������
					m_Shot();				// �e�𔭎�
				}
			}
			else {
				m_isLeftAway = true;	// �����痈�Ă鎞�͍��ɓ�����t���O��ON
				if (m_Type == 1) {	//�^�C�v1��������
					m_Shot();				// �e�𔭎�
				}
			}
		}

		// ��������
		if (m_isRightAway) {
			m_SpeedX -= m_AccelerationX;	// �E�ɉ������Ă���

			// �A�j���[�V��������
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum += 1;
				if (m_AnimationSum >= 5) {
					m_AnimationSum = 0;
				}
				m_Flame += 2;
			}

		}
		if (m_isLeftAway) {
			m_SpeedX += m_AccelerationX;	// ���ɉ������Ă���

			// �A�j���[�V��������
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum -= 1;
				if (m_AnimationSum < 0) {
					m_AnimationSum = 5;
				}
				m_Flame += 2;
			}

		}

		//���x�����W�ɔ��f
		m_X += m_SpeedX;
		m_Y += m_SpeedY;

		break;

		case static_cast<int>(StatusId::BROKEN):
			BrokenCalc(groundMove);
			break;

		default:
			break;
	}
}


// �`��
void Jara::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][72 + m_AnimationSum], true);	// ��ň�������J���[�ݒ�
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}
