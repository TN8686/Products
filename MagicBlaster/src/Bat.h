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

	static constexpr float MOVE_SPEED = 100;	// �ړ����x.
	static constexpr float GRAVITY = -900.0f;		// �d��.
	static constexpr float JUMP_FORCE = 450.0f;		// ��ї�����.
	static constexpr float PLAYER_RANGE = 300.0f;		// �v���C���[�����m����͈�
	static constexpr int HP_INIT = 8;

	static constexpr int BLOCK_HIT_CHECK_NUM = 4;

	// ���ꂼ����K�v�̂Ȃ��ϐ���static������
	static int _batCount;
	static std::vector<Sprite> _spriteList;
	static AnimationList _animationList;
	static CellList _cellList;
	static Texture _texture;

	bool _isDamage;
	double _deathTime;


	// �ϐ��Q.
	// �V��`�F�b�N�֘A.
	AABB2DObj _ceilingCheck;
	bool _isCeiling;
	std::vector <const Block*> _ceilingObj;


	// �A�^�b�N�֘A
	bool _attack = false;

	// �v���C���[�`�F�b�N�֘A
	bool _inPlayerX = false;	// �v���C���[���U���͈͂ɓ����Ă��邩
	bool _inPlayerY = false;


	
public:
	// �����蔻��̐ݒ�
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

	// �v���C���[�`�F�b�N
	bool PlayerCheck();
	bool CeilingCheck();
	bool HitCheck();
};