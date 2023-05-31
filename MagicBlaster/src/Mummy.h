#pragma once
#include "Enemy.h"
#include "primitive2dObj.h"

class Player;
class SpriteResource;

namespace {
	constexpr int HP_INIT = 8;
	constexpr float MOVE_SPEED = 75;	// 移動速度.
	constexpr float GRAVITY = -3600.0f;		// 重力加速度.

	constexpr int SCORE = 100;
}

class Mummy : public Enemy {
private:
	static const AnimationSequence _spawn;
	static const AnimationSequence _walk;
	static const AnimationSequence _death;

	enum AnimationID {
		Spawn,
		Walk,
		Death
	};

	/*
	enum State {
		Spawn,
		Walk,
		Death
	};
	*/

	static SpriteResource* _pSpriteResource;	// 描画情報のポインタ.

	SpriteHandle _spriteHandle;	// スプライトを使用するためのハンドル.
	bool _isDamage;
	double _deathTime;

	// 変数群.
	// 右壁チェック関連.
	AABB2DObj _forwardWallCheck;
	bool _isForwardWall;
public:
	Mummy(Point2D position = { 0, 0 }) :
		GameObj(position),
		_forwardWallCheck(this, { 32, 48 + 12 }, { 1, 84 }),
		_isForwardWall(false),
		_isDamage(false),
		_deathTime(0)
	{
		_hurtBox.emplace_back(this, AABB2D({ 0, 48 + 12 }, { 32, 84 }));
		_forwardWallCheck.color = { 1, 1, 0, 0.5f };
		_score = SCORE;
	};
	~Mummy() = default;

	static bool Load();
	static void pSetSpriteResource(SpriteResource* pSprRes) { _pSpriteResource = pSprRes; }
	static SpriteResource* pGetSpriteResource() { return _pSpriteResource; }

	bool Init();
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
	bool Damage(int damage, DrawObj* pHitObj) override;
	bool HitCheck();

	void ForwardWallCheck();
};