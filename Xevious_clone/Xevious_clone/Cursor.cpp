#include "Cursor.h"

Cursor::Cursor(int* imageHandle) {
	this->m_X = 0;					// X���W
	this->m_Y = 0;					// Y���W
	this->m_ImageId = 0;

	this->m_Image = imageHandle;	// �摜�n���h���|�C���^
}

// �v�Z
void Cursor::Calc(float playerX, float playerY, bool onCursor, bool isShotAfter, int frameCount) {
	this->m_X = playerX;										// X���W
	this->m_Y = playerY - this->CURSOR_RANGE;					// Y���W

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

// �`��
void Cursor::Draw() {

	DrawGraph((int)this->m_Y, (int)this->m_X, this->m_Image[1 + this->m_ImageId], TRUE);

}

//X���W�Q�b�^�[
float Cursor::GetX() {
	return this->m_X;
}

//Y���W�Q�b�^�[
float Cursor::GetY() {
	return this->m_Y;
}