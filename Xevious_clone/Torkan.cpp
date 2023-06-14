#include "Torkan.h"

Torkan::Torkan(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::TORKAN), x, y, type, 0, imageHandleP, scoreManagerP) {
	// 記入されていない物は親クラスのコンストラクタに準じる
	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[1]);		// X、Y移動速度を設定
	m_Flame = 4;					//アニメーション用フレーム数初期値
	m_IsStop      = false;			//停止フラグ初期値
	m_IsReverse   = false;			//帰還フラグ初期値
	m_IsShotAfter = false;			//弾発射後フラグ初期化
	m_AnimationSum = 0;				//アニメーション加算初期値
	m_Score = 5;

	//ランダムなフレーム取得
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, MAX);
	m_RandFlame = RandFlame(rand);
}

void Torkan::Calc(float player_x, float player_y, float groundMove, bool isHit) {

	DamageFunction(isHit);	// 被弾情報処理

	switch (m_Status) {

	case static_cast<int>(StatusId::ACTIVE) :
		m_RandFlame--;

		if (m_RandFlame <= 0 && !m_IsReverse) {		// ランダムなフレームが0で、かつ帰還フラグがfalseなら
			m_IsStop = true;								// 停止フラグをtrue

			//弾発射処理
			if (!m_IsShotAfter) {							// 弾発射後フラグがfalseなら
				m_Shot();									// 弾を発射
				m_IsShotAfter = true;						// 弾発射後フラグをtrueに
			}

			// アニメーション処理
			m_Flame--;
			if (m_Flame <= 0) {

				//アニメーション加算処理
				if (m_AnimationSum <= 5) {
					m_AnimationSum++;
				}
				m_Flame = 4;

				//アニメーションが終わったら
				if (m_AnimationSum == 5) {
					SetSpeedFromAngle(Angle(player_x, player_y, m_X, m_Y), M_SPEED_TABLE[2]);		// X、Y移動速度を再設定
					m_IsReverse = true;	// 帰還フラグをtrue
					m_IsStop = false;		// 停止フラグをfalse
				}
			}
		}

		//停止フラグがtrueだったら
		if (m_IsStop) {
			m_Y += groundMove;	// 止まる(背景と等速で移動)
		}
		else {
			//速度を座標に反映
			m_X += m_SpeedX;
			m_Y += m_SpeedY;
		}

		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}

void Torkan::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][16 + m_AnimationSum], true);	// 後で引数からカラー設定
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}