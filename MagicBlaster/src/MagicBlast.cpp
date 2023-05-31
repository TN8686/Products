#include "MagicBlast.h"
#include "Player.h"
#include "Engine/GamePad.h"
#include "ScoreManager.h"

#include <Xinput.h>
Texture MagicBlast::_texture;

bool MagicBlast::Init() {

	_animationList.push_back(_blast);
	_spriteList.resize(5, Sprite(&_animationList, { 0,0,0 }, 0, { 4, 4 }));

	for (auto& e : _spriteList) {
		e.onActive = false;
	}
	_cellList.list.resize(8);
	if (CreateCellList(8, 4, 2, 32, 32, _cellList) != 0) {
		return false;
	};

	return true;

}

void MagicBlast::Fire(Point2D position, ShootingDirection direction, BlastLevel blastLevel) {
	_position = position;
	_onActive = true;
	_spriteList[0].onActive = true;
	_blastLevel = blastLevel;

	switch (blastLevel)
	{
	case BlastLevel::Small:
		_collision.hl = { 56, 56 };
		_distance = DISTANCE_TABLE[0];
		_spriteList[0].scale = { 4, 4 };
		break;

	case BlastLevel::Medium:
		_collision.hl = { 112, 112 };
		_distance = DISTANCE_TABLE[1];
		_spriteList[0].scale = { 8, 8 };
		break;

	case BlastLevel::Large:
		_collision.hl = { 224, 224 };
		_distance = DISTANCE_TABLE[2];
		_spriteList[0].scale = { 16, 16 };
		break;

	default:
		break;
	}
	SetDirection(direction);

	_spriteList[0].animeController.SetSeqIndex(0);
	_spriteList[0].animeController.Reset();
}

void MagicBlast::SetFirePosition(Point2D position, ShootingDirection direction, BlastLevel blastLevel) {
	_position = position;
	_onActive = false;
	_spriteList[0].onActive = false;
	_blastLevel = blastLevel;

	switch (blastLevel)
	{
	case BlastLevel::Small:
		_collision.hl = { 56, 56 };
		_distance = DISTANCE_TABLE[0];
		_spriteList[0].scale = { 4, 4 };
		break;

	case BlastLevel::Medium:
		_collision.hl = { 112, 112 };
		_distance = DISTANCE_TABLE[1];
		_spriteList[0].scale = { 8, 8 };
		break;

	case BlastLevel::Large:
		_collision.hl = { 224, 224 };
		_distance = DISTANCE_TABLE[2];
		_spriteList[0].scale = { 16, 16 };
		break;

	default:
		break;
	}
	SetDirection(direction);

	_spriteList[0].animeController.SetSeqIndex(0);
	_spriteList[0].animeController.Reset();
}

void MagicBlast::Update(double delta) {
	if (_onActive) {
		HitCheck();

		SpriteUpdate(delta);
	}
}

void MagicBlast::SpriteUpdate(double delta) {

	_spriteList[0].pos.x = _position.x;
	_spriteList[0].pos.y = _position.y;

	for (auto& e : _spriteList) {
		e.animeController.Update(delta);
		if (e.animeController.IsFinished()) {
			e.onActive = false;
		}
	}
	if (_spriteList[0].animeController.IsFinished()) {
		_onActive = false;
	}
}

void MagicBlast::SetDirection(ShootingDirection direction) {
	_direction = direction;
	float rad = -0.7853981633974483f * static_cast<int>(direction);
	_position.x += -_distance * sinf(rad);
	_position.y += -_distance * cosf(rad);
}

bool MagicBlast::HitCheck() {
	if (_pEnemyList == nullptr) {
		return false;
	}
	bool ret = false;
	for (auto& e : *_pEnemyList) {
		if (e->GetOnActive()) {
			if (AABB2DObj::RectOnRect(_collision, *e->GetHurtBox().data())) {
				ret = e->Damage(_attack, this);	// ダメージ.
				ScoreManager::GetIncetance().ComboGaugeReset();
				if (_pPlayer != nullptr) {
					int num = 0;
					switch (_blastLevel)
					{
					case BlastLevel::Small:
						num = 1;
						break;

					case BlastLevel::Medium:
						num = 5;
						break;

					case BlastLevel::Large:
						ScoreManager::GetIncetance().AddCombo(1);	// コンボを加算.
						num = 10;
						break;

					default:
						break;
					}
					if (ret) {
						_pPlayer->AddMagicGauge(num);
					}

				}
			}
		}
	}

	return ret;
}