#pragma once
#include "DrawObj.h"
#include "SootingDirection.h"
#include "primitive2dObj.h"

class Player;
class Enemy;
class Block;

class Bullet : public virtual DrawObj {
private:
	static constexpr float SPEED = 2000;

	static std::vector<std::unique_ptr<Enemy>>* _pEnemyList;
	static Player* _pPlayer;
	static const std::vector<Block>* _pBlockList;

	AABB2DObj _collision;
	Float2 _velocity;
	ShootingDirection _direction;

	bool _isFirst;
public:
	Bullet(Point2D position = { 0, 0 });
	void Init(Point2D position, ShootingDirection direction);
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
	void SetDirection(ShootingDirection direction);
	
	static void SetEnemyList(std::vector<std::unique_ptr<Enemy>>* pEnemyList) { _pEnemyList = pEnemyList; }
	static void SetPlayer(Player* pPlayer) { _pPlayer = pPlayer; }
	static void SetBlockList(std::vector<Block>* pBlocklist) { _pBlockList = pBlocklist; }
	bool HitCheck();
};