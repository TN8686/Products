#pragma once

#include "Player.h"
#include "Debug.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "SoundManager.h"
#include "Cursor.h"
#include "HitChecker.h"
#include "ScoreManager.h"
#include "BackGround.h"
#include "AreaManager.h"
#include "UI.h"

class GameManager
{
public:
	GameManager(bool isDebug);
	~GameManager();

	// ���C�����[�v�@���s����ƒE�o����܂œ����Ń��[�v�������܂�
	int MainLoop();

	// �������֐�
	int TitleInit();
	int MainGameInit();
	int NameEntryInit();

	// �v�Z�֐�
	int TitleCalc();
	int MainGameCalc();
	int NameEntryCalc();

	// �`��֐�
	int TitleDraw();
	int MainGameDraw();
	int NameEntryDraw();

private:
	static constexpr int TEMP_SCREEN_NUM = 3;				// ����ʂ̐�
	static constexpr int MICROSECONDS_PER_FRAME = 16500;	// 1�t���[��������̃}�C�N���b
	static constexpr int MISS_FRAME = 88;					// �~�X�E�F�C�g�t���[��
	static constexpr int RESTART_FRAME = 74;				// ���X�^�[�g�E�F�C�g�t���[��
	static constexpr int GAME_OVER_FRAME = 128;				// �uGAME OVER�v�\���̃t���[��

	bool m_isDebug;						// �f�o�b�O���[�h�t���O
	bool m_isResetFirst;				// ���Z�b�g�L�[����t���O

	int tempScreen[TEMP_SCREEN_NUM];	// ����ʂ̃n���h���p
	int uiScreen;						// UI��ʃn���h���p
	long long int firstTime;			// ���Ԍv���p
	unsigned int frameCount;			// �S�̂̃t���[���J�E���g
	int mode;							// ���݂̃��[�h
	bool isStartBgmAfter;				// �J�n��BGM�I����t���O

	int missFrameCount;					// �~�X���Ɏg�p����t���[���J�E���g
	bool isRestart;						// ���X�^�[�g���t���O
	int restartFrameCount;				// ���X�^�[�g���Ɏg�p����t���[���J�E���g
	bool isGameOver;					// �Q�[���I�[�o�[�\���t���O
	int gameOverCount;					// �Q�[���I�[�o�[�\���t���[���J�E���g


	char keyBuf[256];	// �L�[���͎󂯎��p
	int padBuf;			// �p�b�h���͎󂯎��p

	Debug* debuger;					// �f�o�b�K�[
	SoundManager* soundManager;		// �T�E���h�}�l�[�W��
	AreaManager* areaManager;		// �G���A�}�l�[�W��
	Player* player;					// �v���C���[
	BackGround* backGround;			// �o�b�N�O���E���h
	ScoreManager* scoreManager;		// �X�R�A�}�l�[�W��
	BulletManager* bulletManager;	// �o���b�g�}�l�[�W��
	EnemyManager* enemyManager;		// �G�l�~�[�}�l�[�W��
	HitChecker* hitChecker;			// �q�b�g�`�F�b�J�[
	UI* ui;
};