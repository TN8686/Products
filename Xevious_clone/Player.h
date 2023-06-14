#pragma once

#include "Xevious_clone.h"
#include "SoundManager.h"

// �Ƃ肠�����v���C���[�N���X������Ă݂�
class Player {
public:
	Player(SoundManager* pSoundManager);			// �R���X�g���N�^�錾

	// ������
	void Init();
	void Restart();

	// Get�֐�
	float GetX();		// X���W
	float GetY();		// Y���W
	int GetStock();		// �c�@
	bool GetOnActive();

	// Set�֐�
	void SetIsHit(bool isHit);

	void Extend();		// �G�N�X�e���h

	void Calc(char keyBuf[], int padBuf, int frameCount);	// �v�Z
	void Draw();		// �`��

	// �萔
	static constexpr int PLAYER_IMG_DIVSION_NUM = 6;	// �摜������
	static constexpr int PLAYER_SIZE = 16;		// ���@�̏c���T�C�Y
private:
	static constexpr int BROKEN_IMG_DIVSION_NUM = 7;	// �摜������
	static constexpr int BROKEN_IMG_SIZE = 32;		// ���@�̏c���T�C�Y

	static constexpr float CURSOR_RANGE = 96;
	static constexpr float Y_MAX = CHARACTER_SIZE + CHARACTER_SIZE + CURSOR_RANGE + TEMP_OFFSCREEN_HEIGHT;

	static constexpr int STOCK_INIT = 2;

	int m_width;		// ��
	int m_height;		// ����
	float m_x;			// X���W
	float m_y;			// Y���W
	float m_speed;		// Y�����A�΂ߕ����ւ̈ړ����x
	float m_speed_x;	// X�����݂̂ւ̈ړ����x
	int m_image[PLAYER_IMG_DIVSION_NUM];		// �摜�n���h��
	int m_brokenImage[BROKEN_IMG_DIVSION_NUM];	// �j��摜�n���h��
	int m_stock;		// �c�@
	bool m_onActive;	// �A�N�e�B�u���
	bool m_isHit;		// �q�b�g���
	bool m_isInvincible; // ��Ƀf�o�b�O�p���G�t���O
	bool m_isDraw;		// ��Ƀf�o�b�O�p�`��t���O
	bool m_isFlip;		// �j��`�掞�̔��]�t���O
	int m_brokenAnimID;	// �j��A�j���[�V����ID
	int m_frameCount;	// �j��A�j���[�V�����p�t���[���J�E���^

	SoundManager* m_pSoundManager;	// �T�E���h�}�l�[�W���|�C���^

	void BrokenCalc(int frameCount);	// �j��v�Z
	void BrokenDraw();	// �j��`��
};