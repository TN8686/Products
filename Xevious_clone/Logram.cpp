#include "Logram.h"

Logram::Logram(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, Enemy* pParentObject)
	: Enemy(static_cast<int>(EnemyId::LOGRAM), x, y, 0, 1, imageHandleP, scoreManagerP) {
	m_Score = 30;

	//ランダムなフレーム取得
	std::random_device rd;
	std::default_random_engine rand(rd());

	int max;
	shotIntervalMax >= MIN ? max = shotIntervalMax : max = MIN;

	std::uniform_int_distribution<int> RandFlame(MIN, max);
	m_seed = rand;
	m_rand = RandFlame;
	
	m_RandFlame = m_rand(m_seed);		// 乱数を取得
	m_ShotInterval = m_RandFlame * 8;	// 8倍して扱う
	
	m_Animation = 0;					// アニメーションID
	m_isAnimationIncrement = false;		// ショットアニメーションインクリメントフラグ

	m_pParentObject = pParentObject;	// 親オブジェクトポインタ
}

void Logram::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {
	DamageFunction(isHit);		// 被弾情報処理
	
	// 連鎖破壊処理
	if (m_pParentObject != NULL && m_pParentObject->GetStatus() == static_cast<int>(StatusId::BROKEN)) {	// 親オブジェクトがBROKENだったら
		m_Status = static_cast<int>(StatusId::BROKEN);														// 自身の状態をBROKENに
	}

	switch (m_Status)			// 状態で分岐
	{
	case static_cast<int>(StatusId::ACTIVE):				// アクティブな場合
		if (m_Y < NOT_SHOT_LINE &&
			m_ShotInterval <= 0) {					// 射撃不可ラインより上、かつショットインターバルが0だったら

			m_RandFlame = m_rand(m_seed);			// 乱数を取得
			m_ShotInterval = m_RandFlame * 8;		// ショットインターバルをリセット
			m_isAnimationIncrement = true;			// ショットアニメーションインクリメントフラグをtrueに
		}

		if (m_ShotInterval % 4 == 0) {				// ショットインターバルが4の倍数の時
			if (m_isAnimationIncrement) {			// ショットアニメーションインクリメントフラグがtrueなら
				m_Animation++;						// ショットアニメーションIDをインクリメント
			}
			else if (m_Animation != 0) {			// ショットアニメーションインクリメントフラグがfalseで、かつアニメーションが0番でない時
				m_Animation--;						// ショットアニメーションIDをデクリメント
			}

			if (m_Animation == 3) {					// ショットアニメーションが3番の時
				m_Shot();							// 弾を発射
				m_isAnimationIncrement = false;		// ショットアニメーションインクリメントフラグをfalseに
			}
		}

		m_ShotInterval--;							// ショットインターバルをデクリメント

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
void Logram::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph(static_cast<int>(m_Y), static_cast<int>(m_X), m_imageHandleP->little[colorID][36 + m_Animation], TRUE);	// 描画
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();														// 破壊描画
		break;

	default:
		break;
	}
}
