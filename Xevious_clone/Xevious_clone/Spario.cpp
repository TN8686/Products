#include "Spario.h"

Spario::Spario(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP)
	: Enemy(static_cast<int>(EnemyId::SPARIO), x, y, type, 0, imageHandleP, NULL) {
	// 記入されていない物は親クラスのコンストラクタに準じる
	this->SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[1]);		// X、Y移動速度を設定
	this->m_IsMove = false;
}

void Spario::Calc() {
	if (this->m_Status == static_cast<int>(StatusId::ACTIVE)) {	// 動作中

		if (this->m_IsMove) {	// 最初のフレームでは動かない
			//速度を座標に反映
			this->m_X += this->m_SpeedX;
			this->m_Y += this->m_SpeedY;
		}
		else {
			this->m_IsMove = true;
		}

	}
}

// 描画
void Spario::Draw(int colorID) {
	if (this->m_Status == static_cast<int>(StatusId::ACTIVE)) {	// 動作中

		DrawGraph((int)this->m_Y, (int)this->m_X, this->m_imageHandleP->spario[14 + colorID * 3], TRUE);
	}
}