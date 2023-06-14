#include "Zolbak.h"

Zolbak::Zolbak(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, int* pAerialLevel)
	: Enemy(static_cast<int>(EnemyId::ZOLBAK), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 20;
	m_pAerialLevel = pAerialLevel;
}

void Zolbak::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {								// ヒット判定
		AddScore();								// スコア加算
		*m_pAerialLevel -= LEVEL_DOWN_VALUE;	// レベルを2ダウン
		if (*m_pAerialLevel < 0) {				// 0未満だったら0にする
			*m_pAerialLevel = 0;
		}
		m_isCollision = false;					// 判定を無くす
		m_Status++;								// インクリメントで済ませる
	}

	switch (m_Status)			// 状態で分岐
	{
	case static_cast<int>(StatusId::ACTIVE):				// アクティブな場合
		m_Y += groundMove;		// 背景速度で移動
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// 破壊計算
		break;

	default:
		break;
	}
}


// 描画
void Zolbak::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][7], TRUE);	// 描画
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();																			// 破壊描画
		break;

	default:
		break;
	}
}