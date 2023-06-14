#include "HitChecker.h"

HitChecker::HitChecker(EnemyManager* enemyManagerP, BulletManager* bulletManagerP, Player* playerP, SoundManager* soundManagerP) {
	memset(&m_EnemyHit, false, sizeof(EnemyManager::ENEMY_HIT));
	memset(&m_ZapperHit, false, sizeof(BulletManager::ZAPPER_HIT));

	m_enemyManagerP = enemyManagerP;
	m_bulletManagerP = bulletManagerP;
	m_playerP = playerP;
	m_soundManagerP = soundManagerP;
}

void HitChecker::Calc() {
	// 初期化
	memset(&m_EnemyHit, false, sizeof(EnemyManager::ENEMY_HIT));
	memset(&m_ZapperHit, false, sizeof(BulletManager::ZAPPER_HIT));

	// ヒットチェック
	AerialHitCheck();
	GroundHitCheck();
	SparioHitCheck();

	// ヒット情報を渡す
	m_enemyManagerP->SetEnemyHit(m_EnemyHit);
	m_bulletManagerP->SetZapperHit(m_ZapperHit);
}

// 空中敵ヒットチェック
void HitChecker::AerialHitCheck() {
	bool ret;

	for (int i = 0; i < AERIAL_NUM; i++) {					// 空中敵チェックループ
		if (m_enemyManagerP->GetAerialIsCollision(i)) {	// 食らい判定がある
			for (int j = 0; j < ZAPPER_NUM; j++) {				// ザッパーチェックループ
				if (m_bulletManagerP->GetZapperOnActive(j)) {
					ret = HitCheck(
						m_bulletManagerP->GetZapperX(j), m_bulletManagerP->GetZapperY(j),		// 攻撃判定座標
						BulletManager::M_BULLET_SIZE, BulletManager::M_BULLET_SIZE,							// 攻撃判定幅、高さ
						m_enemyManagerP->GetAerialX(i) + EnemyManagerConst::HURTBOX_ORIGIN,				// 食らい判定X座標
						m_enemyManagerP->GetAerialY(i) + EnemyManagerConst::HURTBOX_ORIGIN,				// 食らい判定Y座標
						EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);						// 食らい判定幅、高さ

					if (!m_EnemyHit.aerial[i]) {		// ヒット情報がfalseだったら
						m_EnemyHit.aerial[i] = ret;	// 結果をセット
					}
					if (!m_ZapperHit.enemy[j]) {		// ヒット情報がfalseだったら
						m_ZapperHit.enemy[j] = ret;	// 結果をセット
					}
				}
			}

			// プレイヤーヒットチェック
			if (m_playerP->GetOnActive()) {	// プレイヤーがアクティブな時
				ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
					m_enemyManagerP->GetAerialX(i) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetAerialY(i) + EnemyManagerConst::HURTBOX_ORIGIN,
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

				if (ret) {
					m_playerP->SetIsHit(true);
				}
			}
		}
	}
}

// 地上敵ヒットチェック
void HitChecker::GroundHitCheck() {
	bool ret = false;
	bool isBlasterHit = false;
	bool onCursor = false;

	// オンカーソル判定
	for (int i = 2; i < GROUND_NUM; i++) {					// 地上敵チェックループ　1と2は例外枠で反応しない
		if (m_enemyManagerP->GetGroundIsCollision(i)) {	// 食らい判定が存在
			ret = HitCheck(
				m_bulletManagerP->GetCursorX(), m_bulletManagerP->GetCursorY(),			// カーソル座標
				BulletManager::M_CURSOR_SIZE, BulletManager::M_CURSOR_SIZE,				// カーソル幅、高さ
				m_enemyManagerP->GetGroundX(i) + EnemyManagerConst::HURTBOX_ORIGIN,		// 食らい判定X座標
				m_enemyManagerP->GetGroundY(i) + EnemyManagerConst::HURTBOX_ORIGIN,		// 食らい判定Y座標
				EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);		// 食らい判定幅、高さ

			if (ret) {									// 一度でもヒットしたら
				onCursor = true;						// trueにしてループ脱出
				break;
			}
		}
	}
	m_bulletManagerP->SetOnCursor(onCursor);		// 結果をセット

	// ブラスターヒット判定
	ret = false;	// 初期化
	if (m_bulletManagerP->GetBlasterFrameCount() == BulletManager::M_BLASTER_FRAME - 1) {	//	ブラスター着弾フレームの1f前
		for (int i = 0; i < GROUND_NUM; i++) {					// 地上敵チェックループ
			if (m_enemyManagerP->GetGroundIsCollision(i)) {	// 食らい判定が存在
				ret = HitCheck(
					m_bulletManagerP->GetLandingPointX(), m_bulletManagerP->GetLandingPointY(),			// 攻撃判定座標
					BulletManager::M_BULLET_SIZE, BulletManager::M_BULLET_SIZE,							// 攻撃判定幅、高さ
					m_enemyManagerP->GetGroundX(i) + EnemyManagerConst::HURTBOX_ORIGIN,					// 食らい判定X座標
					m_enemyManagerP->GetGroundY(i) + EnemyManagerConst::HURTBOX_ORIGIN,					// 食らい判定X座標
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);					// 食らい判定幅、高さ


				//if (!m_EnemyHit.ground[i]) {		// ヒット情報がfalseだったら 複数の弾で判定を取る場合必要になる処理
					m_EnemyHit.ground[i] = ret;	// 結果をセット
				//}
				if (!isBlasterHit && ret) {				// ブラスターヒットがfalseで判定結果がtrueだったら
					isBlasterHit = true;				// ブラスターヒットをtrueに
				}

			}
		}
		if (isBlasterHit) {

			m_soundManagerP->NoisePlay(static_cast<int>(NoiseSeId::EnemyStriken_Blaster));
		}
	}

	// プレイヤーとSPフラッグのヒットチェック
	if (m_playerP->GetOnActive() && m_enemyManagerP->GetGroundCharcterId(0) == static_cast<int>(EnemyId::SPFLAG) &&
		!m_enemyManagerP->GetGroundIsCollision(0)) {	// プレイヤーがアクティブで、対象がSPフラッグで、食らい判定が無い時
		ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
			m_enemyManagerP->GetGroundX(0) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetGroundY(0) + EnemyManagerConst::HURTBOX_ORIGIN,
			EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

		if (ret) {
			m_EnemyHit.ground[0] = ret;
			m_playerP->Extend();
			m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::SpecialFlag_get));
		}
	}
}

// スパリオ（敵通常弾）ヒットチェック
void HitChecker::SparioHitCheck() {
	bool ret = false;

	if (m_playerP->GetOnActive()) {				// プレイヤーがアクティブ
		for (int i = 0; i < SPARIO_NUM; i++) {	// 弾チェックループ
			if (m_enemyManagerP->GetBulletStatus(i) == static_cast<int>(StatusId::ACTIVE)) {	// 弾がアクティブ
				ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
					m_enemyManagerP->GetBulletX(i) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetBulletY(i) + EnemyManagerConst::HURTBOX_ORIGIN,
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

				if (ret) {
					m_playerP->SetIsHit(true);	// 結果をセット
					break;							// ループ脱出
				}
			}
		}
	}
}

/******************************************************************
関数名：HitCheck

機能：ヒットチェックを行う基本関数です

引数：
	float型 HitBoxX, HitBoxY			(i)	攻撃判定座標
	int型	HitBoxWidth, HitBoxHeight	(i)	攻撃判定幅、高さ
	float型	HurtBoxX, HurtBoxY			(i)	食らい判定座標
	int型	HurtBoxWidth, HurtBoxHeight	(i)	食らい判定幅、高さ

返却値： bool型　二つの範囲が重なっているか否か　重なっている:true　重なっていない:false
*********************************************************************/
bool HitChecker::HitCheck(float HitBoxX, float HitBoxY, int HitBoxWidth, int HitBoxHeight, float HurtBoxX, float HurtBoxY, int HurtBoxWidth, int HurtBoxHeight) {
	if (HitBoxX <= HurtBoxX + HurtBoxWidth && HitBoxX + HitBoxWidth >= HurtBoxX &&
		HitBoxY <= HurtBoxY + HurtBoxHeight && HitBoxY + HitBoxHeight >= HurtBoxY) {
		return true;
	}
	return false;
}

