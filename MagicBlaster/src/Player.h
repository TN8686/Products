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
	// アニメーションシークエンス　将来的には外部データからロードする.
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

	// 定数群.
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

	static constexpr float MOVE_SPEED = 300 * 1.2;	// 移動速度.
//	static constexpr float JUMP_FORCE = 1400.0f;	// ジャンプ力　重力3600.0fで縦4マス程度.
//	static constexpr float JUMP_FORCE = 1348.0f;	// ジャンプ力　重力3600.0fで縦4マススレスレの値.
	static constexpr float JUMP_FORCE = 1560.0f;	// ジャンプ力　重力3600.0fで縦5マス程度.

	static constexpr float GRAVITY = 3600.0f;		// 重力加速度.
	static constexpr float ASCEND_INTERRUPTION_VELOCITY = -500;		// ジャンプ中断時の速度.

	static constexpr double COYOTE_TIME = 5.0 / 60;		// コヨーテタイム時間.
	static constexpr double JUMP_BUFF_TIME = 4.0 / 60;	// ジャンプの先行入力受付時間.
	static constexpr double JUMP_UP_TIME = 20.0 / 60;	// ジャンプの先行入力受付時間.
	static constexpr float RESISTANCE = 1000.0f;		// 空気抵抗（ブラスト反動用）.
	static constexpr float VELOCITY_MAX = 2500.0f;		// 加速度上限.

	static constexpr double BULLET_INTERVAL_TIME = 3.0 / 60;			// バレット射撃間隔.
	static constexpr double BULLET_BUFF_TIME = 20.0 / 60;				// バレットバッファ時間.
	static constexpr double BLAST_RECAST_TIME = 36.0 / 60;				// ブラストリキャスト時間.
	static constexpr double THROUGH_BLOCK_UNCHECK_TIME = 10.0 / 60;		// スルーブロックすり抜け後にチェックしなくなる時間.
	static constexpr double THROUGH_INPUT_TIME = 20.0 / 60;				// すり抜けの二回押し受付時間.
	static constexpr double DAMAGE_TIME = 120.0f / 60;					// ダメージによるダウン時間.
	static constexpr double DAMAGE_INVINCIBLE_TIME = 120.0f / 60;		// ダメージによる無敵時間.
	static constexpr double DAMAGE_INVINCIBLE_FLASH_TIME = 4 / 60;		// 無敵時間中の点滅間隔.

	static constexpr int DPAD = GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT;

	static constexpr int BLOCK_HIT_CHECK_NUM = 16;

	static constexpr float MAGIC_CIRCLE_OFFSET_Y = 32;

	static constexpr float MAGIC_GAUGE_LOW = 150;
	static constexpr float MAGIC_GAUGE_MEDIUM = 250;
	static constexpr float MAGIC_GAUGE_MAX = 300;
	static constexpr float MAGIC_GAUGE_BUFF_TIME = 2.0;
	static constexpr float MAGIC_GAUGE_DOWN = 25.0f;

	static SpriteResource* _pSpriteResource;

	SpriteHandle _spriteAura;	// 現状は未使用　後々ゲージ状態の表現として使用予定.
	SpriteHandle _spritePlayer;	// プレイヤーキャラのスプライト.

	// 変数群.

	const GamePad& _gamePad;	// ゲームパッド入力.
	TimeManager& _timeManager;	// タイムマネージャ.

	// マジックゲージ.
	float _magicGauge;
	double _magicGaugeBuffTime;
	std::vector<Sprite>* _pSquareSprite;
	Sprite* _pMagicGaugeSprite;

	// 地上チェック関連.
	const std::vector<Block>* _pBlockList;
	const std::vector<Block>* _pThroughBlockList;

	AABB2DObj _groundCheck;
	bool _isGround;
	std::vector <const Block*> _groundHitObj;	// 接触している足場オブジェクトリスト.
	const Block* _topGroundObj;					// 最も高い位置の足場オブジェクト
	CountDownTimer _throughBlockUnCheckTimer;
	CountDownTimer _throughInputTimer;
	int _prevButtons;

	// 天井チェック関連.
	AABB2DObj _ceilingCheck;
	bool _isCeiling;
	std::vector <const Block*> _ceilingObj;

	// 右壁チェック関連.
	AABB2DObj _rightWallCheck;
	bool _isRightWall;
	std::vector <const Block*> _rightWallObj;

	// 左壁チェック関連.
	AABB2DObj _leftWallCheck;
	bool _isLeftWall;
	std::vector <const Block*> _leftWallObj;

	// 魔法陣関連.
	std::vector <MagicCircle> _magicCircleObj;
	MagicCircle _magicBlastCircleObj;
	std::vector <Sprite>* _pSprMagicCircle;
	GameObj _magicCircleOffsetObj;

	// マジックブラスト.
	MagicBlast* _pMagicBlast;
	double _blastRecastTime;

	AABB2DObj _bodyAtack;
	std::vector<std::unique_ptr<Enemy>>* _pEnemyList;

	State _state;	// 状態.
	ShootingDirection _direction;	// 攻撃方向.
	bool _isLeft;	// 左右.
	bool _canJump;	// ジャンプ可能.

	Float2 _velocity;	// 加速度.
	float _recoilVelocityX;	// 反動加速度.
	Float2 _recoilVelocity;	// 反動加速度.
	bool _isRightRecoil;	// 反動方向（不要？）.
	bool _isUpRecoil;		// ブラストなどの反動上昇中flag.
	CountDownTimer _jumpBuffTimer;	// ジャンプバッファ.
	CountDownTimer _coyoteTimeTimer;	// コヨーテタイム.
	CountDownTimer _bulletIntervalTimer;	// 射撃間隔.
	CountDownTimer _bulletBuffTimer;	// 射撃継続時間.
	CountDownTimer _invincibleTimer;		// 無敵時間.
	CountDownTimer _invincibleFlashTimer;	// 無敵の点滅時間.
	CountDownTimer _damageTimer;		// ダメージダウン時間.
	double _stopTime;		// 時間停止期間.
	bool _isStop;			// 停止フラグ.

	MagicBulletQueue* _pBulletQueue;	// バレット生成器.
	bool _isBulletOffset;

	// 仮の振動関連.
	XINPUT_VIBRATION _viv = { 0, 0 };	// 振動情報.
	double _vibrationTime = 0;		// 振動時間.

	void GroundCheck(const std::vector<Block>& block, const std::vector<Block>& throughBlock);
	void RightWallCheck(const std::vector<Block>& block);
	void LeftWallCheck(const std::vector<Block>& block);
	void CeilingCheck(const std::vector<Block>& block);

	// アップデート関連.
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


