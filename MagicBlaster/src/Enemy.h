#pragma once
#include "DrawObj.h"
#include "IDamageable.h"

class Player;
class Block;

class Enemy : public DrawObj, public IDamageable {
protected:
	bool _isLeft;		// ���E.
	int _hp;			// HP.
	Float2 _velocity;	// �����x.
	Player* _pPlayer;	// �v���C���[�̃|�C���^.
	int _score;			// �X�R�A.

	// �n�`�`�F�b�N�֘A.
	const std::vector<Block>* _pBlockList;
	const std::vector<Block>* _pThroughBlockList;

public:
	Enemy(Point2D position = { 0, 0 }) :
		GameObj(position),
		_isLeft(true),
		_hp(0),
		_velocity(0, 0),
		_pPlayer(nullptr),
		_pBlockList(nullptr),
		_pThroughBlockList(nullptr)
	{};

	~Enemy() = default;

	bool virtual Init() = 0;
	void virtual Update(double delta) override = 0;
	void virtual SpriteUpdate(double delta) override = 0;
	bool virtual Damage(int damage, DrawObj* pHitObj) override = 0;

	void pSetPlayer(Player* player) { _pPlayer = player; }
	void SetIsLeft(bool isLeft) { _isLeft = isLeft; }
	void pSetBlockList(const std::vector<Block>* pBlockList) { _pBlockList = pBlockList; }
	void pSetThroughBlockList(const std::vector<Block>* pThroughBlockList) { _pThroughBlockList = pThroughBlockList; }
};