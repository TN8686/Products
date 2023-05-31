#pragma once
#include "Enemy.h"
#include "Block.h"

class Bat : public Enemy {
private:
	static const AnimationSequence _stay;
	static const AnimationSequence _fly;
	static const AnimationSequence _death;

	enum AnimationID {
		Stay,
		Fly,
		Death
	};

	/*
	enum State {
		Spawn,
		Walk,
		Death
	};
	*/

	static constexpr float MOVE_SPEED = 100;	// 移動速度.
	static constexpr float GRAVITY = -900.0f;		// 重力.
	static constexpr float JUMP_FORCE = 450.0f;		// 飛び立つ強さ.
	static constexpr float PLAYER_RANGE = 300.0f;		// プレイヤーを検知する範囲
	static constexpr int HP_INIT = 8;

	static constexpr int BLOCK_HIT_CHECK_NUM = 4;

	// それぞれ空る必要のない変数はstatic化する
	static int _batCount;
	static std::vector<Sprite> _spriteList;
	static AnimationList _animationList;
	static CellList _cellList;
	static Texture _texture;

	bool _isDamage;
	double _deathTime;


	// 変数群.
	// 天井チェック関連.
	AABB2DObj _ceilingCheck;
	bool _isCeiling;
	std::vector <const Block*> _ceilingObj;


	// アタック関連
	bool _attack = false;

	// プレイヤーチェック関連
	bool _inPlayerX = false;	// プレイヤーが攻撃範囲に入っているか
	bool _inPlayerY = false;


	
public:
	// 当たり判定の設定
	Bat(Point2D position = { 0, 0 }) :
		GameObj(position),
		_isDamage(false),
		_deathTime(0),
		_ceilingCheck(this, {0,-36}, {16,8}),
		_isCeiling(false)
	{
		_ceilingCheck.color = { 1,0,1,0.5f };
		_hurtBox.emplace_back(this, Point2D(0, 0), Float2(25, 25));
	};
	~Bat() = default;

	bool Init();
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;

	bool Damage(int damage, DrawObj* pHitObj) override;
	void Attack(double delta);

	static bool Load();
	static std::vector<Sprite>* GetSpriteListPtr();
	static const CellList* GetCellListPtr();
	static Texture* GetTexturePtr();

	// プレイヤーチェック
	bool PlayerCheck();
	bool CeilingCheck();
	bool HitCheck();
};