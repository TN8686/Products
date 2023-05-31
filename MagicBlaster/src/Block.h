#pragma once

#include "DrawObj.h"
#include "Engine/Sprite.h"
#include "Camera.h"
#include "primitive2dObj.h"

class Block : public virtual DrawObj {
private:
	bool _onActive;
	bool _canThrough;
	AABB2DObj _collision;

	CellList cellList;

public:
	Block(Point2D point = { 0, 0 }, AABB2D collision = { { 0, 0 }, { 32, 32 } }, bool canThrough = false);
	void Update(double delta);
	void SpriteUpdate(double delta);
	void SetCanThrough(bool canThrough) { _canThrough = canThrough; }
	bool GetCanThrough() const { return _canThrough; }
	const AABB2DObj& GetCollision() const;
	void SetCollision(AABB2D collision) { 
		_collision.p = collision.p;
		_collision.hl = collision.hl;
	};
};