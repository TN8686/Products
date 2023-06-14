#include "LandingPoint.h"

LandingPoint::LandingPoint(int* image) {
	this->m_X = 0;				// X座標
	this->m_Y = 0;				// Y座標
	this->m_FrameCount = this->M_FRAME_MAX;		// フレームカウンタ
	this->m_Image = image;		// 画像ハンドルポインタ

}

void LandingPoint::Calc() {
	this->m_FrameCount++;

	if (this->m_FrameCount < this->M_FRAME_MAX) {
		this->m_Y += M_GROUND_MOVE;
	}
	else if (this->m_FrameCount == this->M_FRAME_MAX){
		this->m_X = 0;
		this->m_Y = 0;
	}
}

void LandingPoint::Draw() {
	DrawGraph((int)this->m_Y, (int)this->m_X, this->m_Image[5], TRUE);
}

void LandingPoint::Create(float cursorX, float cursorY) {
	this->m_X = cursorX;
	this->m_Y = cursorY;
	this->m_FrameCount = 0;
}

float LandingPoint::GetX() {
	return this->m_X;
}

float LandingPoint::GetY() {
	return this->m_Y;
}