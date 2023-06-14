#include "Blaster.h"

Blaster::Blaster(int* imageHndleP) {
	this->m_X = 0;
	this->m_Y = 0;

	this->m_Speed = 0;							// Y移動速度
	this->m_Acceleration = 0.125;				// Y加速度
	this->m_FrameCount = this->M_FRAME_MAX + 1;							// 経過フレーム
	this->m_Size = 0;								// サイズ
	this->m_ColorId = 0;							// 現在のカラー配列のID
	this->m_Image = imageHndleP;
}

// 計算
void Blaster::Calc() {
	//フレームカウントが0でも最大値でもない時
	if (this->m_FrameCount != 0 && this->m_FrameCount != this->M_FRAME_MAX) {

		// サイズが2未満で、8の倍数の時
		if (this->m_Size < 2 && this->m_FrameCount % 8 == 0) {
			// サイズ変更
			this->m_Size++;
		}

		// 4の倍数の時と、カラーIDが6の時にIDをインクリメント　原作の奇妙な部分の再現のためこうなっている
		if (this->m_FrameCount % 4 == 0 || this->m_ColorId == 6) {
			// カラー変更
			this->m_ColorId++;
		}
	}

	// カラーIDが最大値未満の時
	if (this->m_FrameCount < this->M_FRAME_MAX) {
		// フレームカウントをインクリメント
		this->m_FrameCount++;

		// 座標と速度を更新
		this->m_Y += this->m_Speed;
		this->m_Speed -= this->m_Acceleration;
	}
	if(this->m_FrameCount == this->M_FRAME_MAX){	// 最大フレームだったら初期化
		this->m_X = 0;
		this->m_Y = 0;
		this->m_Speed = 0;
		this->m_Size = 0;
		this->m_ColorId = 0;
	}
}

void Blaster::Draw() {
	DrawGraph(static_cast<int>(this->m_Y), static_cast<int>(this->m_X), this->m_Image[12 + this->m_Size + this->m_ColorArray[this->m_ColorId] * 3], TRUE);
}


void Blaster::Create(float playerX, float playerY) {
	this->m_X = playerX;
	this->m_Y = playerY;
	this->m_FrameCount = 0;
}