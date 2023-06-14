#include "Cursor.h"

Cursor::Cursor(int* imageHandle) {
	this->m_X = 0;					// X座標
	this->m_Y = 0;					// Y座標
	this->m_ImageId = 0;

	this->m_Image = imageHandle;	// 画像ハンドルポインタ
}

// 計算
void Cursor::Calc(float playerX, float playerY, bool onCursor, bool isShotAfter, int frameCount) {
	this->m_X = playerX;										// X座標
	this->m_Y = playerY - this->CURSOR_RANGE;					// Y座標

	if (isShotAfter) {
		this->m_ImageId = 1;
	}
	else {
		this->m_ImageId = 0;
	}
	if (onCursor && frameCount % 8 >= 4) {
		this->m_ImageId += 2;
	}
}

// 描画
void Cursor::Draw() {

	DrawGraph((int)this->m_Y, (int)this->m_X, this->m_Image[1 + this->m_ImageId], TRUE);

}

//X座標ゲッター
float Cursor::GetX() {
	return this->m_X;
}

//Y座標ゲッター
float Cursor::GetY() {
	return this->m_Y;
}