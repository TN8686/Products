#pragma once

#include "Xevious_clone.h"
#include "Enemy.h"
#include <random>
class Zoshi :
    public Enemy
{
private:
	int m_Flame;				//アニメーションフレーム
	int m_RandFlame;			//プレイヤーに近づくランダムフレーム
	int m_Rand;					//ランダムフレーム
	int m_AnimationSum;			//アニメーション用の加算

	static const int MIN = 64;		//自機に近づくランダムの最小値
	static const int MAX = 127;		//自機に近づくランダムの最大値
	static const int MIN2 = 1;		//方向のランダムの最小値
	static const int MAX2 = 32;		//方向のランダムの最大値

	std::default_random_engine m_seed;			// 乱数生成器のシード
	std::uniform_int_distribution<int> m_rand;	// 乱数生成器
	std::uniform_int_distribution<int> m_rand2;	// 乱数生成器2

public:
	Zoshi(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};

