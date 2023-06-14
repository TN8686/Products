#pragma once
#include "Xevious_clone.h"

class AreaManager
{
public:
	AreaManager();
	void Init();
	void Restart();

	void Calc(float groundMove);

	// ゲッター
	int GetAria();
	int GetLine();
	int GetGroundEnemyLine();
	int GetLoadAria();
	float GetY();
	bool GetIsStart();

	static constexpr int AREA_NUM = 16;							// エリア数
	static constexpr int LINE_MAX = 256;						// 1エリアあたりのライン数
	static constexpr int AREA_CHARACTER_WIDTH = 28;				// エリアのキャラクター単位の幅

private:
	static constexpr int M_START_LINE_MAX = 49;					// 開始時の森のライン数　LINE_MAXの値からすると50が正しいが、原作の挙動を考えると49の方が自然
	static constexpr int M_LOOP_AREA_NUM = 6;					// 最終エリアを抜けた時のループ先のエリアID
	static constexpr int M_LINE_INIT = 39;						// スタート時のライン
	static constexpr int M_LOAD_LINE_OFFSET = 12;				// ロード用ラインオフセット
	static constexpr int M_ENEMY_GROUND_MOVE_SUM_OFFSET = -2;	// 地上敵用背景移動累計オフセット

	bool m_isStart;					// 開始時の森フラグ
	int m_aria;						// 現在のエリア
	int m_loadAria;					// ロードするエリア
	int m_line;						// 現在のライン
	int m_groundEnemyLine;			// 地上敵用ライン
	float m_y;						// エリアY座標進行度
	float m_areaGroundMoveSum;		// エリア用地上移動累積
	float m_enemyGroundMoveSum;		// 地上敵用地上移動累積
};