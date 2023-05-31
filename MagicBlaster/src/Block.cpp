#include "Block.h"
#include "Engine/Graphics.h"
#include "Engine/PSO.h"


Block::Block(Point2D point, AABB2D collision, bool canThrough) :
	GameObj(point), 
	_onActive(true), 
	_collision(this, collision.p, collision.hl), 
	_canThrough(canThrough)
{
	_collision.color = { 0, 0, 1, 0.5f };
}

void Block::Update(double delta) {
	if (!_onActive) {
		return;
	}

	SpriteUpdate(delta);
}

void Block::SpriteUpdate(double delta) {
	if (_pSprite != nullptr) {
		_pSprite->pos.x = _position.x;
		_pSprite->pos.y = _position.y;
	}
}

const AABB2DObj& Block::GetCollision() const {
	return _collision;
}