#include "BozaLogram.h"

BozaLogram::BozaLogram(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, Enemy** ppBozaLogram[])
	: Enemy(static_cast<int>(EnemyId::BARRA), x, y, 0, 1, imageHandleP, scoreManagerP) {

	m_Score = 200;			// スコア
	m_isCalcFiast = true;	// 初回処理フラグ

	for (int i = 0; i < BOZA_LOGRAM_NUM; i++) {
		m_ppBozaLogram[i] = ppBozaLogram[i];		// ダブルポインタを代入
	}
}

void BozaLogram::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// 被弾情報処理

	switch (m_Status)			// 状態で分岐
	{
	case static_cast<int>(StatusId::ACTIVE):				// アクティブな場合
		ActiveCalc(groundMove);
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// 破壊計算
		break;

	default:
		break;
	}
}

// 描画
void BozaLogram::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][62], TRUE);	// 描画
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// 破壊描画
		break;

	default:
		break;
	}
}

void BozaLogram::ActiveCalc(float groundMove) {
	if (m_isCalcFiast) {								// 初回処理
		Enemy* tempP = *m_ppBozaLogram[0];				// エネミーマネージャ内での本体と上ログラムの格納場所を入れ替える
		*m_ppBozaLogram[0] = *m_ppBozaLogram[4];
		*m_ppBozaLogram[4] = tempP;

		m_isCalcFiast = false;							// 初回処理フラグをfalseに
	}


	if (m_Score != M_LOGRAM_BROKEN_SCORE) {													// ログラム破壊後のスコアでなかったら
		for (int i = 0; i < BOZA_LOGRAM_NUM - 1; i++) {										// 子ログラムの状態をチェックする
			if (m_ppBozaLogram[i] != NULL && (*m_ppBozaLogram[i])->GetStatus() == static_cast<int> (StatusId::BROKEN)) {	// 破壊状態になっていたら
				m_Score = M_LOGRAM_BROKEN_SCORE;											// 自身のスコアを変更
			}
		}
	}

	m_Y += groundMove;		// 背景速度で移動
}