#pragma once
#include "Bullet.h"

static constexpr int MAGIC_BULLET_NUM = 8;

class MagicBulletQueue {
private:
	std::vector<Bullet>& _magicBulletList;
public:
	MagicBulletQueue(std::vector<Bullet>& magicBulletList) : _magicBulletList(magicBulletList){}

	bool Create(ShootingDirection direction, Point2D position);
};