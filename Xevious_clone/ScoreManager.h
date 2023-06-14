#pragma once

#include <string>
#include "Xevious_clone.h"
#include "Player.h"
#include "SoundManager.h"

class ScoreManager
{
public:
	//�萔
	static constexpr int USER_NAME_BUF = 11;
	static const int RANKING_MAX = 5;		//�����L���O�ő�l

	typedef struct {
		int highScore[RANKING_MAX];							//�n�C�X�R�A
		char userName[RANKING_MAX][USER_NAME_BUF];			//���[�U�[�l�[��
	}RANKING;

	ScoreManager(Player*, SoundManager*);	// �R���X�g���N�^�錾
	void ScoreCalc(int);
	void Extend();
	int GetScore();
	int GetTopScore(); 
	int RankingUpdate();
	int SetUserName(int ,const char [USER_NAME_BUF]);
	void SetRanking(RANKING);
	RANKING GetRanking();
	void Init();

private:
	int m_Score;							// �X�R�A
	SoundManager* m_SoundManager;			// SoundManager�^�|�C���^
	Player* m_Player;						// Player�^�|�C���^
	int m_ExtendScore;						// ����G�N�X�e���h����X�R�A�̒l
	int m_FirstExtendScore;					// ����̃G�N�X�e���h�i�c�@�A�b�v�j�ɕK�v�ȃX�R�A�̒l
	int m_NextExtend;						// ���ڈȍ~�̃G�N�X�e���h�ɕK�v�ȃX�R�A�̉��Z�l
	bool m_isFirstExtend;					// ����G�N�X�e���h�t���O
	RANKING m_ranking;						// RANKING�^�@�z��
	
	static const int M_MAX_SCORE = 999999;				//�X�R�A���
	static const int M_FIRST_EXTENDSSCORE = 2000;		//����G�N�X�e���h�X�R�A�̒l�i2000�j
	static const int M_NEXT_EXTENDSSCORE = 6000;		//���ڈȍ~�̃G�N�X�e���h�X�R�A�̉��Z�l�i6000



};