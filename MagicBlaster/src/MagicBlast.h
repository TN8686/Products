#pragma once
#include "DrawObj.h"
#include "Enemy.h"
#include "SootingDirection.h"
#include "Engine/Texture.h"
#include "primitive2dObj.h"

class Player;

enum class BlastLevel{
	Small,
	Medium, 
	Large,
	EnumBlastLevelNum

};

class MagicBlast : public DrawObj{
private:

	const AnimationSequence _blast = { { {0, 3.0 / 60.0}, {1, 3.0 / 60.0}, {2, 3.0 / 60.0}, {3, 3.0 / 60.0}, {4, 3.0 / 60.0}, {5, 3.0 / 60.0}, {6, 3.0 / 60.0}, {7, 3.0 / 60.0}}, false };
	static constexpr int ATTACK_TABLE[static_cast<int>(BlastLevel::EnumBlastLevelNum)] = { 100, 200, 300 };
	static constexpr float DISTANCE_TABLE[static_cast<int>(BlastLevel::EnumBlastLevelNum)] = { 128, 128, 256 };

	AnimationList _animationList;
	std::vector<Sprite> _spriteList;
	static Texture _texture;
	CellList _cellList;

	std::vector<std::unique_ptr<Enemy>>* _pEnemyList;
	std::vector<Enemy*> _hitEnemyList;
	Player* _pPlayer;

	AABB2DObj _collision;
	ShootingDirection _direction;
	BlastLevel _blastLevel;
	int _attack;
	float _distance;
	
public:
	MagicBlast() :
		_pEnemyList(nullptr),
		_pPlayer(nullptr),
		_direction(ShootingDirection::RIGHT),
		_blastLevel(BlastLevel::Small),
		_attack(ATTACK_TABLE[0]),
		_distance(DISTANCE_TABLE[0]),
		_collision(this)
	{
		_collision.color = { 1, 0.3f, 0, 0.5f };
	}
	~MagicBlast() = default;

	bool Init();
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
	void SetDirection(ShootingDirection direction);

	void SetEnemyList(std::vector<std::unique_ptr<Enemy>>* pEnemyList) { _pEnemyList = pEnemyList; }
	void SetPlayer(Player* pPlayer) { _pPlayer = pPlayer; }
	bool HitCheck();
	void Fire(Point2D position, ShootingDirection direction, BlastLevel blastLevel);
	void SetFirePosition(Point2D position, ShootingDirection direction, BlastLevel blastLevel);

	const std::vector<Sprite>* GetSpriteListPtr() const { return &_spriteList; }
	const CellList* GetCellListPtr() const { return &_cellList; }
	static Texture* GetTexturePtr() { return &_texture; }
};