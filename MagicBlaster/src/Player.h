#pragma once
#include "DrawObj.h"
#include "Block.h"
#include "MagicCircle.h"
#include "Engine/Texture.h"
#include "SootingDirection.h"
#include "IDamageable.h"
#include "CountDownTimer.h"
#include "Engine/GamePad.h"
#include "TimeManager.h"
#include "primitive2dObj.h"

#include <Xinput.h>


class MagicBulletQueue;
class MagicBlast;
class SpriteResource;
class Enemy;

class Player : public virtual DrawObj, public IDamageable {
public:
	static constexpr int MAGIC_CIRCLE_NUM = 8;

private:
	// �A�j���[�V�����V�[�N�G���X�@�����I�ɂ͊O���f�[�^���烍�[�h����.
	static const AnimationSequence _idle;
	static const AnimationSequence _crouching;
	static const AnimationSequence _walk;
	static const AnimationSequence _jump;
	static const AnimationSequence _up;
	static const AnimationSequence _damage01;
	static const AnimationSequence _damage02;
	static const AnimationSequence _damage03;

	static const AnimationSequence _magicBullet;
	static const AnimationSequence _magicBulletUp;
	static const AnimationSequence _magicBulletDiagonallyAbove;

	static const AnimationSequence _magicBlastLarge;
	static const AnimationSequence _magicBlastLargeUp;
	static const AnimationSequence _magicBlastLargeDiagonallyAbove;
	static const AnimationSequence _magicBlastLargeDiagonalBottom;
	static const AnimationSequence _magicBlastLargeDown;

	// �萔�Q.
	enum {
		ANMID_Idle,
		ANMID_Crouching,
		ANMID_Walk,
		ANMID_Jump,
		ANMID_Up,
		ANMID_Damage01,
		ANMID_Damage02,
		ANMID_Damage03,
		ANMID_MagicBullet,
		ANMID_MagicBulletUp,
		ANMID_MagicBulletDiagonallyAbove,
		ANMID_MagicBlastLarge,
		ANMID_MagicBlastLargeUp,
		ANMID_MagicBlastLargeDiagonallyAbove,
		ANMID_MagicBlastLargeDiagonalBottom,
		ANMID_MagicBlastLargeDown,
		ANMID_MAX
	};

	enum class State {
		STID_Idle = 0x0001,
		STID_Crouching = 0x002,
		STID_Walk = 0x0004,
		STID_Jump = 0x0008,
		STID_Shot = 0x0010,
		STID_MagicBlastLarge = 0x0020,
		STID_Damage = 0x0040,
		STID_StateEnumMax
	};

	static constexpr float MOVE_SPEED = 300 * 1.2;	// �ړ����x.
//	static constexpr float JUMP_FORCE = 1400.0f;	// �W�����v�́@�d��3600.0f�ŏc4�}�X���x.
//	static constexpr float JUMP_FORCE = 1348.0f;	// �W�����v�́@�d��3600.0f�ŏc4�}�X�X���X���̒l.
	static constexpr float JUMP_FORCE = 1560.0f;	// �W�����v�́@�d��3600.0f�ŏc5�}�X���x.

	static constexpr float GRAVITY = 3600.0f;		// �d�͉����x.
	static constexpr float ASCEND_INTERRUPTION_VELOCITY = -500;		// �W�����v���f���̑��x.

	static constexpr double COYOTE_TIME = 5.0 / 60;		// �R���[�e�^�C������.
	static constexpr double JUMP_BUFF_TIME = 4.0 / 60;	// �W�����v�̐�s���͎�t����.
	static constexpr double JUMP_UP_TIME = 20.0 / 60;	// �W�����v�̐�s���͎�t����.
	static constexpr float RESISTANCE = 1000.0f;		// ��C��R�i�u���X�g�����p�j.
	static constexpr float VELOCITY_MAX = 2500.0f;		// �����x���.

	static constexpr double BULLET_INTERVAL_TIME = 3.0 / 60;			// �o���b�g�ˌ��Ԋu.
	static constexpr double BULLET_BUFF_TIME = 20.0 / 60;				// �o���b�g�o�b�t�@����.
	static constexpr double BLAST_RECAST_TIME = 36.0 / 60;				// �u���X�g���L���X�g����.
	static constexpr double THROUGH_BLOCK_UNCHECK_TIME = 10.0 / 60;		// �X���[�u���b�N���蔲����Ƀ`�F�b�N���Ȃ��Ȃ鎞��.
	static constexpr double THROUGH_INPUT_TIME = 20.0 / 60;				// ���蔲���̓�񉟂���t����.
	static constexpr double DAMAGE_TIME = 120.0f / 60;					// �_���[�W�ɂ��_�E������.
	static constexpr double DAMAGE_INVINCIBLE_TIME = 120.0f / 60;		// �_���[�W�ɂ�閳�G����.
	static constexpr double DAMAGE_INVINCIBLE_FLASH_TIME = 4 / 60;		// ���G���Ԓ��̓_�ŊԊu.

	static constexpr int DPAD = GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT;

	static constexpr int BLOCK_HIT_CHECK_NUM = 16;

	static constexpr float MAGIC_CIRCLE_OFFSET_Y = 32;

	static constexpr float MAGIC_GAUGE_LOW = 150;
	static constexpr float MAGIC_GAUGE_MEDIUM = 250;
	static constexpr float MAGIC_GAUGE_MAX = 300;
	static constexpr float MAGIC_GAUGE_BUFF_TIME = 2.0;
	static constexpr float MAGIC_GAUGE_DOWN = 25.0f;

	static SpriteResource* _pSpriteResource;

	SpriteHandle _spriteAura;	// ����͖��g�p�@��X�Q�[�W��Ԃ̕\���Ƃ��Ďg�p�\��.
	SpriteHandle _spritePlayer;	// �v���C���[�L�����̃X�v���C�g.

	// �ϐ��Q.

	const GamePad& _gamePad;	// �Q�[���p�b�h����.
	TimeManager& _timeManager;	// �^�C���}�l�[�W��.

	// �}�W�b�N�Q�[�W.
	float _magicGauge;
	double _magicGaugeBuffTime;
	std::vector<Sprite>* _pSquareSprite;
	Sprite* _pMagicGaugeSprite;

	// �n��`�F�b�N�֘A.
	const std::vector<Block>* _pBlockList;
	const std::vector<Block>* _pThroughBlockList;

	AABB2DObj _groundCheck;
	bool _isGround;
	std::vector <const Block*> _groundHitObj;	// �ڐG���Ă��鑫��I�u�W�F�N�g���X�g.
	const Block* _topGroundObj;					// �ł������ʒu�̑���I�u�W�F�N�g
	CountDownTimer _throughBlockUnCheckTimer;
	CountDownTimer _throughInputTimer;
	int _prevButtons;

	// �V��`�F�b�N�֘A.
	AABB2DObj _ceilingCheck;
	bool _isCeiling;
	std::vector <const Block*> _ceilingObj;

	// �E�ǃ`�F�b�N�֘A.
	AABB2DObj _rightWallCheck;
	bool _isRightWall;
	std::vector <const Block*> _rightWallObj;

	// ���ǃ`�F�b�N�֘A.
	AABB2DObj _leftWallCheck;
	bool _isLeftWall;
	std::vector <const Block*> _leftWallObj;

	// ���@�w�֘A.
	std::vector <MagicCircle> _magicCircleObj;
	MagicCircle _magicBlastCircleObj;
	std::vector <Sprite>* _pSprMagicCircle;
	GameObj _magicCircleOffsetObj;

	// �}�W�b�N�u���X�g.
	MagicBlast* _pMagicBlast;
	double _blastRecastTime;

	AABB2DObj _bodyAtack;
	std::vector<std::unique_ptr<Enemy>>* _pEnemyList;

	State _state;	// ���.
	ShootingDirection _direction;	// �U������.
	bool _isLeft;	// ���E.
	bool _canJump;	// �W�����v�\.

	Float2 _velocity;	// �����x.
	float _recoilVelocityX;	// ���������x.
	Float2 _recoilVelocity;	// ���������x.
	bool _isRightRecoil;	// ���������i�s�v�H�j.
	bool _isUpRecoil;		// �u���X�g�Ȃǂ̔����㏸��flag.
	CountDownTimer _jumpBuffTimer;	// �W�����v�o�b�t�@.
	CountDownTimer _coyoteTimeTimer;	// �R���[�e�^�C��.
	CountDownTimer _bulletIntervalTimer;	// �ˌ��Ԋu.
	CountDownTimer _bulletBuffTimer;	// �ˌ��p������.
	CountDownTimer _invincibleTimer;		// ���G����.
	CountDownTimer _invincibleFlashTimer;	// ���G�̓_�Ŏ���.
	CountDownTimer _damageTimer;		// �_���[�W�_�E������.
	double _stopTime;		// ���Ԓ�~����.
	bool _isStop;			// ��~�t���O.

	MagicBulletQueue* _pBulletQueue;	// �o���b�g������.
	bool _isBulletOffset;

	// ���̐U���֘A.
	XINPUT_VIBRATION _viv = { 0, 0 };	// �U�����.
	double _vibrationTime = 0;		// �U������.

	void GroundCheck(const std::vector<Block>& block, const std::vector<Block>& throughBlock);
	void RightWallCheck(const std::vector<Block>& block);
	void LeftWallCheck(const std::vector<Block>& block);
	void CeilingCheck(const std::vector<Block>& block);

	// �A�b�v�f�[�g�֘A.
	void Jump();
	void Move();
	void Direction();
	void Bullet(double delta);
	void Blast();
	void MagicGauge(double delta);
	void TerrainCheck(double delta);
public:
	Player(Point2D position = {0, 0});

	static bool Load();
	static void pSetSpriteResource(SpriteResource* pSprRes) { _pSpriteResource = pSprRes; }
	static SpriteResource* pGetSpriteResource() { return _pSpriteResource; }

	bool Init();
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
	bool Damage(int damage, DrawObj* pHitObj) override;

	bool SetSprMagicCircle(std::vector <Sprite>* pSprMagicCircle);
	void SetBulletQue(MagicBulletQueue* pBulletQueue) { _pBulletQueue = pBulletQueue; }
	void SetBlockList(const std::vector <Block>* blockList) { _pBlockList = blockList; }
	void SetThroughBlockList(const std::vector <Block>* throughBlockList) { _pThroughBlockList = throughBlockList; }
	void SetMagicBlast(MagicBlast* pMagicBlast) { _pMagicBlast = pMagicBlast; }
	void SetSquareSprite(std::vector <Sprite>* pSprite) { _pSquareSprite = pSprite; }

	void AddMagicGauge(int num);
	void SetEnemyList(std::vector<std::unique_ptr<Enemy>>* pEnemyList) { _pEnemyList = pEnemyList; }
	bool BodyAtackCheck();
};


