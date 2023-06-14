#include "Jara.h"

Jara::Jara(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::JARA), x, y, type, 0, imageHandleP, scoreManagerP) {
	// 記入されていない物は親クラスのコンストラクタに準じる
	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[2]);		// X、Y移動速度を設定
	m_AccelerationX = 0.0625;				// X加速度
	m_isRightAway = false;					// 右逃走フラグ
	m_isLeftAway = false;					// 左逃走フラグ
	m_AnimationSum = 0;
	m_Flame = 2;
	m_Score = 15;
}

void Jara::Calc(float player_x, float player_y, float groundMove, bool isHit) {

	DamageFunction(isHit);	// 被弾情報処理

	switch(m_Status){

		case static_cast<int>(StatusId::ACTIVE):
		// 逃走判定
		if (!m_isRightAway && !m_isLeftAway &&									// 逃走フラグがどちらも立っていない
			((int)player_x / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) < 6 &&		// かつ自機との座標差が6キャラクタ未満
			((int)player_x / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) >= -6) {	// かつ自機との座標差が-6キャラクタ以下	原作での左右でのズレを再現
			if (player_x >= m_X) {
				m_isRightAway = true;		// 右から来てる時は右に逃げるフラグをON
				if (m_Type == 1) {		// タイプが1だったら
					m_Shot();				// 弾を発射
				}
			}
			else {
				m_isLeftAway = true;	// 左から来てる時は左に逃げるフラグをON
				if (m_Type == 1) {	//タイプ1だったら
					m_Shot();				// 弾を発射
				}
			}
		}

		// 逃走処理
		if (m_isRightAway) {
			m_SpeedX -= m_AccelerationX;	// 右に加速していく

			// アニメーション処理
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum += 1;
				if (m_AnimationSum >= 5) {
					m_AnimationSum = 0;
				}
				m_Flame += 2;
			}

		}
		if (m_isLeftAway) {
			m_SpeedX += m_AccelerationX;	// 左に加速していく

			// アニメーション処理
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum -= 1;
				if (m_AnimationSum < 0) {
					m_AnimationSum = 5;
				}
				m_Flame += 2;
			}

		}

		//速度を座標に反映
		m_X += m_SpeedX;
		m_Y += m_SpeedY;

		break;

		case static_cast<int>(StatusId::BROKEN):
			BrokenCalc(groundMove);
			break;

		default:
			break;
	}
}


// 描画
void Jara::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][72 + m_AnimationSum], true);	// 後で引数からカラー設定
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}
