#include "Player.h"
#include "BulletQueue.h"
#include "MagicBlast.h"
#include "Engine/Graphics.h"
#include "ScoreManager.h"

#include <cmath>
#include <string>
#include <assert.h>
// TODO Stateの管理方式を変更して、それぞれのフラグを持ち最後に判定してアニメーションを変える等の処理をする形に.

Player::Player(Point2D position) :
	GameObj(position),
	_state(State::STID_Idle),
	_direction(ShootingDirection::RIGHT),
	_isLeft(false),
	_canJump(false),
	_magicGauge(0),
	_pBlockList(nullptr),
	_pThroughBlockList(nullptr),
	_topGroundObj(nullptr),
	_isGround(false),
	_groundCheck(this, { 0, 96 - 8 + 1 }, { 13, 8 }),	// Y座標はキャラ中心 - 判定幅 + 下に1ドット.
	_isCeiling(false),
	_ceilingCheck(this, { 0, -33 }, { 13, 8 }),
	_rightWallCheck(this, { 20, 32 }, { 8, 32 }),
	_isRightWall(false),
	_leftWallCheck(this, { -20, 32 }, { 8, 32 }),
	_isLeftWall(false),
	_pSprMagicCircle(nullptr),
	_pBulletQueue(nullptr),
	_velocity(0, 0),
	_recoilVelocityX(0),
	_recoilVelocity(0, 0),
	_isRightRecoil(false),
	_jumpBuffTimer(JUMP_BUFF_TIME, false, true),
	_coyoteTimeTimer(COYOTE_TIME, false, true),
	_bulletIntervalTimer(BULLET_INTERVAL_TIME, false, true),
	_bulletBuffTimer(BULLET_BUFF_TIME, false, true),
	_blastRecastTime(0),
	_pMagicBlast(nullptr),
	_pSquareSprite(nullptr),
	_magicGaugeBuffTime(0),
	_stopTime(0),
	_isStop(false),
	_isBulletOffset(false),
	_gamePad(GetGamePad(GamePadId_1P)),
	_timeManager(TimeManager::GetInstance()),
	_throughBlockUnCheckTimer(THROUGH_BLOCK_UNCHECK_TIME, false, true),
	_throughInputTimer(THROUGH_INPUT_TIME, false, true),
	_prevButtons(0),
	_invincibleTimer(DAMAGE_INVINCIBLE_TIME, false, true),
	_invincibleFlashTimer(DAMAGE_INVINCIBLE_FLASH_TIME, true),
	_damageTimer(DAMAGE_TIME, false, true),
	_pMagicGaugeSprite(nullptr)
	, _bodyAtack(this, Point2D(0, 0), Float2(80, 80))	// 仮の攻撃判定.
	, _isUpRecoil(false)
{
	_magicCircleOffsetObj.SetParent(this);
	_hurtBox.emplace_back(this, Point2D(0, 0), Float2(15, 32));
	_bodyAtack.SetOnActive(false);
}

// アニメーションシークエンス　本来は外部データからロードする.
const AnimationSequence Player::_idle = { { {0, 0.125}, {1, 0.125}, {2, 0.125}, {3, 0.125}, {4, 0.125}, {5, 0.125}, {6, 0.125}, {7, 0.125}, {0, 0.125 * 8} } };
const AnimationSequence Player::_crouching = { { {45, 4.0 / 60},{46, 4.0 / 60}}, false };
const AnimationSequence Player::_walk = { { {30, 8.0 / 60}, {31, 8.0 / 60}, {32, 8.0 / 60}, {33, 8.0 / 60}, {34, 8.0 / 60}, {35, 8.0 / 60} } };
const AnimationSequence Player::_jump = { { {58,2.0 / 60, 0},{58, 2.0 / 60, 1.570796f},{58, 2.0 / 60, 3.141592f},{58, 2.0 / 60, -1.570796f} } };
const AnimationSequence Player::_up = { { {127, 4.0 / 60},{128, 4.0 / 60}}, false };
const AnimationSequence Player::_damage01 = { { {121, 0.6},{136, 3.0 / 60},{137, 3.0 / 60}}, false };
const AnimationSequence Player::_damage02 = { { {138, 3.0 / 60} }, false };
const AnimationSequence Player::_damage03 = { { {139, 3.0 / 60}, {140, 3.0 / 60},{141, 3.0 / 60},{142, 2.0}}, false };
const AnimationSequence Player::_magicBullet = { { {166, 3.0 / 60},{167, 3.0 / 60},{168, 3.0 / 60}} };
const AnimationSequence Player::_magicBulletUp = { { {127, 4.5 / 60},{128, 4.5 / 60}} };
const AnimationSequence Player::_magicBulletDiagonallyAbove = { { {85, 3.0 / 60},{84, 3.0 / 60},{83, 3.0 / 60}} };
const AnimationSequence Player::_magicBlastLarge = { { {180, 3.0 / 60}, {181, 3.0 / 60}, {182, 9.0 / 60}, {183, 3.0 / 60}, {184, 3.0 / 60},{185, 3.0 / 60}}, false };
const AnimationSequence Player::_magicBlastLargeUp = { { {132, 3.0 / 60}, {131, 6.0 / 60},{130, 9.0 / 60},{129, 3.0 / 60},{127, 3.0 / 60}},false };
const AnimationSequence Player::_magicBlastLargeDiagonallyAbove = { { {0, 3.0 / 60}, {81, 6.0 / 60},{82, 9.0 / 60},{83, 3.0 / 60},{84, 3.0 / 60},{85, 3.0 / 60}},false };
const AnimationSequence Player::_magicBlastLargeDiagonalBottom = { { {54, 9.0 / 60},{55, 2.0 / 60}, {56, 2.0 / 60}, {57, 2.0 / 60}, {58, 2.0 / 60}, {59, 3.0 / 60}}, false };
const AnimationSequence Player::_magicBlastLargeDown = { { {54, 9.0 / 60},{55, 2.0 / 60}, {56, 2.0 / 60}, {57, 2.0 / 60}, {58, 2.0 / 60}, {212, 3.0 / 60}}, false };

SpriteResource* Player::_pSpriteResource;

bool Player::Load() {
	_pSpriteResource->pGetSpriteList()->reserve(2);	// オーラとキャラでとりあえず二つ.

	// セルリストの作成.
	if (!_pSpriteResource->CreateCellList(15, 15, 48, 48)) {
		return false;
	};

	// アニメーションリストの設定.
	AnimationList tempAnimList;

	tempAnimList.resize(ANMID_MAX);
	tempAnimList[ANMID_Idle] = _idle;
	tempAnimList[ANMID_Crouching] = _crouching;
	tempAnimList[ANMID_Walk] = _walk;
	tempAnimList[ANMID_Jump] = _jump;
	tempAnimList[ANMID_Up] = _up;
	tempAnimList[ANMID_Damage01] = _damage01;
	tempAnimList[ANMID_Damage02] = _damage02;
	tempAnimList[ANMID_Damage03] = _damage03;
	tempAnimList[ANMID_MagicBullet] = _magicBullet;
	tempAnimList[ANMID_MagicBulletUp] = _magicBulletUp;
	tempAnimList[ANMID_MagicBulletDiagonallyAbove] = _magicBulletDiagonallyAbove;
	tempAnimList[ANMID_MagicBlastLarge] = _magicBlastLarge;
	tempAnimList[ANMID_MagicBlastLargeDiagonallyAbove] = _magicBlastLargeDiagonallyAbove;
	tempAnimList[ANMID_MagicBlastLargeUp] = _magicBlastLargeUp;
	tempAnimList[ANMID_MagicBlastLargeDiagonalBottom] = _magicBlastLargeDiagonalBottom;
	tempAnimList[ANMID_MagicBlastLargeDown] = _magicBlastLargeDown;

	_pSpriteResource->SetAnimationList(tempAnimList);

	// 画像をロード.
	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/player_00.png")) {
		return false;
	}

	return true;
}

bool Player::Init() {

	// プレイヤースプライトの割り当て.
	_pSpriteResource->Register(_spriteAura);
	_pSpriteResource->Register(_spritePlayer);

	_spritePlayer.pGetSprite()->scale = { 4.0f, 4.0f };

	_spriteAura.pGetSprite()->onActive = true;	// ひとまず現状はオーラは非表示に.

	// 魔法陣オブジェクトの初期化.
	_magicCircleObj.resize(MAGIC_CIRCLE_NUM + 1);
	float rad = -0.7853981633974483f;
	for (int i = 0; i < MAGIC_CIRCLE_NUM + 1; ++i) {
		auto& obj = _magicCircleObj[i];
		obj.SetParent(&_magicCircleOffsetObj);

		Point2D pos;
		pos.x = -80 * sin(rad * i);
		pos.y = -80 * cosf(rad * i);
		obj.SetLocalPosition(pos);
	}

	// 初期化.
	_isDamageable = true;

	// 矩形スプライト関連.
	assert(_pSquareSprite != nullptr);
	if (_pSquareSprite != nullptr) {
		// マジックゲージのスプライトを用意.
		auto& s = _pSquareSprite->emplace_back(Sprite(nullptr, { 20, 20, 0 }));;
		s.cameraRelative = false;
		s.rectOffset.bottom = 20;
		s.rectOffset.right = 299;
		s.addColor = { 0.2f, 0.2f, 0.2f, 1.0f };

		_pMagicGaugeSprite = &_pSquareSprite->emplace_back(Sprite(nullptr, { 20, 20, 0 }));;
		_pMagicGaugeSprite->cameraRelative = false;
		_pMagicGaugeSprite->rectOffset.bottom = 20;
	}

	// 判定可視化の色指定.
	_hurtBox[0].color = { 1, 0, 0, 0.7f };
	_bodyAtack.color = { 1, 0.3f, 0, 0.5f };
	_rightWallCheck.color = { 1, 1, 0, 0.7f };
	_leftWallCheck.color = { 1, 1, 0, 0.7f };
	_ceilingCheck.color = { 1, 0, 1, 0.7f };
	_groundCheck.color = { 1, 0, 1, 0.7f };

	return true;
}

/*
* MagicCircleオブジェクトにスプライトをセットします.
*/
bool Player::SetSprMagicCircle(std::vector <Sprite>* pSprMagicCircle) {
	_pSprMagicCircle = pSprMagicCircle;
	assert(!(pSprMagicCircle == nullptr || pSprMagicCircle->size() < MAGIC_CIRCLE_NUM + 1));
	if (pSprMagicCircle == nullptr || pSprMagicCircle->size() < MAGIC_CIRCLE_NUM + 1) {
		return false;
	}

	for (int i = 0; i < MAGIC_CIRCLE_NUM + 1; ++i) {
		auto& obj = _magicCircleObj[i];
		auto spr = &_pSprMagicCircle->at(i);
		obj.pSetSprite(spr);
		obj.SetSprOnActive(false);
	}

	return true;
}

/*
* プレイヤーの更新.
*/
void Player::Update(double delta) {
	/*
	wchar_t str[256];	// デバッグ出力用.
	swprintf_s(str, L"%1f\n", _velocity.y);
	OutputDebugString(str);
	*/
	auto ucDelta = _timeManager.GetUncorrectedDelta();	// 補正前deltaを取得.
	_stopTime -= ucDelta;

	auto pSprite = _spritePlayer.pGetSprite();
	assert(pSprite != nullptr);

	// TODO 各種タイマーをここでまとめてアップデートするように変更.
	_damageTimer.Update(delta);
	_invincibleTimer.Update(delta);
	_throughBlockUnCheckTimer.Update(delta);
	_throughInputTimer.Update(delta);

	if (_damageTimer.IsReachedFrame()) {
		_state = State::STID_Idle;	// ステートを初期化.
		_invincibleTimer.Reset();
	}

	// 無敵関連.
	if (!_invincibleTimer.IsReached()) {	// 無敵時間中.
		_isDamageable = false;
		if (pSprite != nullptr) {
			auto a = pSprite->addColor.w;


			auto ratio = _invincibleTimer.GetRatio();
			if (ratio > 0.7) {
				a = -0.5f;
			}
			else {
				if (static_cast<int>(ratio * 100) % 4 < 2) {
					a = -0.8f;
				}
				else {
					a = -0.3f;
				}
			}

			pSprite->addColor.w = a;
		}
	}
	else if (_invincibleTimer.IsReachedFrame()) {	// 無敵時間終了フレーム.
		_isDamageable = true;

		assert(pSprite != nullptr);
		if (pSprite != nullptr) {
			auto a = pSprite->addColor.w;
			a = 0;
			pSprite->addColor.w = a;
		}
	}

	// ダメージ時間停止の解除　TODO 時間停止を区別する.
	/*
	if (_stopTime <= 0 && _isStop && _state == State::STID_Damage) {
		_pTimeManager->SetNextMagnification(1);
	}
	*/

	// 振動時間計測 TODO 振動関連は正式に実装したい.
	if (_vibrationTime >= 0) {
		_vibrationTime -= ucDelta;
	}
	else {	// 0以下で振動を解除.
		_viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);
	}

	_magicCircleObj[MAGIC_CIRCLE_NUM].SetStartUp(false);	// ブラスト用魔法陣をOFFにし続ける.
	// ブラストのリキャスト時間中.
	if (_blastRecastTime > 0) {
		if (!_isStop) {
			_blastRecastTime -= ucDelta;
		}
		_magicCircleObj[MAGIC_CIRCLE_NUM].SetStartUp(true);
		_magicCircleObj[MAGIC_CIRCLE_NUM].SetOnActive(true);
	}

	if (_state != State::STID_Damage) {	// ダメージ状態でない.

		// 時間停止解除時にブラスト発生　TODO 現状は時間停止したら常に撃っちゃってるので区別する.
		if (_stopTime <= 0 && _isStop) {
			// 振動発生.
			_viv.wLeftMotorSpeed = _viv.wRightMotorSpeed = 65535.0;
			XInputSetState(GamePadId_1P, &_viv);
			_vibrationTime = 1;

			// 生成.
			_pMagicBlast->Fire(_position, _direction, BlastLevel::Large);
			_magicGauge = 0;
			//_hurtBox[0].SetOnActive(true);		// 無敵解除はまだしない.
			_bodyAtack.SetOnActive(true);			// 攻撃判定をtrueに.
			auto c = _spritePlayer.pGetSprite()->color[0];
			_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 2, 0.5, 0.5, c.w };

			// 時間停止解除
			_isStop = false;
			_timeManager.SetNextMagnification(1);

			// 徐々にズーム解除.
			_pCamera->SetSecondTarget(_pMagicBlast);
			_pCamera->Zoom(0.8, 0.5);
			_pCamera->BlendLerp(0, 0.5);
		}
		if (delta == 0) {	//　時間停止中 TODO どの時間停止か区別する.
			SpriteUpdate(delta);
			_magicCircleObj[MAGIC_CIRCLE_NUM].Update(ucDelta);	// 補正前デルタで更新.
			return;
		}

		if (_bodyAtack.GetOnActive()) {
			BodyAtackCheck();
		}

		_state = State::STID_Idle;	// ステートを初期化　この先どの状態にもならなかったらIdleになる	あんまり良くないやり方な気がするので改善したい.

		Move();	// 左右移動.

		// TODO かなり無理矢理だけど一時的に、ブラスト撃った時の反動ですぐ空中モーション入るように　ブラスト処理に統合.
		if (_velocity.y < 0) {
			_state = State::STID_Jump;
		}

		Direction();	// 射撃方向設定.

		/*	// デバッグ用移動.
		if (gamepad.buttons & GamePad::DPAD_UP) {
			_position.y -= MOVE_SPEED * delta;
		}
		else if (gamepad.buttons & GamePad::DPAD_DOWN) {
			if (!_isGround) {
				_position.y += MOVE_SPEED * delta;
			}
		}
		*/

		Jump();	// ジャンプ系処理.

	}

	TerrainCheck(delta);	// 地形チェック.

	// Stateに応じた魔法陣の基準点や各種判定の位置調整.
	if (_state == State::STID_Jump) {
		_magicCircleOffsetObj.SetLocalPosition({ 0, 0 });
	}
	else if (_state == State::STID_Crouching) {
		_magicCircleOffsetObj.SetLocalPosition({ 0, MAGIC_CIRCLE_OFFSET_Y * 2 });
	}
	else {
		_magicCircleOffsetObj.SetLocalPosition({ 0, MAGIC_CIRCLE_OFFSET_Y });
	}
	_hurtBox[0].p.y = _magicCircleOffsetObj.GetLocalPosition().y;
	_rightWallCheck.p.y = _magicCircleOffsetObj.GetLocalPosition().y;
	_leftWallCheck.p.y = _magicCircleOffsetObj.GetLocalPosition().y;

	Bullet(delta);	// バレット生成.

	Blast();	// ブラスト生成.

	_prevButtons = _gamePad.buttons;	// 今回の入力を保持.

	MagicGauge(delta);

	for (auto& e : _magicCircleObj) {
		e.Update(delta);
	}

	SpriteUpdate(delta);
}

void Player::SpriteUpdate(double delta) {
	auto _delta = _timeManager.GetUncorrectedDelta();

	auto pSprite = _spritePlayer.pGetSprite();
	assert(pSprite != nullptr);
	if (pSprite != nullptr) {
		pSprite->flipHorizontal = !_isLeft;

		// スプライト座標の更新.
		pSprite->pos.x = _position.x;
		pSprite->pos.y = _position.y;

		// アニメーションの更新.
		pSprite->animeController.Update(_delta);

		// アニメーション切り替え.
		switch (_state)
		{
		case Player::State::STID_Idle:
			if (_direction == ShootingDirection::UP) {
				pSprite->animeController.SetSeqIndex(ANMID_Up);
			}
			else {
				pSprite->animeController.SetSeqIndex(ANMID_Idle);
			}
			break;
		case Player::State::STID_Crouching:
			pSprite->animeController.SetSeqIndex(ANMID_Crouching);
			break;
		case Player::State::STID_Walk:

			pSprite->animeController.SetSeqIndex(ANMID_Walk);
			break;
		case Player::State::STID_Jump:

			pSprite->animeController.SetSeqIndex(ANMID_Jump);
			break;
		case Player::State::STID_Shot:
			if (_direction == ShootingDirection::UP) {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBulletUp);
			}
			else if (_direction == ShootingDirection::RIGHT_UP || _direction == ShootingDirection::LEFT_UP) {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBulletDiagonallyAbove);
			}
			else {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBullet);
			}
			break;
		case Player::State::STID_MagicBlastLarge:
			if (_direction == ShootingDirection::UP) {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBlastLargeUp);
			}
			else if (_direction == ShootingDirection::RIGHT_UP || _direction == ShootingDirection::LEFT_UP) {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBlastLargeDiagonallyAbove);
			}
			else if (_direction == ShootingDirection::RIGHT_DOWN || _direction == ShootingDirection::LEFT_DOWN) {

				pSprite->animeController.SetSeqIndex(ANMID_MagicBlastLargeDiagonalBottom);
			}
			else if (_direction == ShootingDirection::DOWN) {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBlastLargeDown);
			}
			else {
				pSprite->animeController.SetSeqIndex(ANMID_MagicBlastLarge);
			}
			break;
		case Player::State::STID_Damage:
			if (_velocity.y < 0) {
				pSprite->animeController.SetSeqIndex(ANMID_Damage01);
			}
			else if (_velocity.y >= 0 && !_isGround) {
				pSprite->animeController.SetSeqIndex(ANMID_Damage02);
			}
			else {
				_velocity.x = 0;
				pSprite->animeController.SetSeqIndex(ANMID_Damage03);
			}
			break;
		default:
			break;
		}
	}
	auto pSpriteAura = _spriteAura.pGetSprite();
	*pSpriteAura = *pSprite;


	pSpriteAura->color[0] = pSpriteAura->color[1] = { 0, 0, 0, 0.9f };

	for (auto& e : _magicCircleObj) {
		e.SpriteUpdate(_delta);
	}

	assert(_pMagicGaugeSprite != nullptr);
	if (_pMagicGaugeSprite != nullptr) {
		_pMagicGaugeSprite->rectOffset.right = _magicGauge - 1;
		if (_magicGauge <= MAGIC_GAUGE_LOW) {	// TODO マジックナンバーなので定数から取るように.
			_pMagicGaugeSprite->addColor = { 0,1,0,1 };

			// オーラを消す.
			pSpriteAura->onActive = false;
		}
		else if (_magicGauge <= MAGIC_GAUGE_MEDIUM) {
			_pMagicGaugeSprite->addColor = { 1,1,0,1 };

			// 黄オーラ.
			pSpriteAura->pos.y -= 8;
			pSpriteAura->scale.x *= 1.1f;
			pSpriteAura->scale.y *= 1.1f;
			pSpriteAura->addColor.x = 1;
			pSpriteAura->addColor.y = 1;
			pSpriteAura->addColor.z = 0;
		}
		else {
			_pMagicGaugeSprite->addColor = { 1,0,0,1 };

			// 赤オーラ.
			pSpriteAura->pos.y -= 12;
			pSpriteAura->scale.x *= 1.2f;
			pSpriteAura->scale.y *= 1.2f;
			pSpriteAura->addColor.x = 1;
			pSpriteAura->addColor.y = 0;
			pSpriteAura->addColor.z = 0;
		}
	}

}

bool Player::Damage(int damage, DrawObj* pHitObj) {
	if (!_isDamageable || !_onActive) {
		return false;
	}
	_state = State::STID_Damage;
	_magicGauge -= damage;
	_magicGaugeBuffTime = 0;
	_isDamageable = false;
	_velocity.y = -1000;	// ダメージの吹っ飛び.
	_isUpRecoil = true;
	_recoilVelocity.x = 0;	// 反動を解除.
	_damageTimer.Reset();
	ScoreManager::GetIncetance().ComboBreak();

	//_stopTime = 0.6;	// 一旦時間停止をコメントアウト.
	//_isStop = true;
	//_pTimeManager->SetNextMagnification(0);

	if (_position.x < pHitObj->GetWorldPosition().x) {
		_velocity.x = -300;
		_isLeft = false;
	}
	else {
		_velocity.x = 300;
		_isLeft = true;
	}

	return true;


}

/// <summary>
/// 魔力ゲージを加算します.
/// </summary>
/// <param name="num">加算する値</param>
void Player::AddMagicGauge(int num) {
	_magicGaugeBuffTime = MAGIC_GAUGE_BUFF_TIME;
	_magicGauge += num;
	if (_magicGauge > MAGIC_GAUGE_MAX) {
		_magicGauge = MAGIC_GAUGE_MAX;
	}
}

// 地上との接触判定を行います.
void Player::GroundCheck(const std::vector<Block>& block, const std::vector<Block>& throughBlock) {
	_isGround = false;
	_groundHitObj.clear();

	if (_velocity.y < 0) {	// ジャンプ上昇中は判定しない.
		return;
	}

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_groundCheck, e.GetCollision())) {
			_isGround = true;
			_groundHitObj.push_back(&e);
		}
	}

	if (_throughBlockUnCheckTimer.IsReached()) {	// スルーブロックすり抜け中でない
		for (auto& e : throughBlock) {
			if (AABB2DObj::RectOnRect(_groundCheck, e.GetCollision())) {
				_isGround = true;
				_groundHitObj.push_back(&e);

			}
		}
	}
}


void Player::CeilingCheck(const std::vector<Block>& block) {
	_isCeiling = false;
	_ceilingObj.clear();

	if (_velocity.y >= 0) {	// 上昇中以外判定しない.
		return;
	}

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_ceilingCheck, e.GetCollision())) {
			_isCeiling = true;
			_ceilingObj.push_back(&e);
		}
	}
}

// 前方（右側）との接触判定を行います.
void Player::RightWallCheck(const std::vector<Block>& block) {
	_isRightWall = false;
	_rightWallObj.clear();

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_rightWallCheck, e.GetCollision())) {
			_isRightWall = true;
			_rightWallObj.push_back(&e);
		}
	}
}

// 後方（左側）との接触判定を行います.
void Player::LeftWallCheck(const std::vector<Block>& block) {
	_isLeftWall = false;
	_leftWallObj.clear();

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_leftWallCheck, e.GetCollision())) {
			_isLeftWall = true;
			_leftWallObj.push_back(&e);
			break;
		}
	}
}

/// <summary>
/// ジャンプ関連処理.
/// </summary>
void Player::Jump() {
	if (_gamePad.buttonDown & GamePad::A || _gamePad.buttonDown & GamePad::B) {
		_jumpBuffTimer.Reset();	// ジャンプを予約.
	}

	// ジャンプが予約されており、かつジャンプ可能ならジャンプ.
	if (_canJump && !_jumpBuffTimer.IsReached()) {
		_jumpBuffTimer.SetTime(JUMP_BUFF_TIME);
		_velocity.y = -JUMP_FORCE;
		_canJump = false;
		_state = State::STID_Jump;

		// 体当たり判定の解除　TODO 冗長なので後で修正.
		if (_bodyAtack.GetOnActive()) {
			_bodyAtack.SetOnActive(false);
			_hurtBox[0].SetOnActive(true);
			auto c = _spritePlayer.pGetSprite()->color[0];
			_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 1, 1, 1, c.w };
		}
	}
}

/// <summary>
/// 左右移動を行います.
/// </summary>
void Player::Move() {
	_velocity.x = 0.0f;			// 第一加速度を初期化.

	if (_gamePad.buttons & GamePad::DPAD_RIGHT) {
		if (!(_gamePad.buttons & GamePad::R) || !_isGround) {
			if (_isRightWall) {
				_velocity.x = 0.0f;
			}
			else {
				_velocity.x = MOVE_SPEED;
			}
			_state = State::STID_Walk;

		}
		_isLeft = false;
	}
	else if (_gamePad.buttons & GamePad::DPAD_LEFT) {
		if (!(_gamePad.buttons & GamePad::R) || !_isGround) {
			if (_isLeftWall) {
				_velocity.x = 0.0f;
			}
			else {
				_velocity.x = -MOVE_SPEED;
			}
			_state = State::STID_Walk;
		}
		_isLeft = true;
	}

	// スルーブロックすり抜け.
	if (_gamePad.buttonDown & GamePad::DPAD_DOWN) {
		if (!_throughInputTimer.IsReached() && !(_prevButtons & DPAD)  && !(_gamePad.buttons & GamePad::R) && _topGroundObj != nullptr && _topGroundObj->GetCanThrough()) {
			_throughBlockUnCheckTimer.Reset();
			_isGround = false;
			_canJump = false;
			_coyoteTimeTimer.SetTime(COYOTE_TIME);
			_jumpBuffTimer.SetTime(JUMP_BUFF_TIME);
			_state = State::STID_Jump;
		}
		else {
			_throughInputTimer.Reset();
		}
	}

	if (_topGroundObj != nullptr && _topGroundObj->GetCanThrough() && _gamePad.buttons & GamePad::DPAD_DOWN && (_gamePad.buttonDown & GamePad::A || _gamePad.buttonDown & GamePad::B)) {
		_throughBlockUnCheckTimer.Reset();
		_isGround = false;
		_canJump = false;
		_coyoteTimeTimer.SetTime(COYOTE_TIME);
		_jumpBuffTimer.SetTime(JUMP_BUFF_TIME);
		_state = State::STID_Jump;
	}
}

/// <summary>
/// 射撃方向を設定します.
/// </summary>
void Player::Direction() {
	if (_gamePad.buttons & GamePad::DPAD_UP) {			// 上系.
		if (_gamePad.buttons & GamePad::DPAD_RIGHT) {
			_direction = ShootingDirection::RIGHT_UP;		// 右上.
		}
		else if (_gamePad.buttons & GamePad::DPAD_LEFT) {
			_direction = ShootingDirection::LEFT_UP;		// 左上.
		}
		else {
			_direction = ShootingDirection::UP;				// 上.
		}
	}
	else if (_gamePad.buttons & GamePad::DPAD_DOWN) {	// 下系.
		if (_gamePad.buttons & GamePad::DPAD_RIGHT) {
			_direction = ShootingDirection::RIGHT_DOWN;		// 右下.
		}
		else if (_gamePad.buttons & GamePad::DPAD_LEFT) {
			_direction = ShootingDirection::LEFT_DOWN;		// 左下.
		}
		else if (_gamePad.buttons & GamePad::R || !_isGround) {
			_direction = ShootingDirection::DOWN;			// 下.
		}
		else {

			_direction = ShootingDirection::DOWN;			// 下.
			/*	// しゃがみ廃止.
			_state = State::STID_Crouching;				// しゃがみ系.
			
			if (_isLeft) {
				_direction = ShootingDirection::LEFT;		// しゃがみ左.
			}
			else {
				_direction = ShootingDirection::RIGHT;		// しゃがみ右.
			}
			*/
		}
	}
	else {												// 左右系.
		if (_isLeft) {
			_direction = ShootingDirection::LEFT;			// 左.
		}
		else {
			_direction = ShootingDirection::RIGHT;			// 右.
		}
	}
}

/// <summary>
/// バレット関連処理.
/// </summary>
/// <param name="delta">経過時間</param>
void Player::Bullet(double delta) {

	// バレット生成.
	_bulletIntervalTimer.Update(delta);	// 射撃間隔.

	// バレット用魔法陣を常にOFFにし続ける.
	for (int i = 0; i < MAGIC_CIRCLE_NUM; ++i) {
		_magicCircleObj[i].SetStartUp(false);
	}

	assert(_pBulletQueue != nullptr);
	if (_pBulletQueue != nullptr && _state != State::STID_Damage) {	// _pBulletQueueがnullでない、かつダメージ状態でない.

		_bulletBuffTimer.Update(delta);	// 射撃継続時間.

		if (_gamePad.buttonDown & GamePad::X) {
			_bulletBuffTimer.Reset();
		}

		if ((_gamePad.buttons & GamePad::X || !_bulletBuffTimer.IsReached()) && _blastRecastTime <= 0) {	// 射撃ボタンが押されている、または射撃継続時間中　かつブラスト硬直中でない.

			if (_state == State::STID_Idle) {	// 立ち状態の時.
				_state = State::STID_Shot;		// 射撃状態に.
			}
			_magicCircleObj[static_cast<int>(_direction)].SetOnActive(true);
			_magicCircleObj[static_cast<int>(_direction)].SetStartUp(true);

			if (_bulletIntervalTimer.IsReached()) {	// バレット射撃間隔が経過している.
				auto pos = _magicCircleObj[static_cast<int>(_direction)].GetWorldPosition();
				
				_bulletIntervalTimer.Reset();	// 射撃間隔を初期化.

				// 一発撃つ毎に生成座標を前にズラし、見栄えを整える.
				_isBulletOffset = !_isBulletOffset;	
				if (_isBulletOffset) {
					float rad = -0.7853981633974483f * static_cast<int>(_direction);

					pos.x += -16 * sinf(rad);
					pos.y += -16 * cosf(rad);
				}

				_pBulletQueue->Create(_direction, pos);

				/* 振動が二値のコントローラーだとクドくなってしまうので、バレットの振動は一旦無し.
				if (_viv.wRightMotorSpeed < 65535.0) {
					_viv.wRightMotorSpeed = 768;	// 振動最弱設定.
					_vibrationTime = 1.0 / 60;
				}
				XInputSetState(GamePadId_1P, &_viv);
				*/
			}
		}
	}
}

/// <summary>
/// ブラスト関連処理.
/// </summary>
void Player::Blast() {
	assert(_pMagicBlast != nullptr);
	if (_pMagicBlast != nullptr && _state != State::STID_Damage) {
		if (_gamePad.buttonDown & GamePad::Y && _blastRecastTime <= 0) {
			_isUpRecoil = true;	// ブラスト反動フラグ.

			_canJump = false;	// ジャンプ不可に.
			if (_state == State::STID_Crouching) {
				_direction = ShootingDirection::DOWN;
			}
			BlastLevel lv;
			float distance = 0;

			if (_magicGauge <= MAGIC_GAUGE_LOW) {
				lv = BlastLevel::Small;
				distance = 80;
				_pSprMagicCircle->at(MAGIC_CIRCLE_NUM).SetCellIndex(1);
				_pMagicBlast->Fire(_position, _direction, lv);
				_magicGauge = 0;
				
				// 攻撃判定解除　TODO 冗長.
				_bodyAtack.SetOnActive(false);
				_hurtBox[0].SetOnActive(true);
				auto c = _spritePlayer.pGetSprite()->color[0];
				_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 1, 1, 1, c.w };

				if (_viv.wRightMotorSpeed < 65535.0) {
					_viv.wRightMotorSpeed = 65535.0 / 4;
				}
				if (_viv.wLeftMotorSpeed < 65535.0) {
					_viv.wLeftMotorSpeed = 65535.0 / 4;
				}
				_vibrationTime = 10.0 / 60;
				XInputSetState(GamePadId_1P, &_viv);

			}
			else if (_magicGauge <= MAGIC_GAUGE_MEDIUM) {
				lv = BlastLevel::Medium;
				distance = 80;
				_pSprMagicCircle->at(MAGIC_CIRCLE_NUM).SetCellIndex(2);
				_pMagicBlast->Fire(_position, _direction, lv);
				_magicGauge = 0;

				// 攻撃判定解除　TODO 冗長.
				_bodyAtack.SetOnActive(false);
				_hurtBox[0].SetOnActive(true);
				auto c = _spritePlayer.pGetSprite()->color[0];
				_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 1, 1, 1, c.w };

				if (_viv.wRightMotorSpeed < 65535.0) {
					_viv.wRightMotorSpeed = 65535.0 / 2;
				}
				if (_viv.wRightMotorSpeed < 65535.0) {
					_viv.wRightMotorSpeed = 65535.0 / 2;
				}
				_vibrationTime = 20.0 / 60;
				XInputSetState(GamePadId_1P, &_viv);
			}
			else {
				lv = BlastLevel::Large;
				distance = 160;
				_pCamera->SetSecondTarget(_pMagicBlast);

				_pMagicBlast->SetFirePosition(_position, _direction, BlastLevel::Large);
				_pCamera->Zoom(1.8, 0.3);
				if (_direction == ShootingDirection::DOWN) {
					_pCamera->BlendLerp(0.45f, 0.3);
				}
				else if(_direction == ShootingDirection::UP) {
					_pCamera->BlendLerp(0.65f, 0.3);
				}
				else {
					_pCamera->BlendLerp(0.6f, 0.3);
				}
				_timeManager.SetNextMagnification(0);
				_stopTime = 0.6;
				_isStop = true;
				_pSprMagicCircle->at(MAGIC_CIRCLE_NUM).SetCellIndex(6);

				_hurtBox[0].SetOnActive(false);	// 発動するまで無敵.
			}
			_blastRecastTime = BLAST_RECAST_TIME;

			float rad = -0.7853981633974483f * static_cast<int>(_direction);
			Point2D pos;
			pos.x = -distance * sin(rad);
			pos.y = -distance * cosf(rad);

			_magicCircleObj[MAGIC_CIRCLE_NUM].SetLocalPosition(pos);
			_magicCircleObj[MAGIC_CIRCLE_NUM].SetOnActive(true);
			_magicCircleObj[MAGIC_CIRCLE_NUM].SetStartUp(true);
			_pSprMagicCircle->at(MAGIC_CIRCLE_NUM).rotation = rad;

			if (_direction != ShootingDirection::UP) {
				_velocity.y = -900;

				if (_direction != ShootingDirection::DOWN) {
					_recoilVelocity.x = 800;
				}
			}
			else {
				if (_velocity.y < 0) {
					_velocity.y = 900;
				}
				else {
					_velocity.y += 900;
				}
			}
			_isRightRecoil = _isLeft;
			_state = State::STID_MagicBlastLarge;
		}
	}
}

/// <summary>
/// マジックゲージ関連処理.
/// </summary>
/// <param name="delta">経過時間</param>
void Player::MagicGauge(double delta) {
	if (_magicGaugeBuffTime > 0) {
		_magicGaugeBuffTime -= delta;
	}
	else {
		_magicGauge -= MAGIC_GAUGE_DOWN * delta;
		if (_magicGauge < 0) {
			_magicGauge = 0;
		}
	}
}


/// <summary>
/// 地形チェックと座標更新.
/// </summary>
/// <param name="delta">経過時間</param>
void Player::TerrainCheck(double delta) {

	// 反動処理.
	Float2 tempVelocity = _velocity;

	if (_recoilVelocity.x > 0) {
		_recoilVelocity.x -= RESISTANCE * delta;
		if (_recoilVelocity.x <= 0) {
			_recoilVelocity.x = 0;
		}
	}
	if (_isRightRecoil) {
		if (!_isRightWall) {
			tempVelocity.x += _recoilVelocity.x;
		}
	}
	else {
		if (!_isLeftWall) {
			tempVelocity.x -= _recoilVelocity.x;
		}
	}

	// 加速度上限
	if (tempVelocity.x * tempVelocity.x > VELOCITY_MAX * VELOCITY_MAX) {
		tempVelocity.x = VELOCITY_MAX;
	}
	if (tempVelocity.y * tempVelocity.y > VELOCITY_MAX * VELOCITY_MAX) {
		tempVelocity.y = VELOCITY_MAX;
	}

	// 座標を更新.
	Point2D prePosition = _position;
	_position.x += tempVelocity.x * delta;
	_position.y += tempVelocity.y * delta;

	// 地形接触関連.
	assert(_pBlockList != nullptr && _pThroughBlockList != nullptr);
	if (_pBlockList != nullptr && _pThroughBlockList != nullptr) {
		_topGroundObj = nullptr;

		// 現状は強制で上判定.
		CeilingCheck(*_pBlockList);
		if (_isCeiling) {
			_velocity.y = 0;

			if (_ceilingObj.size() != 0) {
				const Block* buttomObj = _ceilingObj[0];	// 最初の要素で初期化.
				float buttom = buttomObj->GetWorldPosition().y + buttomObj->GetCollision().p.y + buttomObj->GetCollision().hl.y;
				for (auto itr = _ceilingObj.begin() + 1; itr != _ceilingObj.end(); ++itr) {
					AABB2DObj b = (*itr)->GetCollision();
					if (buttom < b.GetWorldPosition().y + b.hl.y) {
						buttom = b.GetWorldPosition().y + b.hl.y;
						buttomObj = *itr;
					}
				}
				_position.y = buttom - (_ceilingCheck.p.y - _ceilingCheck.hl.y);	// ヒットしていた地形の中で最も低い物に座標を合わせる.
			}
		}

		// 一旦地形チェック.
		GroundCheck(*_pBlockList, *_pThroughBlockList);
		RightWallCheck(*_pBlockList);
		LeftWallCheck(*_pBlockList);

		// TODO このへん色々無駄が多いし二分探索的にやった方が良いけど改善は後回し.
		if (tempVelocity.y != 0 && tempVelocity.x != 0 && _recoilVelocity.x != 0) {
			Float2 v = tempVelocity * delta / BLOCK_HIT_CHECK_NUM;	// 今回のフレームの加速度（移動量）をチェック回数等分.
			for (int i = 0; (_isGround && (_isRightWall || _isLeftWall)) && i < BLOCK_HIT_CHECK_NUM - 1; ++i) {	// 最低でも1 / BLOCK_HIT_CHECK_NUM フレーム分は動かす.
				_position = _position - v;	// 1 / BLOCK_HIT_CHECK_NUMフレーム分巻き戻す.
				// 再度地形とヒットチェックを繰り返す.
				GroundCheck(*_pBlockList, *_pThroughBlockList);
				if (_isRightWall) {
					RightWallCheck(*_pBlockList);
				}
				if (_isLeftWall) {
					LeftWallCheck(*_pBlockList);
				}
			}


			// 片方だけ当たっていた場合、当たっていなかった方は1フレーム分の移動に戻す.
			if (_isGround && !(_isRightWall || _isLeftWall)) {
				_position.y = prePosition.y + tempVelocity.y * delta;
			}


			if ((_isRightWall || _isLeftWall) && !_isGround) {
				_position.x = prePosition.x + tempVelocity.x * delta;
			}
		}

		GroundCheck(*_pBlockList, *_pThroughBlockList);

		if (!_isUpRecoil && _velocity.y < ASCEND_INTERRUPTION_VELOCITY && !(_gamePad.buttons & GamePad::A || _gamePad.buttons & GamePad::B)) {
			_velocity.y = ASCEND_INTERRUPTION_VELOCITY;
		}


		_velocity.y += GRAVITY * delta;	// 落下.
		if (_isUpRecoil && _velocity.y >= 0) {
			_isUpRecoil = false;
		}

		if (!_isGround) {	// 空中にいる時.

			if (_coyoteTimeTimer.Update(delta)) {
				if (_state != State::STID_Damage) {
					// コヨーテタイムを過ぎたらジャンプ不可に.
					_state = State::STID_Jump;
					_canJump = false;
				}
			}

			_jumpBuffTimer.Update(delta);		// ジャンプ予約中.

		}
		else if (_velocity.y >= 0.0f) {	// 下降中でなければ着地しない.
			if (_state == State::STID_Jump) {
				_magicCircleOffsetObj.SetLocalPosition({ 0, 0 });
			}
			else {
				_magicCircleOffsetObj.SetLocalPosition({ 0, MAGIC_CIRCLE_OFFSET_Y });
			}

			_canJump = true;
			_coyoteTimeTimer.Reset();
			_velocity.y = 0.0f;
			if (_groundHitObj.size() != 0) {
				_topGroundObj = _groundHitObj[0];	// 最初の要素で初期化.
				float top = _topGroundObj->GetWorldPosition().y + _topGroundObj->GetCollision().p.y - _topGroundObj->GetCollision().hl.y;
				for (auto itr = _groundHitObj.begin() + 1; itr != _groundHitObj.end(); ++itr) {
					AABB2D b = (*itr)->GetCollision();
					if (top > (*itr)->GetWorldPosition().y + b.p.y - b.hl.y) {
						top = (*itr)->GetWorldPosition().y + b.p.y - b.hl.y;
						_topGroundObj = *itr;	// GroundCheckの際に_groundHitObjに先にすり抜け不可ブロックを入れているため、すり抜け不可ブロックに接触している場合必ずそちらが入っている.
					}
				}
				_position.y = top - (_groundCheck.p.y + _groundCheck.hl.y) + 1;	// ヒットしていた地形の中で最も高い物に座標を合わせる.
			}
		}

		RightWallCheck(*_pBlockList);
		if (_isRightWall) {
			if (_rightWallObj.size() != 0) {
				float left = _rightWallObj[0]->GetWorldPosition().x + _rightWallObj[0]->GetCollision().p.x - _rightWallObj[0]->GetCollision().hl.x;	// 最初の値で初期化.
				for (auto& e : _rightWallObj) {							// ヒットしていた地形の中で最も左側の物に座標を合わせる.
					AABB2D b = e->GetCollision();
					if (left > e->GetWorldPosition().x + b.p.x - b.hl.x) {

						left = e->GetWorldPosition().x + b.p.x - b.hl.x;
					}
				}
				_position.x = left - (_rightWallCheck.p.x + _rightWallCheck.hl.x) + 1;
			}
		}


		LeftWallCheck(*_pBlockList);
		if (_isLeftWall) {
			if (_leftWallObj.size() != 0) {
				float right = _leftWallObj[0]->GetWorldPosition().x + _leftWallObj[0]->GetCollision().p.x + _leftWallObj[0]->GetCollision().hl.x;	// 最初の値で初期化.
				for (auto& e : _leftWallObj) {							// ヒットしていた地形の中で最も右側の物に座標を合わせる.
					AABB2D b = e->GetCollision();
					if (right < e->GetWorldPosition().x + b.p.x + b.hl.x) {
						right = e->GetWorldPosition().x + b.p.x + b.hl.x;
					}
					_position.x = right - (_leftWallCheck.p.x - _leftWallCheck.hl.x) - 1;
				}
			}
		}

	} // _blockList != nullptr;

	// 画面端
	assert(_pCamera != nullptr);
	if (_pCamera != nullptr) {
		auto topLeft = _pCamera->GetTopLeft();
		auto bottomRight = _pCamera->GetBottomRight();
		if (_position.x - _leftWallCheck.p.x > bottomRight.x) {
			_position.x = bottomRight.x + _leftWallCheck.p.x;
		}
		if (_position.x - _rightWallCheck.p.x < topLeft.x) {
			_position.x = topLeft.x + _rightWallCheck.p.x;
		}

		if (_position.y - 96 > bottomRight.y) {	// 仮に、下に落下したら上から復帰
			_position.y = topLeft.y - 96;
		}
	}

	// ブラスト反動攻撃解除.
	if (_bodyAtack.GetOnActive() && _isGround && _recoilVelocity.x <= 0) {
		_bodyAtack.SetOnActive(false);
		_hurtBox[0].SetOnActive(true);
		auto c = _spritePlayer.pGetSprite()->color[0];
		_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 1, 1, 1, c.w };
	}
}


bool Player::BodyAtackCheck() {
	if (_pEnemyList == nullptr) {
		return false;
	}
	bool ret = false;
	for (auto& e : *_pEnemyList) {
		if (e->GetOnActive()) {
			if (AABB2DObj::RectOnRect(_bodyAtack, *e->GetHurtBox().data())) {
				ret = e->Damage(10, this);	// 仮のダメージ.
				ScoreManager::GetIncetance().ComboGaugeReset();
				AddMagicGauge(1);
			}
		}
	}

	return ret;
}