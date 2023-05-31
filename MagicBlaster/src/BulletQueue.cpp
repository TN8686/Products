#include "BulletQueue.h"

bool MagicBulletQueue::Create(ShootingDirection direction, Point2D position) {
	for (auto& e : _magicBulletList) {
		if (!e.GetOnActive()) {
			e.SetOnActive(true);
			e.Init(position, direction);

			return true;
		}
	}
	return false;
}