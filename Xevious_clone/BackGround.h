#pragma once

#include "Xevious_clone.h"
#include "AreaManager.h"

typedef struct {
	int image;	// �Q�Ƃ���摜�n���h��
	float x;	// X���W
	float y;	// Y���W
	int imageX;	// �Q�Ƃ���摜��X���W
	int imageY; // �Q�Ƃ���摜��Y���W
}BACK_GROUND;

class BackGround
{
public:
	// �R���X�g���N�^�錾
	BackGround();

	// �v�Z
	void Init();									// �w�i�\���̏�񏉊���

	void Calc(int area, int line, bool isStart);	// �S�̌v�Z

	// �`��
	void Draw();

	// �Q�b�^�[
	float GetSpeed();								// �X�N���[�����x

private:
	static constexpr float M_SPEED_INIT = 0.5f;		// �����X�N���[�����x
	static const int M_BACK_GROUND_NUM = 40;		// �w�i�\���̔z��
	static const int M_HEIGHT = 8;					// �w�i�������̍���
	static const int M_WIDTH = TEMP_SCREEN_WIDTH;	// �w�i�������̕�
	static constexpr int M_IMAGE_X[AreaManager::AREA_NUM] = {
		288, 0, 672, 112, 512, 208, 800, 32, 336, 736, 112, 32, 448, 672, 208, 0 };		// �G���A�ɉ������Q�Ƃ���摜��Y���W

	float m_speed;									// �w�i�X�N���[�����x
	int m_startImage;								// �J�n���̐X�摜
	int m_mapImage;									// �S�̃}�b�v�摜
	int m_startImageWidth;							// �J�n���̐X�̉摜�̕��i90�x��]�̊֌W�Ŏ��ۂ͍����j
	int m_startImageHeight;							// �J�n���̐X�̉摜�̍����i���ۂ͕��j
	int m_mapImageWidth;							// �S�̃}�b�v�摜�̕��i���ۂ͍����j
	int m_mapImageHeight;							// �S�̃}�b�v�摜�̍����i���ۂ͕��j
	BACK_GROUND m_backGround[M_BACK_GROUND_NUM];	// �w�i�\���̔z��
};

