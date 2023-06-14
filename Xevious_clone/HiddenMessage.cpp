#include "HiddenMessage.h"

HiddenMessage::HiddenMessage(float x, float y, ScoreManager* pScoreManager, UI* pUI)
	: Enemy(static_cast<int>(EnemyId::HIDDEN_MESSAGE), x, y, 0, 0, NULL, pScoreManager){
	m_drawFrameCount = DRAW_FRAME;
	m_pUI = pUI;
}

void HiddenMessage::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	if (isHit) {								// ヒット判定
		AddScore();								// スコア加算
		m_isCollision = false;					// 判定を無くす
		m_Status++;								// インクリメントで済ませる
		m_pUI->SetIsDrawHiddenMessage(true);	// 隠しメッセージ出力フラグをtrueに
	}

	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		m_Y += groundMove;
		break;

	case static_cast<int>(StatusId::BROKEN):
		if (m_drawFrameCount == 0) {
			m_pUI->SetIsDrawHiddenMessage(false);								// 隠しメッセージ出力フラグをfalseに
			m_Status = static_cast<int>(StatusId::NON_EXISTENT);				// 状態を非存在に
		}
		m_drawFrameCount--;
		break;

	default:
		break;
	}
}