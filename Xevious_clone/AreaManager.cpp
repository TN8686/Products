#include "AreaManager.h"

AreaManager::AreaManager(){
	Init();
}

void AreaManager::Init() {
	m_aria = 0;												// エリア
	m_loadAria = 0;											// ロード用エリア
	m_isStart = true;
	m_line = M_LINE_INIT;									// ライン
	m_groundEnemyLine = M_LINE_INIT;						// 地上敵ライン
	m_y = M_LINE_INIT * CHARACTER_SIZE;						// エリアのY座標進行度
	m_areaGroundMoveSum = 0;								// エリアライン用
	m_enemyGroundMoveSum = M_ENEMY_GROUND_MOVE_SUM_OFFSET;	// 地上敵ライン用 2ドット分ズラす
}

void AreaManager::Restart() {
	// ここでミスした後の最終ラインに応じて次のエリアに行くかどうかの処理を入れる

	m_isStart = true;
	m_line = M_LINE_INIT;									// ライン
	m_groundEnemyLine = M_LINE_INIT;						// 地上敵ライン
	m_y = M_LINE_INIT * CHARACTER_SIZE;						// エリアのY座標進行度
	m_areaGroundMoveSum = 0;								// エリアライン用
	m_enemyGroundMoveSum = M_ENEMY_GROUND_MOVE_SUM_OFFSET;	// 地上敵ライン用 2ドット分ズラす
}

void AreaManager::Calc(float groundMove) {
	m_y += groundMove;				// 背景移動速度を累積
	m_areaGroundMoveSum += groundMove;
	m_enemyGroundMoveSum += groundMove;

	// エリアライン処理
	if (m_areaGroundMoveSum >= CHARACTER_SIZE) {				// キャラクタサイズを超えたら
		m_areaGroundMoveSum -= CHARACTER_SIZE;					// キャラクタサイズ分減算
		m_line++;												// ラインをインクリメント
		if (m_line + M_LOAD_LINE_OFFSET == LINE_MAX) {
			m_loadAria++;
		}

		if (m_line >= LINE_MAX || (m_isStart && m_line > M_START_LINE_MAX)) {	// エリア終端、または開幕の森で終端だったら
			if (m_isStart) {									// 開幕だったら
				m_y -= M_START_LINE_MAX * CHARACTER_SIZE;
				m_line -= M_START_LINE_MAX;						// lineを1に
			}
			else {
				m_y -= LINE_MAX * CHARACTER_SIZE;				// 減算は仮の値
				m_line -= LINE_MAX;								// lineを0に
				//m_aria++;										// 次のエリアへ　ただし現状は対応しない
				if (m_aria >= AREA_NUM) {						// 加算後のエリアIDが16以上だったら
					m_aria = M_LOOP_AREA_NUM;					// エリアID6（エリア7）に
				}
			}
		}
	}

	// 地上敵ライン処理　現状の仕様だと255の位置に地上敵が存在する場合前エリア終端で出現してしまうので注意（原作には存在しないが）
	if (m_enemyGroundMoveSum >= CHARACTER_SIZE) {				// キャラクタサイズを超えたら
		m_enemyGroundMoveSum -= CHARACTER_SIZE;					// キャラクタサイズ分減算
		m_groundEnemyLine++;;									// ラインをインクリメント
		if (m_groundEnemyLine >= LINE_MAX || (m_isStart && m_groundEnemyLine > M_START_LINE_MAX)) {	// エリア終端、または開幕の森で終端だったら
			if (m_isStart) {									// 開幕だったら
				m_groundEnemyLine -= M_START_LINE_MAX;			// lineを1に
				m_isStart = false;								// 開幕フラグをfalseに
			}
			else {
				m_groundEnemyLine -= LINE_MAX;			// lineを0に
			}
		}
	}
}

// 現在のエリアIDを返します　0～15の値　（原作で表示は無いが）表示上の値より1小さい事に注意
int AreaManager::GetAria() {
	return m_aria;
}

// 現在のエリアラインを返します　0～255の値
int AreaManager::GetLine() {
	return m_line;
}

// 現在のロード用エリアIDを返します　0～15の値　（原作で表示は無いが）表示上の値より1小さい事に注意
int AreaManager::GetLoadAria() {
	return m_loadAria;
}

// 現在のロード用エリアラインを返します　0～255の値
int AreaManager::GetGroundEnemyLine() {
	return m_groundEnemyLine;
}

// 現在のy座標を返します　0～2047の値
float AreaManager::GetY() {
	return m_y;
}

// 現在開始時の森かどうかを返します　true：開始時　false：通常エリア
bool AreaManager::GetIsStart() {
	return m_isStart;
}

