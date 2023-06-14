#include "UI.h"

UI::UI(const int uiScreen, ScoreManager* scoreManagerP, Player* playerP, const int* padBuf) : m_uiScreen(uiScreen), m_pPadBuf(padBuf) {

	m_color[static_cast<int>(ColorName::RED)] = GetColor(0xFF, 0x00, 0x00);
	m_color[static_cast<int>(ColorName::YELLOW)] = GetColor(0xFF, 0xFF, 0x00);
	m_color[static_cast<int>(ColorName::ORANGE)] = GetColor(0xFF, 0xAE, 0x00);
	m_color[static_cast<int>(ColorName::STOCK)] = GetColor(0xE0, 0xBC, 0x62);

	m_titleLogo = LoadGraph("img/title.png");
	LoadDivGraph("img/Xevious_font.png", M_FONT_NUM, M_FONT_IMAGE_DIVISION_NUM, M_FONT_IMAGE_DIVISION_NUM, CHARACTER_SIZE, CHARACTER_SIZE, m_fontImage);

	m_scoreManagerP = scoreManagerP;	// �X�R�A�}�l�[�W���|�C���^
	m_playerP = playerP;				// �v���C���[�|�C���^

	memset(&m_tempName, 0x00, sizeof(m_tempName));			// �����L���O�l�[���e�L�X�g������
	memset(&m_inputCh, 0x00, sizeof(m_inputCh));			// �����L���O�X�R�A�e�L�X�g������
	m_inputCh.text[0] = m_Ch = 32;							// �������͕���
	m_tempName.x = M_RANKING_NAME_X;						// �ꎞ���O�e�L�X�gX���W
	m_inputCh.x = M_RANKING_NAME_X;							// ���͕����\���pX���W
	
	m_rank = -1;						// �����L���O�����p�@�����������N
	m_chSelectCount = 0;				// �����L���O�����p�@�����I���J�E���g
	m_isInputFirst = true;				// �������̓{�^������t���O
	m_isDrawHiddenMessage = false;		// �B�����b�Z�[�W�o�̓t���O

	m_preScore = 0;						// �O��X�R�A

	memset(m_rankingNameText, 0x00, sizeof(m_rankingNameText));			// �����L���O�l�[���e�L�X�g������
	memset(m_rankingScoreText, 0x00, sizeof(m_rankingScoreText));		// �����L���O�X�R�A�e�L�X�g������

	for (int i = 0; i < ScoreManager::RANKING_MAX; i++) {								// �����L���O�֘A���W�ݒ�
		m_rankingNameText[i].x = M_RANKING_NAME_X;
		m_rankingNameText[i].y = TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12 + (i * 2);
		m_rankingScoreText[i].y = TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12 + (i * 2);
	}


	// �e�L�X�gY���W�ݒ�
	m_readyStock.y = m_readyText[2].y;	// READY�X�g�b�N��
	m_scoreText.y = M_SCORE_Y;			// �X�R�A
	m_highScoreText.y = M_SCORE_Y;		// �n�C�X�R�A

	Init();								// ����������
}

// �������֐�
void UI::Init() {
	SetScoreText();						// �X�R�A�e�L�X�g
	SetReadyStock();					// READY�X�g�b�N
	m_readyFlameCount = M_READY_FLAME;	// READY�\���t���[���J�E���g
	m_isDrawHiddenMessage = false;		// �B�����b�Z�[�W�o�̓t���O
}

// �l�[���G���g���[������
int UI::NameEntryInit() {
	m_rank = m_scoreManagerP->RankingUpdate();					// �����L���O�X�V�A�����������N���擾

	if (m_rank != -1) {											// �����N�C�����Ă�����
		SetRankingText();										// �����L���O�e�L�X�g���\��
		memset(m_tempName.text, 0x00, sizeof(m_tempName.text));	// ������
		memset(m_inputCh.text, 0x00, sizeof(m_inputCh.text));	// ������
		m_tempName.y = m_rankingNameText[m_rank].y;				// �������͊֘A�e�L�X�g��Y���W�ݒ�
		m_inputCh.y = m_rankingNameText[m_rank].y;
		m_pTempNameText = m_tempName.text;						// ���ݓ��͐�̃|�C���^��ݒ�
		m_isInputFirst = false;									// �{�^�����̓t���O������
		m_inputCh.text[0] = m_Ch = 32;							// �������͕��� 
		m_inputCh.x = M_RANKING_NAME_X;							// �������͕����\��X���W
		m_chSelectCount = 0;									// �������͑I���J�E���g
	}

	return m_rank;
}

// �������͂��I����ăQ�[���I�[�o�[�ɂȂ������̂�1��Ԃ��A����ȊO�ł�0��Ԃ��܂�
int UI::Calc(int mode) {
	int tmpScore;

	switch (mode)
	{
	case static_cast<int>(ModeId::TITLE):
		break;

	case static_cast<int>(ModeId::MAIN_GAME):

		if (m_readyFlameCount != 0) {				// READY�\���p�t���[���J�E���g�f�N�������g
			m_readyFlameCount--;
		}

		tmpScore = m_scoreManagerP->GetScore();		// �X�R�A�擾
		if (m_preScore != tmpScore) {				// �ω�����������
			m_preScore = tmpScore;					// �X�R�A�ێ�
			SetScoreText();							// ������
		}
		break;

	case static_cast<int>(ModeId::NAME_ENTRY):

		if (*m_pPadBuf & PAD_INPUT_LEFT) {
			m_chSelectCount--;
			if (m_chSelectCount < 0) {
				m_chSelectCount = M_CH_SELECT_COUNT_MAX;
				m_Ch--;
			}
		}
		if (*m_pPadBuf & PAD_INPUT_RIGHT) {
			m_chSelectCount++;
			if (m_chSelectCount >= M_CH_SELECT_COUNT_MAX) {
				m_chSelectCount = 0;
				m_Ch++;
			}
		}

		
		if (m_Ch == 33) {	// �X�y�[�X�̌ゾ������
			m_Ch = 65;		// A��
		}
		if (m_Ch == 64) {	// A�̑O��������
			m_Ch = 32;		// �X�y�[�X��
		}

		if (m_Ch == 31) {	// �X�y�[�X�̑O��������
			m_Ch = 90;		// Z��
		}
		if (m_Ch == 91) {	// Z�̌ゾ������
			m_Ch = 32;		// �X�y�[�X��
		}

		if (*m_pPadBuf & PAD_INPUT_2 + PAD_INPUT_4) {
			if (m_Ch == 32) {
				m_inputCh.text[0] = 46;
			}
			else {
				m_inputCh.text[0] = m_Ch + 32;
			}
		}
		else {
			m_inputCh.text[0] = m_Ch;
		}

		if (*m_pPadBuf & PAD_INPUT_1 + PAD_INPUT_3) {
			if (m_isInputFirst) {
				m_isInputFirst = false;
				*m_pTempNameText = m_inputCh.text[0];		// ��������
				m_pTempNameText++;							// �|�C���^���C���N�������g��1�Y����
				m_inputCh.x++;								// ���͕�����X���W���C���N�������g

				if (strlen(m_tempName.text) >= 10) {
					m_scoreManagerP->SetUserName(m_rank, m_tempName.text);
					return 1;
				}
			}
		}
		else {
			m_isInputFirst = true;
		}


	default:
		break;
	}

	return 0;
}

void UI::Draw(int mode, int frameCount, bool isGameOver) {
	int i = 0;	//�J�E���^

	if (mode == static_cast<int>(ModeId::TITLE) || frameCount % 32 < 16) {	// �^�C�g���ȊO�ł͓_�ŏ���
		DrawTextStruct(m_scoreSystemText[0]);	// 1UP
	}
	for (i = 1; i < M_SCORE_SYSTEM_TEXT_NUM; i++) {
		DrawTextStruct(m_scoreSystemText[i]);	// HIGH SCORE�@�X�R�A������0�@�n�C�X�R�A������0
	}
	DrawTextStruct(m_scoreText);				// �X�R�A
	DrawTextStruct(m_highScoreText);			// �n�C�X�R�A

	// �t�B���^�[	��ʏ�̃n�C�X�R�A�\��
	Filter(m_scoreSystemText[1], static_cast<int>(ColorName::ORANGE));


	switch (mode)
	{
	case static_cast<int>(ModeId::TITLE):
		DrawGraph(M_LOGO_X, M_LOGO_Y, m_titleLogo, TRUE);	// ���S

		DrawTextStruct(m_cloneText);						// CLONE
		DrawTextStruct(m_verText);							// �o�[�W�����\�L
		//DrawTextStruct(m_demoText);							// �f����

		for (i = 0; i < M_COPYRIGHT_NUM; i++) {
			DrawTextStruct(m_copyright[i]);					// �����\�L
		}
		// �t�B���^�[	namco
		Filter(m_copyright[1], static_cast<int>(ColorName::RED));

		for (i = 0; i < M_PROMPT_START_TEXT_NUM; i++) {
			DrawTextStruct(m_promptStartText[i]);			// �J�n�𑣂��e�L�X�g
		}
		DrawTextStruct(m_credit);							// �N���W�b�g


		break;

	case static_cast<int>(ModeId::MAIN_GAME):
		// ������o��
		if (m_readyFlameCount != 0) {
			for (i = 0; i < M_READY_TEXT_NUM; i++) {
				DrawTextStruct(m_readyText[i]);			// READY�e�L�X�g
			}
			DrawTextStruct(m_readyStock);				// READY�X�g�b�N���\��
		}


		// �X�g�b�N�\��
		for (i = 0; i < m_playerP->GetStock(); i++) {
			DrawGraphicString((m_stock.x + i) * CHARACTER_SIZE, m_stock.y * CHARACTER_SIZE, m_stock.text, m_fontImage);
		}

		// �X�g�b�N�\���t�B���^�[�@Filter�֐����ƈꕶ�����|����`�ɂȂ��Ă��܂��ď������d���̂ł܂Ƃ߂Ă�����
		GraphFilterRectBlt(m_uiScreen, m_uiScreen, CHARACTER_SIZE * m_stock.x, CHARACTER_SIZE * m_stock.y,
			TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT,
			CHARACTER_SIZE * m_stock.x, CHARACTER_SIZE * m_stock.y,
			DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER, 128, TRUE, m_color[static_cast<int>(ColorName::STOCK)], 255);

		// �B�����b�Z�[�W
		if (m_isDrawHiddenMessage) {
			for (int i = 0; i < M_HIDDEN_MESSAGE_TEXT_NUM; i++) {
				DrawTextStruct(m_hiddenMessageText[i]);
			}
		}

		break;

	case static_cast<int>(ModeId::NAME_ENTRY):
		if (isGameOver) {
			DrawTextStruct(m_gameOverText);
		}
		else {
			for (i = 0; i < M_CONG_TEXT_NUM; i++) {
				DrawTextStruct(m_congText[i]);											// �R���O���`�����[�V�����e�L�X�g
				Filter(m_congText[i], static_cast<int>(ColorName::RED));				// �t�B���^�[�@��
			}

			for (i = 0; i < ScoreManager::RANKING_MAX; i++) {
				DrawTextStruct(m_rankingScoreText[i]);									// �����L���O�X�R�A
				DrawTextStruct(m_rankingNameText[i]);									// �����L���O�l�[��

			}

			for (i = 0; i < M_BEST_FIVE_TEXT_NUM; i++) {
				DrawTextStruct(m_bestFiveText[i]);										// �x�X�g�t�@�C�u�e�L�X�g

				if (i == m_rank) {														// �����N�C�����������̂� 
					Filter(m_bestFiveText[i], static_cast<int>(ColorName::YELLOW));		// �t�B���^�[�@���F
				}
			}

			DrawTextStruct(m_tempName);
			DrawTextStruct(m_inputCh);

			Filter(m_tempName, static_cast<int>(ColorName::YELLOW));
			Filter(m_inputCh, static_cast<int>(ColorName::YELLOW));

		}
		break;

	default:
		break;
	}

}

/*
* ���O�FDrawGraphicString
*
* �@�\�F�摜�̃t�H���g�f�[�^�ŕ�������o�͂��܂�
*
* �����F
* (i) int x X���W
* (i) int y Y���W
* (i) const char* text ������A�h���X
* (i)int* pFontImage �t�H���g�摜�n���h���|�C���^
*
* �߂�l�F
* �����F0
* ���s�F-1
*/
int UI::DrawGraphicString(int x, int y, const char* text, int* pFontImage, int transflag)
{
	int cx = x;

	for (int i = 0; text[i] != NULL; i++)
	{
		if (DrawGraph(cx, y, pFontImage[static_cast<unsigned char>(text[i])], transflag) != 0) {
			return -1;
		}

		cx += CHARACTER_SIZE;
	}

	return 0;
}


/*
* ���O�FDrawGraphicTextStruct
*
* �@�\�FTEXT_STRUCT�^�̃f�[�^�����ɁA��������o�͂��܂�
*
* �����F(i) TEXT_STRUCT textStruct �e�L�X�g���
*
* �߂�l�F
* �����F0
* ���s�F-1
*/
int UI::DrawTextStruct(TEXT_STRUCT textStruct) {
	return DrawGraphicString(textStruct.x * CHARACTER_SIZE, textStruct.y * CHARACTER_SIZE, textStruct.text, m_fontImage);
}

/*
* ���O�FFilter
*
* �@�\�FTEXT_STRUCT�^�̃f�[�^�����ɁA�J���[�t�B���^�[���|���܂�
*
* �����F
* (i) TEXT_STRUCT textStruct �e�L�X�g���
* (i) int colorId �J���[ID
*
* �߂�l�F
* �����F0
* ���s�F-1
*
* ���ӁFcolorId�� static_cast<int>(ColorName::**) �Ŏw�肵�Ă�������
*/
int UI::Filter(TEXT_STRUCT textStruct, int colorId) {
	return 	GraphFilterRectBlt(m_uiScreen, m_uiScreen, CHARACTER_SIZE * textStruct.x, CHARACTER_SIZE * textStruct.y,
		CHARACTER_SIZE * (textStruct.x + strlen(textStruct.text)), CHARACTER_SIZE * (textStruct.y + 1),
		CHARACTER_SIZE * textStruct.x, CHARACTER_SIZE * textStruct.y,
		DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER, 128, TRUE, m_color[colorId], 255);
}

/*
* ���O�FIntToString
*
* �@�\�F�������̒l�𕶎���ɂ��đ�O�����̃A�h���X�ɑ�����܂�
*
* �����F
* (i) int number ������ɕϊ�����l
* (i) unsigned int textBuf �����̃o�b�t�@
* (o) char* text ����������̃A�h���X
*
* �߂�l�F
* �����F0
* ���s�F-1
*/
int UI::IntToString(int number, unsigned int textBuf, char* text) {
	std::string tmp = std::to_string(number);
	if (strcpy_s(text, textBuf, tmp.c_str()) == 0) {
		return 0;
	}
	return -1;
}


/*
* ���O�FSetReadyStock
*
* �@�\�Fm_readyStock.text��m_readyStock.x��ݒ肵�܂�
*
* �߂�l�F
* �����F0
* ���s�F-1
*/
int UI::SetReadyStock() {
	int ret;
	ret = IntToString(m_playerP->GetStock(), UI_TEXT_BUF, m_readyStock.text);
	m_readyStock.x = m_readyText[2].x - 1 - strlen(m_readyStock.text);

	return ret;
}

/*
* ���O�FSetScoreText
*
* �@�\�Fm_scoreText��m_highScoreText�̕������X���W��ݒ肵�܂�
*
* �߂�l�F
* �����F0
* ���s�F-1
*/
int UI::SetScoreText() {
	if (IntToString(m_scoreManagerP->GetScore(), UI_TEXT_BUF, m_scoreText.text) == -1) {		// ���݃X�R�A
		return -1;
	}
	m_scoreText.x = M_SCORE_X - strlen(m_scoreText.text);

	if (IntToString(m_scoreManagerP->GetTopScore(), UI_TEXT_BUF, m_highScoreText.text) == -1) {	// �n�C�X�R�A
		return -1;
	}
	m_highScoreText.x = M_HIGH_SCORE_X - strlen(m_highScoreText.text);

	return 0;
}

/*
* ���O�FSetRankingText
* 
* �@�\�F�����L���O�f�[�^�����ɁA�����L���O�e�L�X�g���\�����܂�
* 
* �����F�Ȃ�
* 
* �߂�l�F
* �����F0
* ���s�F-1
*/

int UI::SetRankingText() {
	
	ScoreManager::RANKING tempRanking = m_scoreManagerP->GetRanking();	// �����L���O�f�[�^�ǂݍ���

	for (int i = 0; i < ScoreManager::RANKING_MAX; i++) {
		strcpy_s(m_rankingNameText[i].text, ScoreManager::USER_NAME_BUF, tempRanking.userName[i]);	// ���O�f�[�^���R�s�[
		IntToString(tempRanking.highScore[i], ScoreManager::USER_NAME_BUF, m_rankingScoreText[i].text);	// �X�R�A���e�L�X�g�ɕϊ�
		m_rankingScoreText[i].x = M_RANKING_SCORE_X - strlen(m_rankingScoreText[i].text);				// X���W��ݒ�
	}

	return 0;
}

// �B�����b�Z�[�W�\���t���O
void UI::SetIsDrawHiddenMessage(bool isDrawHiddenMessage) {
	m_isDrawHiddenMessage = isDrawHiddenMessage;
}