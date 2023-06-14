#include "ScoreManager.h"

ScoreManager::ScoreManager(Player* p, SoundManager* s) {
	Init();

	m_SoundManager = s;												// SoundManager型ポインタ
	m_Player = p;													// Plauer型ポインタ

	// ハイスコア　将来的にロードするようにする
	for (int i = 0; i < RANKING_MAX; i++) {
		m_ranking.highScore[i] = 4000 - (500 * i);					//1位から５００ずつマイナス
	}
	strcpy_s(m_ranking.userName[0], USER_NAME_BUF, "M.Nakamura");
	strcpy_s(m_ranking.userName[1], USER_NAME_BUF, "Eirry Mou.");
	strcpy_s(m_ranking.userName[2], USER_NAME_BUF, "Evezoo End");
	strcpy_s(m_ranking.userName[3], USER_NAME_BUF, "S.Okamoto");
	strcpy_s(m_ranking.userName[4], USER_NAME_BUF, "S.Kojuma");
}	

void ScoreManager::Init() {
	m_Score = 0;													// スコア
	m_FirstExtendScore = M_FIRST_EXTENDSSCORE;						// 初回のエクステンド（残機アップ）に必要なスコアの値
	m_ExtendScore = m_FirstExtendScore;								// 次回エクステンドするスコアの値
	m_NextExtend = M_NEXT_EXTENDSSCORE;								// 二回目以降のエクステンドに必要なスコアの加算値
	m_isFirstExtend = true;											// 初回エクステンドフラグ
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
	if (m_Score >= m_ExtendScore) {								//次回エクステンドスコアを超えているかチェック
		m_Player->Player::Extend();								//超えていたらPlayer型ポインタからplayerのエクステンド関数を呼び出す
		m_SoundManager->SoundManager::SpecialSePlay();			//SoundManager型ポインタからSE再生関数を呼び出し、エクステンド音を鳴らす

		if (m_isFirstExtend) {									// 初回エクステンドだったら
			m_ExtendScore = m_NextExtend;						// 次回エクステンドスコアを二回目以降エクステンドスコアに
			m_isFirstExtend = false;							// 初回フラグをfalseに

		}
		else if (m_ExtendScore + m_NextExtend < M_MAX_SCORE) {	// 次回エクステンドスコアと二回目以降エクステンドスコアの合計が、スコア上限を超えていないかチェック
			m_ExtendScore += m_NextExtend;						// 超えていなかったら次回エクステンドスコアに二回目以降エクステンドスコアを加算
		}
	}
}

int ScoreManager::GetScore() {
	return m_Score;
}

//ランキング一位のスコアと、現在のスコアを比較して、高い方の値を返す関数
int ScoreManager::GetTopScore() {
	if (m_ranking.highScore[0] > m_Score) {
		return m_ranking.highScore[0];
	}
	else {
		return m_Score;
	}
	
}

/******************************************************************
関数名：RankingUpdate関数

機能：現在のスコアをランキングの適切な順位に入れ、それに合わせてユーザーネームも並び替える関数
スコアが同じ場合は低い方に入れる（原作での挙動は未確認…）
ランクインしている場合、現在のスコアが入った位置のユーザーネームは全て0x00で初期化されている状態にする

引数：

返却値：戻り値として、現在のスコアが何位に入ったのかを返す（-1～最大順位の値で、0以上は順位を表し、-1はランク外を表す）

注意事項：
*********************************************************************/
int ScoreManager::RankingUpdate() {

	int i;
	for (i = RANKING_MAX - 1; i >= 0; i--) {												// ランキング処理
		if (m_Score > m_ranking.highScore[i]) {												// チェック対象スコアより現在スコアの方が大きかったら
			if (i < RANKING_MAX - 1) {														// チェック対象が最下位じゃなかったら
				m_ranking.highScore[i + 1] = m_ranking.highScore[i];						// スコアのランクを下げる
				strcpy_s(m_ranking.userName[i + 1], USER_NAME_BUF, m_ranking.userName[i]);	// 名前のランクを下げる
			}
		}
		else {
			break;																			// ハイスコアの方が大きい時点で終了
		}
	}
	
	if (i < RANKING_MAX - 1) {																// ランク外じゃなかったら
		i++;																				// iを補正

		m_ranking.highScore[i] = m_Score;													// ランクインした位置に現在スコアを代入
		memset(m_ranking.userName[i], 0x00, sizeof(m_ranking.userName[i]));					// 名前欄を初期化

		return i;																			// 入ったランクを返す
	}
	else {																					// ランク外だったら
		return -1;																			// -1を返す
	}
}

/*int ScoreManager::RankingUpdate() {
	int seve;														//スコアの一時セーブ
	char charseve[USER_NAME_BUF];									//文字セーブ
	char hoge[USER_NAME_BUF] ;										//入れ替えようの配列
	int scoreSeve;													//スコアセーブ
	int rank = USER_NAME_BUF;										// 5
	int count = 0;													//入れ替えカウント
	scoreSeve = m_Score;											//スコアセーブ
	for (int i = 0; i < M_RANKING_MAX; i++) {						//バブルソート
		if (m_ranking.highScore[i] < scoreSeve) {
			seve = m_ranking.highScore[i];
			m_ranking.highScore[i] = scoreSeve;
			scoreSeve = seve;
			strcpy_s(charseve, USER_NAME_BUF,m_ranking.userName[i]);
			if (count < 1) {
				memset(m_ranking.userName[i], 0x00, USER_NAME_BUF);	//初回のみ0x00に初期化
			}
			else {
				strcpy_s(m_ranking.userName[i], USER_NAME_BUF, hoge);//初回以降保存された名前を入れる
			}
			strcpy_s(hoge, USER_NAME_BUF, charseve);
			rank--;													//順位　５からマイナスしていく
			count++;
		}
		else {
			rank = -1;												//ランク外
		}
	}
	return rank;
}*/

/******************************************************************
関数名：SetUserName関数

機能：引数として0～4の値とchar型文字列を入れると、引数の順位のユーザーネームに引数の文字列の先頭から10バイト分コピーする関数
第一引数に不正な値が入った場合は何もしない

引数：第一引数：int型　第二引数：char型配列

返却値：
	成功：0
	失敗：-1

注意事項：
	１：第一引数に不正な値が入った場合は何もしない
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

//引数としてRANKING型変数を入れると、自身のRANKING型変数に代入するだけの関数
void ScoreManager::SetRanking(RANKING ranking) {
	m_ranking = ranking;
}

//自身のRANKING型変数を戻り値として返すだけの関数
ScoreManager::RANKING ScoreManager::GetRanking() {
	return m_ranking;
}
