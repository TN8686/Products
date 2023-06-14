#include "Blaster.h"

Blaster::Blaster(int* imageHndleP) {
	this->m_X = 0;
	this->m_Y = 0;

	this->m_Speed = 0;							// Y�ړ����x
	this->m_Acceleration = 0.125;				// Y�����x
	this->m_FrameCount = this->M_FRAME_MAX + 1;							// �o�߃t���[��
	this->m_Size = 0;								// �T�C�Y
	this->m_ColorId = 0;							// ���݂̃J���[�z���ID
	this->m_Image = imageHndleP;
}

// �v�Z
void Blaster::Calc() {
	//�t���[���J�E���g��0�ł��ő�l�ł��Ȃ���
	if (this->m_FrameCount != 0 && this->m_FrameCount != this->M_FRAME_MAX) {

		// �T�C�Y��2�����ŁA8�̔{���̎�
		if (this->m_Size < 2 && this->m_FrameCount % 8 == 0) {
			// �T�C�Y�ύX
			this->m_Size++;
		}

		// 4�̔{���̎��ƁA�J���[ID��6�̎���ID���C���N�������g�@����̊�ȕ����̍Č��̂��߂����Ȃ��Ă���
		if (this->m_FrameCount % 4 == 0 || this->m_ColorId == 6) {
			// �J���[�ύX
			this->m_ColorId++;
		}
	}

	// �J���[ID���ő�l�����̎�
	if (this->m_FrameCount < this->M_FRAME_MAX) {
		// �t���[���J�E���g���C���N�������g
		this->m_FrameCount++;

		// ���W�Ƒ��x���X�V
		this->m_Y += this->m_Speed;
		this->m_Speed -= this->m_Acceleration;
	}
	if(this->m_FrameCount == this->M_FRAME_MAX){	// �ő�t���[���������珉����
		this->m_X = 0;
		this->m_Y = 0;
		this->m_Speed = 0;
		this->m_Size = 0;
		this->m_ColorId = 0;
	}
}

void Blaster::Draw() {
	DrawGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X), this->m_Image[12 + this->m_Size + this->m_ColorArray[this->m_ColorId] * 3], TRUE);
}


void Blaster::Create(float playerX, float playerY) {
	this->m_X = playerX;
	this->m_Y = playerY;
	this->m_FrameCount = 0;
}