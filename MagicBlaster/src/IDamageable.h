#pragma once
#include "primitive2dObj.h"

class IDamageable {
protected:
	bool _isDamageable;
	std::vector<AABB2DObj> _hurtBox;
public:
	IDamageable() : _isDamageable(true) {};
	virtual ~IDamageable() = default;

	bool virtual Damage(int damage, DrawObj* pHitObj) = 0;

	const std::vector<AABB2DObj>& GetHurtBox() { return _hurtBox; }

};