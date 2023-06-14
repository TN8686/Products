#pragma once

#include <string>
#include "Xevious_clone.h"
#include "ScoreManager.h"
#include "Player.h"

enum class ColorName : int {
	RED,
	YELLOW,
	ORANGE,
	STOCK
};

class UI
{
public:
	UI(const int uiScreen, ScoreManager* scoreManagerP, Player* playerP, const int* padBuf);
	void Init();
	int NameEntryInit();
	int Calc(int mode);
	void Draw(int mode, int frameCount, bool isGameOver);
	int SetRankingText();
	void SetIsDrawHiddenMessage(bool isDrawHiddenMessage);

private:
	static constexpr int UI_TEXT_BUF = 32;

	static constexpr int M_FONT_NUM = 256;					// �t�H���g�摜�z��
	static constexpr int M_FONT_IMAGE_DIVISION_NUM = 16;	// �t�H���g�摜������X,Y
	static constexpr int M_COLOR_NUM = 4;					// �J���[�z��
	static constexpr int M_CH_SELECT_COUNT_MAX = 8;			// �����I���J�E���g�ő�l

	// �e�e�L�X�g�z��
	static constexpr int M_SCORE_SYSTEM_TEXT_NUM = 4;
	static constexpr int M_READY_TEXT_NUM = 3;
	static constexpr int M_COPYRIGHT_NUM = 2;
	static constexpr int M_PROMPT_START_TEXT_NUM = 2;
	static constexpr int M_CONG_TEXT_NUM = 2;
	static constexpr int M_BEST_FIVE_TEXT_NUM = 6;
	static constexpr int M_HIDDEN_MESSAGE_TEXT_NUM = 2;

	static constexpr int M_LOGO_X = CHARACTER_SIZE * 4 - 3;	// ���SX���W
	static constexpr int M_LOGO_Y = CHARACTER_SIZE * 14;	// ���SY���W
	static constexpr int M_READY_FLAME = 161;				// READY��\������t���[����
	static constexpr int M_SCORE_Y = 5;						// �X�R�A�A�n�C�X�R�A�\��Y���W
	static constexpr int M_SCORE_X = 6;						// �X�R�A�\��X���W
	static constexpr int M_HIGH_SCORE_X = 16;				// �n�C�X�R�A�\��X���W
	static constexpr int M_RANKING_SCORE_X = 13;			// �����L���O�\���X�R�AX���W
	static constexpr int M_RANKING_NAME_X = 16;				// �����L���O�\�����OX���W

	// �e�L�X�g�ƃL�����N�^�P�ʂ̍��W���Z�b�g�ɂ����e�L�X�g�\����
	typedef struct {
		char text[UI_TEXT_BUF];
		int x;
		int y;
	}TEXT_STRUCT;

	// �X�R�A�n�e�L�X�g
	const TEXT_STRUCT m_scoreSystemText[M_SCORE_SYSTEM_TEXT_NUM] = {
		"1UP", 3, 4,
		"HIGH SCORE", 9, 4,
		"0", M_SCORE_X, 5,			// �X�R�A������0
		"0", M_HIGH_SCORE_X, 5		// �n�C�X�R�A������0
	};

	const TEXT_STRUCT m_cloneText = {
		"CLONE" , 17, 19
	};

	// �f����
	const TEXT_STRUCT m_demoText = {
		"DEMO Ver.",16, 20
	};

	// �o�[�W�����\�L
	const TEXT_STRUCT m_verText = {
		"Ver.1.1",16, 20
	};

	// �����\�L�@����ɏ����Ă����񂩂ȁc�H
	const TEXT_STRUCT m_copyright[M_COPYRIGHT_NUM] = {
		"{|} 1982 NAMCO LTD.", 5, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 5,
		"�������", 11, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 3				// NAMCO���S�@"�������"��ASKII�R�[�h�ɑΉ������Ă���
	};

	// �J�n�𑣂��e�L�X�g
	const TEXT_STRUCT m_promptStartText[M_PROMPT_START_TEXT_NUM] = {
		"PUSH START BUTTON", 6, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 13,
		"ONE PLAYER ONLY", 7, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 11
	};

	// �N���W�b�g�@�ЂƂ܂�01�Œ�ŕ\��
	const TEXT_STRUCT m_credit = {
		"CREDIT 01", TEMP_SCREEN_WIDTH / CHARACTER_SIZE - 12, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 1
	};

	// READY�e�L�X�g
	const TEXT_STRUCT m_readyText[M_READY_TEXT_NUM] = {
		"PLAYER ONE", 9, 27,
		"READY!", 11, 29,
		"SOLVALOU LEFT", 8, 31
	};

	// �X�g�b�N�\���p
	const TEXT_STRUCT m_stock = {
		"^", 0, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 1
	};

	// �B�����b�Z�[�W
	const TEXT_STRUCT m_hiddenMessageText[M_HIDDEN_MESSAGE_TEXT_NUM] = {
		"THANK YOU FOR PLAYING!", 1, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 3,
		"program by N.Takafumi", 6, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 2
	};


	// �R���O���`�����[�V����
	const TEXT_STRUCT m_congText[M_CONG_TEXT_NUM] = {
		"CONGRATULATIONS!", 6, 13,
		"ENTER YOUR INITIAL", 5, 16
	};

	// �x�X�g�t�@�C�u�\��
	const TEXT_STRUCT m_bestFiveText[M_BEST_FIVE_TEXT_NUM] = {
		"1st        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12,
		"2nd        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 10,
		"3rd        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 8,
		"4th        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 6,
		"5th        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 4,
		"Best Five WARRIORS", 5, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 15,
	};

	// �Q�[���I�[�o�[
	const TEXT_STRUCT m_gameOverText = {
		"GAME OVER" , 10, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12
	};

	const int m_uiScreen;				// UI�X�N���[���n���h��
	const int* m_pPadBuf;				// �p�b�h�o�t�|�C���^

	ScoreManager* m_scoreManagerP;		// �X�R�A�}�l�[�W���|�C���^
	Player* m_playerP;					// �v���C���[�|�C���^

	TEXT_STRUCT m_readyStock;			// READY���̃X�g�b�N��
	TEXT_STRUCT m_scoreText;			// �X�R�A
	TEXT_STRUCT m_highScoreText;		// ��ʏ㕔�̃n�C�X�R�A
	TEXT_STRUCT m_rankingNameText[ScoreManager::RANKING_MAX];	// �����L���O
	TEXT_STRUCT m_rankingScoreText[ScoreManager::RANKING_MAX];	// �����L���O

	TEXT_STRUCT m_tempName;				// �����L���O�����p�@�ꎞ���O�e�L�X�g
	TEXT_STRUCT m_inputCh;				// �����L���O�����p�@���݂̕����\���p
	char m_Ch;							// �����L���O�����p�@���݂̕����̓����I�Ȓl
	char* m_pTempNameText;				// �����L���O�����p�@���ݓ��͂��Ă���m_tempName.text�̃|�C���^
	bool m_isInputFirst;				// �����L���O�����p�@�������͂̃{�^��������̓t���O

	int m_rank;							// �����L���O�����p�@�����������N
	int m_chSelectCount;				// �����L���O�����p�@�������͂Ŏ��̕����ɍs���J�E���g

	int m_readyFlameCount;				// READY�\���t���[���J�E���g
	int m_preScore;						// �O�t���[���̃X�R�A
	bool m_isDrawHiddenMessage;			// �B�����b�Z�[�W�`��t���O

	int m_titleLogo;					// ���S�摜�n���h��
	int m_fontImage[M_FONT_NUM];		// �t�H���g�摜�n���h���z��
	unsigned int m_color[M_COLOR_NUM];	// �J���[�z��

	int DrawGraphicString(int x, int y, const char* strings, int* phFont, int transflag = TRUE);
	int DrawTextStruct(TEXT_STRUCT textStruct);
	int Filter(TEXT_STRUCT textStruct, int color);
	int IntToString(int num, unsigned int textBuf, char* text);
	int SetReadyStock();
	int SetScoreText();
};