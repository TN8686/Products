#pragma once
#include "Xevious_clone.h"

class Zapper
{
private:
	// 定数
	static const int M_SPEED = 6;
	static const int M_SIZE = 16;

	bool m_OnActive;	// アクティブ状態
	float m_X;			// X座標
	float m_Y;			// Y座標
	float m_Speed;		// 速度（Y）
	int m_Width;		// 幅
	int m_Height;		// 高さ
	int* m_Image;		// 画像ハンドルポインタ
	int m_AnimationId;	// アニメーションID
	bool m_IsFlip;		// 左右反転
	double m_Angle;		// 左右反転用の回転の値

public:
	Zapper(int* ImageHandle);

	void Calc();
	void Draw();

	void SetOnActive(bool onActive);
	void SetAnimationId(int animationId);
	void SetIsTurn(bool isTurn);
	void SetX(float X);
	void SetY(float Y);

	bool GetOnActive();
	float GetX();
	float GetY();
};