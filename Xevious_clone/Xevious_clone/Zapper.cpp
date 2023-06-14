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

// �v�Z
void Zapper::Calc() {
	if (m_OnActive) {
		this->m_Y -= this->m_Speed;

	}
}

// �`��
void Zapper::Draw() {
	if (this->m_IsFlip) {	// ���]�t���O��true�Ȃ�
		// (m_X)�ƁA(m_X + m_Width)�����ւ��ĕ`��@����ō��E���]����
		DrawExtendGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X + this->m_Width),
			static_cast<int>(this->m_Y + this->m_Height), static_cast<int>(this->m_X),
			this->m_Image[this->m_AnimationId], true);
	}
	else {	// false�Ȃ�
		// ���ʂɕ`��
		DrawExtendGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X),
			static_cast<int>(this->m_Y + this->m_Height), static_cast<int>(this->m_X + this->m_Width),
			this->m_Image[this->m_AnimationId], true);
	}

}

// �A�j���[�V����ID�Z�b�^�[
void Zapper::SetAnimationId(int animationId) {
	this->m_AnimationId = animationId;
}

// ���E���]�t���O�Z�b�^�[
void Zapper::SetIsTurn(bool m_IsFlip) {
	this->m_IsFlip = m_IsFlip;
}

// �A�N�e�B�u��ԃZ�b�^�[
void Zapper::SetOnActive(bool onActive) {
	this->m_OnActive = onActive;
}

// X���W�Z�b�^�[
void Zapper::SetX(float x) {
	this->m_X = x;
}

// Y���W�Z�b�^�[
void Zapper::SetY(float y) {
	this->m_Y = y;
}

// �A�N�e�B�u��ԃQ�b�^�[
bool Zapper::GetOnActive() {
	return this->m_OnActive;
}

// X���W�Q�b�^�[
float Zapper::GetX() {
	return this->m_X;
}

// Y���W�Q�b�^�[
float Zapper::GetY() {
	return this->m_Y;
}