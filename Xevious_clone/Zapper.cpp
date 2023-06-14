#include "Zapper.h"

Zapper::Zapper(int* ImageHandle) {
	this->m_OnActive = false;
	this->m_X = 0;
	this->m_Y = 0;
	this->m_Width = this->M_SIZE;
	this->m_Height = this->M_SIZE;
	this->m_Speed = (float)this->M_SPEED;
	this->m_Image = ImageHandle;
	this->m_AnimationId = 0;
	this->m_IsFlip = 0;
	this->m_Angle = 0;
}

// 計算
void Zapper::Calc() {
	if (m_OnActive) {
		this->m_Y -= this->m_Speed;

	}
}

// 描画
void Zapper::Draw() {
	if (this->m_IsFlip) {	// 反転フラグがtrueなら
		// (m_X)と、(m_X + m_Width)を入れ替えて描画　これで左右反転する
		DrawExtendGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X + this->m_Width),
			static_cast<int>(this->m_Y + this->m_Height), static_cast<int>(this->m_X),
			this->m_Image[this->m_AnimationId], true);
	}
	else {	// falseなら
		// 普通に描画
		DrawExtendGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X),
			static_cast<int>(this->m_Y + this->m_Height), static_cast<int>(this->m_X + this->m_Width),
			this->m_Image[this->m_AnimationId], true);
	}

}

// アニメーションIDセッター
void Zapper::SetAnimationId(int animationId) {
	this->m_AnimationId = animationId;
}

// 左右反転フラグセッター
void Zapper::SetIsTurn(bool m_IsFlip) {
	this->m_IsFlip = m_IsFlip;
}

// アクティブ状態セッター
void Zapper::SetOnActive(bool onActive) {
	this->m_OnActive = onActive;
}

// X座標セッター
void Zapper::SetX(float x) {
	this->m_X = x;
}

// Y座標セッター
void Zapper::SetY(float y) {
	this->m_Y = y;
}

// アクティブ状態ゲッター
bool Zapper::GetOnActive() {
	return this->m_OnActive;
}

// X座標ゲッター
float Zapper::GetX() {
	return this->m_X;
}

// Y座標ゲッター
float Zapper::GetY() {
	return this->m_Y;
}