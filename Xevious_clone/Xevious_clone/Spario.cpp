#include "Spario.h"

Spario::Spario(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP)
	: Enemy(static_cast<int>(EnemyId::SPARIO), x, y, type, 0, imageHandleP, NULL) {
	// �L������Ă��Ȃ����͐e�N���X�̃R���X�g���N�^�ɏ�����
	this->SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[1]);		// X�AY�ړ����x��ݒ�
	this->m_IsMove = false;
}

void Spario::Calc() {
	if (this->m_Status == static_cast<int>(StatusId::ACTIVE)) {	// ���쒆

		if (this->m_IsMove) {	// �ŏ��̃t���[���ł͓����Ȃ�
			//���x�����W�ɔ��f
			this->m_X += this->m_SpeedX;
			this->m_Y += this->m_SpeedY;
		}
		else {
			this->m_IsMove = true;
		}

	}
}

// �`��
void Spario::Draw(int colorID) {
	if (this->m_Status == static_cast<int>(StatusId::ACTIVE)) {	// ���쒆

		DrawGraph((int)this->m_Y, (int)this->m_X, this->m_imageHandleP->spario[14 + colorID * 3], TRUE);
	}
}