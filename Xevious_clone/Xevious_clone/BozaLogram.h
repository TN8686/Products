#pragma once

#include "Enemy.h"

class BozaLogram : public Enemy
{
	public:
	BozaLogram(float x, float y, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP, Enemy** ppBozaLogram[]);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;

	static const int BOZA_LOGRAM_NUM = 5;		// ボザログラムと子ログラムの数
	static const int BOZA_LOGRAM_OFFSET = 12;	// 生成位置の座標オフセット
private:
	static const int M_LOGRAM_BROKEN_SCORE = 60;

	void ActiveCalc(float groundMove);

	Enemy** m_ppBozaLogram[BOZA_LOGRAM_NUM];	// ダブルポインタ配列
	bool m_isCalcFiast;							// 初回処理フラグ

};