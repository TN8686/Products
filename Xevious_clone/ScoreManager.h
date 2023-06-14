#pragma once

#include <string>
#include "Xevious_clone.h"
#include "Player.h"
#include "SoundManager.h"

class ScoreManager
{
public:
	//定数
	static constexpr int USER_NAME_BUF = 11;
	static const int RANKING_MAX = 5;		//ランキング最大値

	typedef struct {
		int highScore[RANKING_MAX];							//ハイスコア
		char userName[RANKING_MAX][USER_NAME_BUF];			//ユーザーネーム
	}RANKING;

	ScoreManager(Player*, SoundManager*);	// コンストラクタ宣言
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
	int m_Score;							// スコア
	SoundManager* m_SoundManager;			// SoundManager型ポインタ
	Player* m_Player;						// Player型ポインタ
	int m_ExtendScore;						// 次回エクステンドするスコアの値
	int m_FirstExtendScore;					// 初回のエクステンド（残機アップ）に必要なスコアの値
	int m_NextExtend;						// 二回目以降のエクステンドに必要なスコアの加算値
	bool m_isFirstExtend;					// 初回エクステンドフラグ
	RANKING m_ranking;						// RANKING型　配列
	
	static const int M_MAX_SCORE = 999999;				//スコア上限
	static const int M_FIRST_EXTENDSSCORE = 2000;		//初回エクステンドスコアの値（2000）
	static const int M_NEXT_EXTENDSSCORE = 6000;		//二回目以降のエクステンドスコアの加算値（6000



};