#pragma once
#include "Enemy.h"

class Torkan :
    public Enemy
{
private:
	int m_Flame;				//アニメーションフレーム
	int m_RandFlame;			//プレイヤーに近づくランダムフレーム
	int m_IsStop;				//停止時にm_Xとm_Yを止めるフラグ
	int m_IsReverse;			//帰っていく用のフラグ
	int m_AnimationSum;			//アニメーション用の加算
	int m_IsShotAfter;			//弾発射終わりフラグ

	static const int MIN = 64;		//自機に近づくランダムの最小値
	static const int MAX = 127;		//自機に近づくランダムの最大値

public:
	Torkan(float x, float y, float player_x, float player_y, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);

	void Calc(float player_x, float player_y, float groundMove, bool isHit) override;
	void Draw(int colorID) override;
};

