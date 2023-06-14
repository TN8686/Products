#include "BackGround.h"

BackGround::BackGround() {
	m_speed = M_SPEED_INIT;													// �w�i�X�N���[�����x
	m_startImage = LoadGraph("img/map_ready.png");							// �J�n���̐X�̉摜
	m_mapImage = LoadGraph("img/map.png");									// �S�̃}�b�v�摜
	GetGraphSize(m_startImage, &m_startImageHeight, &m_startImageWidth);	// �摜�T�C�Y
	GetGraphSize(m_mapImage, &m_mapImageHeight, &m_mapImageWidth);			// �摜�T�C�Y
	Init();																	// �w�i�\���̔z�񏉊���

}


/******************************************************************
�֐����FInit

�@�\�F�w�i�\���̔z��̏������������܂�

�����F�Ȃ�

�ԋp�l�F�Ȃ�
*********************************************************************/
void BackGround::Init() {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {											// �w�i�\���̔z�񐔕�
		m_backGround[i].image = m_startImage;												// ���蓖�Ă�摜���J�n���̐X��
		m_backGround[i].x = static_cast<float>(TEMP_OFFSCREEN_WIDTH);						// X���W
		m_backGround[i].y = static_cast<float>(TEMP_SCREEN_HEIGHT - (i + 1) * M_HEIGHT);	// Y���W�@������z�u���Ă���
		m_backGround[i].imageX = 0;															// �摜��X���W
		m_backGround[i].imageY = m_startImageHeight - (i + 1) * M_HEIGHT;					// �摜��Y���W
	}
}

// �w�i�̃X�N���[�����x��Ԃ��܂�
float BackGround::GetSpeed() {
	return m_speed;
}

/******************************************************************
�֐����FCalc

�@�\�F�w�i�S�̂̌v�Z���s���܂�

�����F
	int�^	area	(i)	�G���AID
	int�^	line	(i)	�G���A���C��
	bool�^	isStart	(i)	�J�n�����ۂ�

�ԋp�l�F�Ȃ�
*********************************************************************/
void BackGround::Calc(int area, int line, bool isStart) {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {											// �w�i�\���̔z�񐔕�
		m_backGround[i].y += m_speed;														// �X�N���[�����x�����Z

		if (m_backGround[i].y >= TEMP_SCREEN_HEIGHT) {										// ��ʊO�ɏo����
			m_backGround[i].y -= TEMP_SCREEN_HEIGHT;										// Y���W�����ʕ����

			if (isStart) {																	// �J�n����������
				m_backGround[i].imageY = m_startImageHeight - (line + 1) * CHARACTER_SIZE;	// �摜�̎Q��Y���W�����݂̃��C���̈ʒu�ɉ��������ɍX�V
			}
			else {																			// �J�n���łȂ�������
				if (m_backGround[i].image != m_mapImage) {									// �Q�Ɖ摜�n���h�����S�̃}�b�v�摜�łȂ�������
					m_backGround[i].image = m_mapImage;										// �Q�Ɖ摜�n���h����S�̃}�b�v�摜��
				}
				m_backGround[i].imageY = m_mapImageHeight - (line + 1) * CHARACTER_SIZE;	// �摜�̎Q��Y���W�����݂̃��C���̈ʒu�ɉ��������ɍX�V

				if (m_backGround[i].imageX != M_IMAGE_X[area]) {							// ���݂̎Q��X���W�����݂̃G���A�̎Q��X���W�ƈقȂ��Ă�����
					m_backGround[i].imageX = M_IMAGE_X[area];								// �G���A�f�[�^�ɉ�����X���W����
				}
			}
		}
	}

}

void BackGround::Draw() {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {
		DrawRectGraph(static_cast<int>(m_backGround[i].y), static_cast<int>(m_backGround[i].x),	// X���W�AY���W
			m_backGround[i].imageY, m_backGround[i].imageX, M_HEIGHT, M_WIDTH,					// �Q�Ɖ摜��X���W�AY���W�A�`�悷�镝�A�`�悷�鍂��
			m_backGround[i].image, false, false);												// �Q�Ɖ摜�n���h���A
	}
}