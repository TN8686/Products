#include "ScoreManager.h"

ScoreManager::ScoreManager(Player* p, SoundManager* s) {
	Init();

	m_SoundManager = s;												// SoundManager�^�|�C���^
	m_Player = p;													// Plauer�^�|�C���^

	// �n�C�X�R�A�@�����I�Ƀ��[�h����悤�ɂ���
	for (int i = 0; i < RANKING_MAX; i++) {
		m_ranking.highScore[i] = 4000 - (500 * i);					//1�ʂ���T�O�O���}�C�i�X
	}
	strcpy_s(m_ranking.userName[0], USER_NAME_BUF, "M.Nakamura");
	strcpy_s(m_ranking.userName[1], USER_NAME_BUF, "Eirry Mou.");
	strcpy_s(m_ranking.userName[2], USER_NAME_BUF, "Evezoo End");
	strcpy_s(m_ranking.userName[3], USER_NAME_BUF, "S.Okamoto");
	strcpy_s(m_ranking.userName[4], USER_NAME_BUF, "S.Kojuma");
}	

void ScoreManager::Init() {
	m_Score = 0;													// �X�R�A
	m_FirstExtendScore = M_FIRST_EXTENDSSCORE;						// ����̃G�N�X�e���h�i�c�@�A�b�v�j�ɕK�v�ȃX�R�A�̒l
	m_ExtendScore = m_FirstExtendScore;								// ����G�N�X�e���h����X�R�A�̒l
	m_NextExtend = M_NEXT_EXTENDSSCORE;								// ���ڈȍ~�̃G�N�X�e���h�ɕK�v�ȃX�R�A�̉��Z�l
	m_isFirstExtend = true;											// ����G�N�X�e���h�t���O
}

void ScoreManager::ScoreCalc(int m_score) {
	m_Score += m_score;
	if (m_Score > M_MAX_SCORE)
	{
		m_Score = M_MAX_SCORE;
	}
	Extend();
}

void ScoreManager::Extend() {
	if (m_Score >= m_ExtendScore) {								//����G�N�X�e���h�X�R�A�𒴂��Ă��邩�`�F�b�N
		m_Player->Player::Extend();								//�����Ă�����Player�^�|�C���^����player�̃G�N�X�e���h�֐����Ăяo��
		m_SoundManager->SoundManager::SpecialSePlay();			//SoundManager�^�|�C���^����SE�Đ��֐����Ăяo���A�G�N�X�e���h����炷

		if (m_isFirstExtend) {									// ����G�N�X�e���h��������
			m_ExtendScore = m_NextExtend;						// ����G�N�X�e���h�X�R�A����ڈȍ~�G�N�X�e���h�X�R�A��
			m_isFirstExtend = false;							// ����t���O��false��

		}
		else if (m_ExtendScore + m_NextExtend < M_MAX_SCORE) {	// ����G�N�X�e���h�X�R�A�Ɠ��ڈȍ~�G�N�X�e���h�X�R�A�̍��v���A�X�R�A����𒴂��Ă��Ȃ����`�F�b�N
			m_ExtendScore += m_NextExtend;						// �����Ă��Ȃ������玟��G�N�X�e���h�X�R�A�ɓ��ڈȍ~�G�N�X�e���h�X�R�A�����Z
		}
	}
}

int ScoreManager::GetScore() {
	return m_Score;
}

//�����L���O��ʂ̃X�R�A�ƁA���݂̃X�R�A���r���āA�������̒l��Ԃ��֐�
int ScoreManager::GetTopScore() {
	if (m_ranking.highScore[0] > m_Score) {
		return m_ranking.highScore[0];
	}
	else {
		return m_Score;
	}
	
}

/******************************************************************
�֐����FRankingUpdate�֐�

�@�\�F���݂̃X�R�A�������L���O�̓K�؂ȏ��ʂɓ���A����ɍ��킹�ă��[�U�[�l�[�������ёւ���֐�
�X�R�A�������ꍇ�͒Ⴂ���ɓ����i����ł̋����͖��m�F�c�j
�����N�C�����Ă���ꍇ�A���݂̃X�R�A���������ʒu�̃��[�U�[�l�[���͑S��0x00�ŏ���������Ă����Ԃɂ���

�����F

�ԋp�l�F�߂�l�Ƃ��āA���݂̃X�R�A�����ʂɓ������̂���Ԃ��i-1�`�ő叇�ʂ̒l�ŁA0�ȏ�͏��ʂ�\���A-1�̓����N�O��\���j

���ӎ����F
*********************************************************************/
int ScoreManager::RankingUpdate() {

	int i;
	for (i = RANKING_MAX - 1; i >= 0; i--) {												// �����L���O����
		if (m_Score > m_ranking.highScore[i]) {												// �`�F�b�N�ΏۃX�R�A��茻�݃X�R�A�̕����傫��������
			if (i < RANKING_MAX - 1) {														// �`�F�b�N�Ώۂ��ŉ��ʂ���Ȃ�������
				m_ranking.highScore[i + 1] = m_ranking.highScore[i];						// �X�R�A�̃����N��������
				strcpy_s(m_ranking.userName[i + 1], USER_NAME_BUF, m_ranking.userName[i]);	// ���O�̃����N��������
			}
		}
		else {
			break;																			// �n�C�X�R�A�̕����傫�����_�ŏI��
		}
	}
	
	if (i < RANKING_MAX - 1) {																// �����N�O����Ȃ�������
		i++;																				// i��␳

		m_ranking.highScore[i] = m_Score;													// �����N�C�������ʒu�Ɍ��݃X�R�A����
		memset(m_ranking.userName[i], 0x00, sizeof(m_ranking.userName[i]));					// ���O����������

		return i;																			// �����������N��Ԃ�
	}
	else {																					// �����N�O��������
		return -1;																			// -1��Ԃ�
	}
}

/*int ScoreManager::RankingUpdate() {
	int seve;														//�X�R�A�̈ꎞ�Z�[�u
	char charseve[USER_NAME_BUF];									//�����Z�[�u
	char hoge[USER_NAME_BUF] ;										//����ւ��悤�̔z��
	int scoreSeve;													//�X�R�A�Z�[�u
	int rank = USER_NAME_BUF;										// 5
	int count = 0;													//����ւ��J�E���g
	scoreSeve = m_Score;											//�X�R�A�Z�[�u
	for (int i = 0; i < M_RANKING_MAX; i++) {						//�o�u���\�[�g
		if (m_ranking.highScore[i] < scoreSeve) {
			seve = m_ranking.highScore[i];
			m_ranking.highScore[i] = scoreSeve;
			scoreSeve = seve;
			strcpy_s(charseve, USER_NAME_BUF,m_ranking.userName[i]);
			if (count < 1) {
				memset(m_ranking.userName[i], 0x00, USER_NAME_BUF);	//����̂�0x00�ɏ�����
			}
			else {
				strcpy_s(m_ranking.userName[i], USER_NAME_BUF, hoge);//����ȍ~�ۑ����ꂽ���O������
			}
			strcpy_s(hoge, USER_NAME_BUF, charseve);
			rank--;													//���ʁ@�T����}�C�i�X���Ă���
			count++;
		}
		else {
			rank = -1;												//�����N�O
		}
	}
	return rank;
}*/

/******************************************************************
�֐����FSetUserName�֐�

�@�\�F�����Ƃ���0�`4�̒l��char�^�����������ƁA�����̏��ʂ̃��[�U�[�l�[���Ɉ����̕�����̐擪����10�o�C�g���R�s�[����֐�
�������ɕs���Ȓl���������ꍇ�͉������Ȃ�

�����F�������Fint�^�@�������Fchar�^�z��

�ԋp�l�F
	�����F0
	���s�F-1

���ӎ����F
	�P�F�������ɕs���Ȓl���������ꍇ�͉������Ȃ�
*********************************************************************/
int ScoreManager::SetUserName(int rank,const char userName[USER_NAME_BUF]) {
	if (rank >= 0 && rank < 5) {
		strcpy_s(m_ranking.userName[rank], USER_NAME_BUF, userName);
		return 0;
	}
	else {
		return -1;
	}
}

//�����Ƃ���RANKING�^�ϐ�������ƁA���g��RANKING�^�ϐ��ɑ�����邾���̊֐�
void ScoreManager::SetRanking(RANKING ranking) {
	m_ranking = ranking;
}

//���g��RANKING�^�ϐ���߂�l�Ƃ��ĕԂ������̊֐�
ScoreManager::RANKING ScoreManager::GetRanking() {
	return m_ranking;
}
