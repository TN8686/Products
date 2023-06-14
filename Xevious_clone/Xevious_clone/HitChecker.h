#pragma once

#include "BulletManager.h"
#include "EnemyManager.h"
#include "Player.h"

class HitChecker
{
public:
	HitChecker(EnemyManager* enemyManagerP, BulletManager* bulletManagerP, Player* playerP, SoundManager* soundManagerP);
	void Calc();
private:
	EnemyManager::ENEMY_HIT m_EnemyHit;
	BulletManager::ZAPPER_HIT m_ZapperHit;

	EnemyManager* m_enemyManagerP;
	BulletManager* m_bulletManagerP;
	Player* m_playerP;
	SoundManager* m_soundManagerP;

	void AerialHitCheck();
	void GroundHitCheck();
	void SparioHitCheck();

	bool HitCheck(float HitBoxX, float HitBoxY, int HitBoxWidth, int HitBoxHeight, float HurtBoxX, float HurtBoxY, int HurtBoxWidth, int HurtBoxHeight);

};