#include "Bullet.h"
#include <cmath>
#include "Player.h"
#include "Enemy.h"
#include "Block.h"
#include "ScoreManager.h"
#include <assert.h>

std::vector<std::unique_ptr<Enemy>>* Bullet::_pEnemyList = nullptr;
Player* Bullet::_pPlayer = nullptr;
const std::vector<Block>* Bullet::_pBlockList = nullptr;

Bullet::Bullet(Point2D position) : 
	GameObj(position),
	_collision(this, { 0, 0 }, { 8, 8 }),
	_velocity(0, 0),
	_direction(ShootingDirection::UP),
	_isFirst(true)
{
	_collision.color = { 1, 0.3f, 0, 0.5f };
}

void Bullet::Init(Point2D position, ShootingDirection direction) {
	_position = position;
	SetDirection(direction);
	_isFirst = true;
}

void Bullet::Update(double delta) {
	if (_onActive) {

		if (_isFirst) {
			_isFirst = false;
		}
		else {
			_position.x += _velocity.x * delta;
			_position.y += _velocity.y * delta;
		}

		auto topLeft = _pCamera->GetTopLeft();
		auto bottomRight = _pCamera->GetBottomRight();
		if (_position.x - 16 > bottomRight.x) {
			_onActive = false;
		}
		if (_position.x + 16 < topLeft.x) {
			_onActive = false;
		}
		if (_position.y - 16 > bottomRight.y) {
			_onActive = false;
		}
		if (_position.y + 16 < topLeft.y) {
			_onActive = false;
		}

		HitCheck();

		SpriteUpdate(delta);
	}
}

void Bullet::SpriteUpdate(double delta) {
	if (_pSprite != nullptr) {
		_pSprite->onActive = _onActive;
		_pSprite->pos.x = _position.x;
		_pSprite->pos.y = _position.y;
	}
}

void Bullet::SetDirection(ShootingDirection direction) {
	_direction = direction;
	float rad = -0.7853981633974483f * static_cast<int>(direction);

	_velocity.x = -SPEED * sinf(rad);
	_velocity.y = -SPEED * cosf(rad);
}

bool Bullet::HitCheck(){
	assert(_pEnemyList != nullptr || _pBlockList != nullptr);
	if (_pEnemyList == nullptr || _pBlockList == nullptr) {
		return false;
	}

	bool ret = false;

	for (auto& e : *_pBlockList) {
		if (AABB2DObj::RectOnRect(_collision, e.GetCollision())) {
			ret = true;
			break;
		}
	}

	for (auto& e : *_pEnemyList) {
		if (AABB2DObj::RectOnRect(_collision, *e->GetHurtBox().data())) {
			ret = e->Damage(1, this);	// ‰¼‚É1ƒ_ƒ[ƒW“ü‚ê‚é.
			ScoreManager::GetIncetance().ComboGaugeReset();

			if (_pPlayer != nullptr && ret) {
				_pPlayer->AddMagicGauge(5);	// ‰¼‚É‰ÁŽZ.
			}
		}
	}

	if (ret) {
		SetOnActive(false);
	}

	return ret;
}