#include "Derota.h"

Derota::Derota(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::DEROTA), x, y, 0, 1, imageHandleP, scoreManagerP) {
	//ランダムなフレーム取得
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, shotIntervalMax);
	m_seed = rand;
	m_rand = RandFlame;

	this->m_RandFlame = this->m_rand(this->m_seed);	// 乱数を取得
	this->m_ShotInterval = this->m_RandFlame * 8;	// 8倍して扱う
	this->m_Score = 100;

}

void Derota::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {

	DamageFunction(isHit);		// 被弾情報処理

	switch (m_Status) {					// 状態がアクティブ

	case static_cast<int>(StatusId::ACTIVE):				// アクティブな場合
		if (this->m_Y <= TEMP_SCREEN_HEIGHT - TEMP_SCREEN_HEIGHT / 4 &&
			this->m_ShotInterval <= 0) {					// とりあえず仮に画面3/4以上だったら　かつ、ショットインターバルが0だったら

			this->m_Shot();								// 弾を発射
			this->m_RandFlame = this->m_rand(this->m_seed);	// 乱数を取得
			this->m_ShotInterval = m_RandFlame * 8;			// ショットインターバルをリセット

		}

		this->m_ShotInterval--;								// ショットインターバルをデクリメント

		m_Y += groundMove;
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);	// 破壊計算
		break;

	default:
		break;
	}
}

// 描画
void Derota::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][31], TRUE);	// 描画
		break;

	case static_cast<int>(StatusId::BROKEN):
	case static_cast<int>(StatusId::DESTROYED):
		BrokenDraw();              // 破壊描画
		break;

	default:
		break;
	}
}