#include "Barra.h"

// Calc以外の仕様は共通なのでトーロイドを参照してください
Barra::Barra(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::BARRA), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 10;
}

/* 地上敵のCalcの引数は空中敵の物に加えて、カーソル座標、着弾地点座標も取ります
* グロブダー以外は使用しませんが、かならずこの引数のCalcにして下さい
* 引数に現在の地上スクロール速度と被弾情報を持つようにしました
* 地上スクロール速度を使用する場合はこちらを参照してください
*/
void Barra::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// 被弾情報処理

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
void Barra::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][23], TRUE);	// 描画
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// 破壊描画
		break;

	default:
		break;
	}
}