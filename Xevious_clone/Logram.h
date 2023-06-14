#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"

class Logram : public Enemy
{
private:
	int m_Animation;							// アニメーションID
	int m_RandFlame;							// ショットインターバルの元になるランダムな値
	bool m_isAnimationIncrement;				// ショットアニメーションインクリメントフラグ
	static constexpr int MIN = 4;					// 乱数の最低値
	//static const int MAX = 32;				// 乱数の最大値　引数から設定するように変更
	std::default_random_engine m_seed;			// 乱数生成器のシード
	std::uniform_int_distribution<int> m_rand;	// 乱数生成器

public:
	Logram(float x, float y, int shotIntervalMax, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerPint, Enemy* pBozaLogram);

	void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};