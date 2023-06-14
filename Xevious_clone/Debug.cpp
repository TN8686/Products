#include "Debug.h"

Debug::Debug(bool isDebugMode) {
	this->m_IsDebugMode = isDebugMode;	// �f�o�b�O���[�h�t���O
	this->m_IsStop = false;				// �v�Z������~�t���O
	this->m_IsStopKeyFirst = true;		// ��~�L�[������̓t���O
	this->m_IsAdvanceKeyFirst = true;	// �R�}����L�[������̓t���O
	this->m_IsFrameAdvance = false;		// �R�}����t���O
	SetDebugFont();
}

// �f�o�b�O�p�̃t�H���g�T�C�Y�ݒ�
void Debug::SetDebugFont() {
	if (this->m_IsDebugMode) {
		SetFontSize(14);
		SetFontThickness(0);
	}
}

// �l��\��
void Debug::DrawPlayerValue(Player* player, int score, AreaManager* areaManager, int level) {
	if (this->m_IsDebugMode) {
		char textbuf[256];
		sprintf(textbuf, "X:%.1f\nY:%.1f\nStock:%d\nScore:%d\nArea:%d\nLoadArea:%d\nAreaLine:%d\nGroundEnemyAreaLine:%d\nAreaY:%.1f\nLevel:%d", player->GetX(), player->GetY(), player->GetStock(),
			score, areaManager->GetAria(), areaManager->GetLoadAria(), areaManager->GetLine(), areaManager->GetGroundEnemyLine(), areaManager->GetY(), level);
		DrawString(WINDOW_WIDTH + 2, TEMP_SCREEN_WIDTH, textbuf, GetColor(255, 255, 255));
	}
}

/******************************************************************
�֐����FDrawDebugWindow

�@�\�F�f�o�b�O���[�h�p�̗̈�ɏ���\��

�����F
	int�^	temp_screen	(i) �ŐV�̉���ʂ̃n���h��
	Player�^	player	(i)	�v���C���[�I�u�W�F�N�g

�ԋp�l�F�Ȃ�

���ӎ����F�Ȃ�
*********************************************************************/
void Debug::DrawDebugWindow(int temp_screen, Player* playerP, int score, AreaManager* areaManagerP, int level){
	if (this->m_IsDebugMode) {
		// �f�o�b�O�p�`��̈�ɉ���ʂ��܂邲�ƕ`��
		DrawGraph(WINDOW_WIDTH, 0, temp_screen, TRUE);
		// �\����ʂ̕\���O�̈�̈ꕔ���������Ⴄ�̂ŖډB��
		DrawBox(WINDOW_WIDTH, TEMP_SCREEN_WIDTH, WINDOW_WIDTH + TEMP_SCREEN_WIDTH + TEMP_SCREEN_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
		// �f�o�b�O�p�Ƀv���C���[�f�[�^�̈ꕔ���o��
		DrawPlayerValue(playerP, score, areaManagerP, level);
	}
}

/******************************************************************
�֐����FCalcStop

�@�\�F
	�X�y�[�X�L�[�����͂����x�A�v�Z��~�t���O���X�C�b�`���A���쒆�ƒ�~����؂�ւ��܂�
	�v�Z��~���Ƀs���I�h�L�[�������ꂽ�ꍇ�A1�t���[���������쒆�ɂ��܂�

�����F	char�^	key	(i)	�L�[���͏��

�ԋp�l�F
	���쒆�Ftrue
	��~���Ffalse

���ӎ����F���̊֐����̂̓t���O����Ԃ��݂̂Ȃ̂ŁA�߂�l�g����if���Ŕ��肵�Ă�������
*********************************************************************/
bool Debug::CalcStop(char key[], int padBuf) {
	if (this->m_IsDebugMode) {
		// �p�b�h�����~�ł���o�[�W����
		if (!(padBuf & PAD_INPUT_8 || key[KEY_INPUT_SPACE]) && !this->m_IsStopKeyFirst) {			// �X�y�[�X�L�[�����͂���Ă��炸�A����~������̓t���O�������Ă��Ȃ�
			this->m_IsStopKeyFirst = true;								// ��~������̓t���O��true��
		}
		if ((padBuf & PAD_INPUT_8 || key[KEY_INPUT_SPACE]) && this->m_IsStopKeyFirst) {			// �X�y�[�X�L�[�����͂���Ă��āA����~������͂ł���
			this->m_IsStop = !this->m_IsStop;							// �v�Z��~�t���O�𔽓]
			this->m_IsStopKeyFirst = false;								// ��~������̓t���O��false��
		}

		/*// �p�b�h�����~�ł��Ȃ��o�[�W����
		if (!(key[KEY_INPUT_SPACE]) && !this->m_IsStopKeyFirst) {			// �X�y�[�X�L�[�����͂���Ă��炸�A����~������̓t���O�������Ă��Ȃ�
			this->m_IsStopKeyFirst = true;								// ��~������̓t���O��true��
		}
		if ((key[KEY_INPUT_SPACE]) && this->m_IsStopKeyFirst) {			// �X�y�[�X�L�[�����͂���Ă��āA����~������͂ł���
			this->m_IsStop = !this->m_IsStop;							// �v�Z��~�t���O�𔽓]
			this->m_IsStopKeyFirst = false;								// ��~������̓t���O��false��
		}
		*/// �����܂�

		if (!(padBuf & PAD_INPUT_7 || key[KEY_INPUT_PERIOD]) && !this->m_IsAdvanceKeyFirst) {		// �s���I�h�L�[�����͂���Ă��炸�A���R�}���菉����̓t���O�������Ă��Ȃ�
			this->m_IsAdvanceKeyFirst = true;							// �R�}���菉����̓t���O��true��
		}
		if ((padBuf & PAD_INPUT_7 || key[KEY_INPUT_PERIOD]) && this->m_IsAdvanceKeyFirst) {		// �s���I�h�L�[�����͂���Ă��āA���R�}���菉����͂ł���
			this->m_IsFrameAdvance = true;
			this->m_IsAdvanceKeyFirst = false;							// �R�}���菉����̓t���O��false��
		}

		if (!this->m_IsStop || this->m_IsFrameAdvance) {				// ��~���łȂ��A�܂��̓R�}����t���O��true
			this->m_IsFrameAdvance = false;
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

// �f�o�b�O���[�h�Q�b�^�[
bool Debug::GetIsDebugMode() {
	return m_IsDebugMode;
}