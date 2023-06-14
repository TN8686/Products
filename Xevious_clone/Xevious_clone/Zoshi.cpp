#include "Zoshi.h"

Zoshi::Zoshi(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::ZOSHI), x, y, type, 0, imageHandleP, scoreManagerP) {
	switch (m_Type)																// タイプで分岐
	{
	case 0:
		m_Score = 7;							// スコア									
		break;
	case 1:
	case 2:
		m_Score = 10;							// スコア
		break;

	default:
		break;
	}

	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);		// X、Y移動速度を設定
	this->m_Flame = 1;															//アニメーション用フレーム数初期値
	this->m_AnimationSum = 0;													//アニメーション加算初期値
	//ランダムなフレーム取得
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, MAX);						//64～127
	std::uniform_int_distribution<int> Rand(MIN2, MAX2);						//1～32
	this->m_seed = rand;
	this->m_rand = RandFlame;													//64～127
	this->m_rand2 = Rand;														//1～32
	this->m_Rand = this->m_rand2(this->m_seed);									//方向転換　1～32
	this->m_RandFlame = this->m_rand(this->m_seed);								//弾間隔　64～127 不明のためタルケン参考
}

void Zoshi::Calc(float playerX, float playerY, float groundMove, bool isHit) {
	DamageFunction(isHit);	// 被弾情報処理
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
	this->m_RandFlame--;
	this->m_Flame--;
	if (this->m_Flame <= 0) {																//フレームごと							
		if (this->m_AnimationSum <= 2) {													//アニメーション加算処理
			this->m_AnimationSum++;
		}
		else if (this->m_AnimationSum == 3) {												//ループのためリセット
			this->m_AnimationSum = 0;
		}
		this->m_Flame = 1;																	//毎フレーム
	}

	switch (m_Type)	// タイプで分岐
	{
	case 0:	// 上から出現し、方向転換はランダムな方向。70点
																							//弾発射処理
		if (this->m_RandFlame <= 0) {						// ランダムなフレームが0かつ弾発射後フラグがfalseなら
			this->m_Shot();																	// 弾を発射
			this->m_Rand = this->m_rand2(this->m_seed);										// 乱数を取得　１～３２
			this->SetSpeedFromAngle(static_cast<float>(m_Rand) * RAD_32, M_SPEED_TABLE[0]);	// X、Y移動速度を再設定
			this->m_RandFlame= this->m_rand(this->m_seed);									// 乱数を取得３２～１２７
		}
		break;
	case 1:	//上から出現し、方向転換はソルバルウの方向。100点
	case 2:	//ソルバルウとの X 距離 8 キャラクタ以上の画面下部から出現し、方向転換はソルバルウの方向
		if (this->m_RandFlame <= 0) {						// ランダムなフレームが0	// 弾発射後フラグがfalseなら
			this->m_Shot();																	// 弾を発射
			this->SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);	// X、Y移動速度を再設定
			this->m_RandFlame= this->m_rand(this->m_seed);									// 乱数を取得６４～１２７
		}
		break;
	default:	// どれにも一致しなかったらダミー
		break;
	}
	//移動加算
	this->m_X += this->m_SpeedX;
	this->m_Y += this->m_SpeedY;
	break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}
void Zoshi::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
	DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][32 + m_AnimationSum], true);
	break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}