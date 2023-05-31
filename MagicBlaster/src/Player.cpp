#include "Player.h"
#include "BulletQueue.h"
#include "MagicBlast.h"
#include "Engine/Graphics.h"
#include "ScoreManager.h"

#include <cmath>
#include <string>
#include <assert.h>
// TODO State�̊Ǘ�������ύX���āA���ꂼ��̃t���O�������Ō�ɔ��肵�ăA�j���[�V������ς��铙�̏���������`��.

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
	_groundCheck(this, { 0, 96 - 8 + 1 }, { 13, 8 }),	// Y���W�̓L�������S - ���蕝 + ����1�h�b�g.
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
	, _bodyAtack(this, Point2D(0, 0), Float2(80, 80))	// ���̍U������.
	, _isUpRecoil(false)
{
	_magicCircleOffsetObj.SetParent(this);
	_hurtBox.emplace_back(this, Point2D(0, 0), Float2(15, 32));
	_bodyAtack.SetOnActive(false);
}

// �A�j���[�V�����V�[�N�G���X�@�{���͊O���f�[�^���烍�[�h����.
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
	_pSpriteResource->pGetSpriteList()->reserve(2);	// �I�[���ƃL�����łƂ肠�������.

	// �Z�����X�g�̍쐬.
	if (!_pSpriteResource->CreateCellList(15, 15, 48, 48)) {
		return false;
	};

	// �A�j���[�V�������X�g�̐ݒ�.
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

	// �摜�����[�h.
	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/player_00.png")) {
		return false;
	}

	return true;
}

bool Player::Init() {

	// �v���C���[�X�v���C�g�̊��蓖��.
	_pSpriteResource->Register(_spriteAura);
	_pSpriteResource->Register(_spritePlayer);

	_spritePlayer.pGetSprite()->scale = { 4.0f, 4.0f };

	_spriteAura.pGetSprite()->onActive = true;	// �ЂƂ܂�����̓I�[���͔�\����.

	// ���@�w�I�u�W�F�N�g�̏�����.
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

	// ������.
	_isDamageable = true;

	// ��`�X�v���C�g�֘A.
	assert(_pSquareSprite != nullptr);
	if (_pSquareSprite != nullptr) {
		// �}�W�b�N�Q�[�W�̃X�v���C�g��p��.
		auto& s = _pSquareSprite->emplace_back(Sprite(nullptr, { 20, 20, 0 }));;
		s.cameraRelative = false;
		s.rectOffset.bottom = 20;
		s.rectOffset.right = 299;
		s.addColor = { 0.2f, 0.2f, 0.2f, 1.0f };

		_pMagicGaugeSprite = &_pSquareSprite->emplace_back(Sprite(nullptr, { 20, 20, 0 }));;
		_pMagicGaugeSprite->cameraRelative = false;
		_pMagicGaugeSprite->rectOffset.bottom = 20;
	}

	// ��������̐F�w��.
	_hurtBox[0].color = { 1, 0, 0, 0.7f };
	_bodyAtack.color = { 1, 0.3f, 0, 0.5f };
	_rightWallCheck.color = { 1, 1, 0, 0.7f };
	_leftWallCheck.color = { 1, 1, 0, 0.7f };
	_ceilingCheck.color = { 1, 0, 1, 0.7f };
	_groundCheck.color = { 1, 0, 1, 0.7f };

	return true;
}

/*
* MagicCircle�I�u�W�F�N�g�ɃX�v���C�g���Z�b�g���܂�.
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
* �v���C���[�̍X�V.
*/
void Player::Update(double delta) {
	/*
	wchar_t str[256];	// �f�o�b�O�o�͗p.
	swprintf_s(str, L"%1f\n", _velocity.y);
	OutputDebugString(str);
	*/
	auto ucDelta = _timeManager.GetUncorrectedDelta();	// �␳�Odelta���擾.
	_stopTime -= ucDelta;

	auto pSprite = _spritePlayer.pGetSprite();
	assert(pSprite != nullptr);

	// TODO �e��^�C�}�[�������ł܂Ƃ߂ăA�b�v�f�[�g����悤�ɕύX.
	_damageTimer.Update(delta);
	_invincibleTimer.Update(delta);
	_throughBlockUnCheckTimer.Update(delta);
	_throughInputTimer.Update(delta);

	if (_damageTimer.IsReachedFrame()) {
		_state = State::STID_Idle;	// �X�e�[�g��������.
		_invincibleTimer.Reset();
	}

	// ���G�֘A.
	if (!_invincibleTimer.IsReached()) {	// ���G���Ԓ�.
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
	else if (_invincibleTimer.IsReachedFrame()) {	// ���G���ԏI���t���[��.
		_isDamageable = true;

		assert(pSprite != nullptr);
		if (pSprite != nullptr) {
			auto a = pSprite->addColor.w;
			a = 0;
			pSprite->addColor.w = a;
		}
	}

	// �_���[�W���Ԓ�~�̉����@TODO ���Ԓ�~����ʂ���.
	/*
	if (_stopTime <= 0 && _isStop && _state == State::STID_Damage) {
		_pTimeManager->SetNextMagnification(1);
	}
	*/

	// �U�����Ԍv�� TODO �U���֘A�͐����Ɏ���������.
	if (_vibrationTime >= 0) {
		_vibrationTime -= ucDelta;
	}
	else {	// 0�ȉ��ŐU��������.
		_viv = { 0, 0 };
		XInputSetState(GamePadId_1P, &_viv);
	}

	_magicCircleObj[MAGIC_CIRCLE_NUM].SetStartUp(false);	// �u���X�g�p���@�w��OFF�ɂ�������.
	// �u���X�g�̃��L���X�g���Ԓ�.
	if (_blastRecastTime > 0) {
		if (!_isStop) {
			_blastRecastTime -= ucDelta;
		}
		_magicCircleObj[MAGIC_CIRCLE_NUM].SetStartUp(true);
		_magicCircleObj[MAGIC_CIRCLE_NUM].SetOnActive(true);
	}

	if (_state != State::STID_Damage) {	// �_���[�W��ԂłȂ�.

		// ���Ԓ�~�������Ƀu���X�g�����@TODO ����͎��Ԓ�~�������Ɍ���������Ă�̂ŋ�ʂ���.
		if (_stopTime <= 0 && _isStop) {
			// �U������.
			_viv.wLeftMotorSpeed = _viv.wRightMotorSpeed = 65535.0;
			XInputSetState(GamePadId_1P, &_viv);
			_vibrationTime = 1;

			// ����.
			_pMagicBlast->Fire(_position, _direction, BlastLevel::Large);
			_magicGauge = 0;
			//_hurtBox[0].SetOnActive(true);		// ���G�����͂܂����Ȃ�.
			_bodyAtack.SetOnActive(true);			// �U�������true��.
			auto c = _spritePlayer.pGetSprite()->color[0];
			_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 2, 0.5, 0.5, c.w };

			// ���Ԓ�~����
			_isStop = false;
			_timeManager.SetNextMagnification(1);

			// ���X�ɃY�[������.
			_pCamera->SetSecondTarget(_pMagicBlast);
			_pCamera->Zoom(0.8, 0.5);
			_pCamera->BlendLerp(0, 0.5);
		}
		if (delta == 0) {	//�@���Ԓ�~�� TODO �ǂ̎��Ԓ�~����ʂ���.
			SpriteUpdate(delta);
			_magicCircleObj[MAGIC_CIRCLE_NUM].Update(ucDelta);	// �␳�O�f���^�ōX�V.
			return;
		}

		if (_bodyAtack.GetOnActive()) {
			BodyAtackCheck();
		}

		_state = State::STID_Idle;	// �X�e�[�g���������@���̐�ǂ̏�Ԃɂ��Ȃ�Ȃ�������Idle�ɂȂ�	����܂�ǂ��Ȃ������ȋC������̂ŉ��P������.

		Move();	// ���E�ړ�.

		// TODO ���Ȃ薳��������ǈꎞ�I�ɁA�u���X�g���������̔����ł����󒆃��[�V��������悤�Ɂ@�u���X�g�����ɓ���.
		if (_velocity.y < 0) {
			_state = State::STID_Jump;
		}

		Direction();	// �ˌ������ݒ�.

		/*	// �f�o�b�O�p�ړ�.
		if (gamepad.buttons & GamePad::DPAD_UP) {
			_position.y -= MOVE_SPEED * delta;
		}
		else if (gamepad.buttons & GamePad::DPAD_DOWN) {
			if (!_isGround) {
				_position.y += MOVE_SPEED * delta;
			}
		}
		*/

		Jump();	// �W�����v�n����.

	}

	TerrainCheck(delta);	// �n�`�`�F�b�N.

	// State�ɉ��������@�w�̊�_��e�픻��̈ʒu����.
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

	Bullet(delta);	// �o���b�g����.

	Blast();	// �u���X�g����.

	_prevButtons = _gamePad.buttons;	// ����̓��͂�ێ�.

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

		// �X�v���C�g���W�̍X�V.
		pSprite->pos.x = _position.x;
		pSprite->pos.y = _position.y;

		// �A�j���[�V�����̍X�V.
		pSprite->animeController.Update(_delta);

		// �A�j���[�V�����؂�ւ�.
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
		if (_magicGauge <= MAGIC_GAUGE_LOW) {	// TODO �}�W�b�N�i���o�[�Ȃ̂Œ萔������悤��.
			_pMagicGaugeSprite->addColor = { 0,1,0,1 };

			// �I�[��������.
			pSpriteAura->onActive = false;
		}
		else if (_magicGauge <= MAGIC_GAUGE_MEDIUM) {
			_pMagicGaugeSprite->addColor = { 1,1,0,1 };

			// ���I�[��.
			pSpriteAura->pos.y -= 8;
			pSpriteAura->scale.x *= 1.1f;
			pSpriteAura->scale.y *= 1.1f;
			pSpriteAura->addColor.x = 1;
			pSpriteAura->addColor.y = 1;
			pSpriteAura->addColor.z = 0;
		}
		else {
			_pMagicGaugeSprite->addColor = { 1,0,0,1 };

			// �ԃI�[��.
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
	_velocity.y = -1000;	// �_���[�W�̐������.
	_isUpRecoil = true;
	_recoilVelocity.x = 0;	// ����������.
	_damageTimer.Reset();
	ScoreManager::GetIncetance().ComboBreak();

	//_stopTime = 0.6;	// ��U���Ԓ�~���R�����g�A�E�g.
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
/// ���̓Q�[�W�����Z���܂�.
/// </summary>
/// <param name="num">���Z����l</param>
void Player::AddMagicGauge(int num) {
	_magicGaugeBuffTime = MAGIC_GAUGE_BUFF_TIME;
	_magicGauge += num;
	if (_magicGauge > MAGIC_GAUGE_MAX) {
		_magicGauge = MAGIC_GAUGE_MAX;
	}
}

// �n��Ƃ̐ڐG������s���܂�.
void Player::GroundCheck(const std::vector<Block>& block, const std::vector<Block>& throughBlock) {
	_isGround = false;
	_groundHitObj.clear();

	if (_velocity.y < 0) {	// �W�����v�㏸���͔��肵�Ȃ�.
		return;
	}

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_groundCheck, e.GetCollision())) {
			_isGround = true;
			_groundHitObj.push_back(&e);
		}
	}

	if (_throughBlockUnCheckTimer.IsReached()) {	// �X���[�u���b�N���蔲�����łȂ�
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

	if (_velocity.y >= 0) {	// �㏸���ȊO���肵�Ȃ�.
		return;
	}

	for (auto& e : block) {
		if (AABB2DObj::RectOnRect(_ceilingCheck, e.GetCollision())) {
			_isCeiling = true;
			_ceilingObj.push_back(&e);
		}
	}
}

// �O���i�E���j�Ƃ̐ڐG������s���܂�.
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

// ����i�����j�Ƃ̐ڐG������s���܂�.
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
/// �W�����v�֘A����.
/// </summary>
void Player::Jump() {
	if (_gamePad.buttonDown & GamePad::A || _gamePad.buttonDown & GamePad::B) {
		_jumpBuffTimer.Reset();	// �W�����v��\��.
	}

	// �W�����v���\�񂳂�Ă���A���W�����v�\�Ȃ�W�����v.
	if (_canJump && !_jumpBuffTimer.IsReached()) {
		_jumpBuffTimer.SetTime(JUMP_BUFF_TIME);
		_velocity.y = -JUMP_FORCE;
		_canJump = false;
		_state = State::STID_Jump;

		// �̓����蔻��̉����@TODO �璷�Ȃ̂Ō�ŏC��.
		if (_bodyAtack.GetOnActive()) {
			_bodyAtack.SetOnActive(false);
			_hurtBox[0].SetOnActive(true);
			auto c = _spritePlayer.pGetSprite()->color[0];
			_spritePlayer.pGetSprite()->color[0] = _spritePlayer.pGetSprite()->color[1] = { 1, 1, 1, c.w };
		}
	}
}

/// <summary>
/// ���E�ړ����s���܂�.
/// </summary>
void Player::Move() {
	_velocity.x = 0.0f;			// �������x��������.

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

	// �X���[�u���b�N���蔲��.
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
/// �ˌ�������ݒ肵�܂�.
/// </summary>
void Player::Direction() {
	if (_gamePad.buttons & GamePad::DPAD_UP) {			// ��n.
		if (_gamePad.buttons & GamePad::DPAD_RIGHT) {
			_direction = ShootingDirection::RIGHT_UP;		// �E��.
		}
		else if (_gamePad.buttons & GamePad::DPAD_LEFT) {
			_direction = ShootingDirection::LEFT_UP;		// ����.
		}
		else {
			_direction = ShootingDirection::UP;				// ��.
		}
	}
	else if (_gamePad.buttons & GamePad::DPAD_DOWN) {	// ���n.
		if (_gamePad.buttons & GamePad::DPAD_RIGHT) {
			_direction = ShootingDirection::RIGHT_DOWN;		// �E��.
		}
		else if (_gamePad.buttons & GamePad::DPAD_LEFT) {
			_direction = ShootingDirection::LEFT_DOWN;		// ����.
		}
		else if (_gamePad.buttons & GamePad::R || !_isGround) {
			_direction = ShootingDirection::DOWN;			// ��.
		}
		else {

			_direction = ShootingDirection::DOWN;			// ��.
			/*	// ���Ⴊ�ݔp�~.
			_state = State::STID_Crouching;				// ���Ⴊ�݌n.
			
			if (_isLeft) {
				_direction = ShootingDirection::LEFT;		// ���Ⴊ�ݍ�.
			}
			else {
				_direction = ShootingDirection::RIGHT;		// ���Ⴊ�݉E.
			}
			*/
		}
	}
	else {												// ���E�n.
		if (_isLeft) {
			_direction = ShootingDirection::LEFT;			// ��.
		}
		else {
			_direction = ShootingDirection::RIGHT;			// �E.
		}
	}
}

/// <summary>
/// �o���b�g�֘A����.
/// </summary>
/// <param name="delta">�o�ߎ���</param>
void Player::Bullet(double delta) {

	// �o���b�g����.
	_bulletIntervalTimer.Update(delta);	// �ˌ��Ԋu.

	// �o���b�g�p���@�w�����OFF�ɂ�������.
	for (int i = 0; i < MAGIC_CIRCLE_NUM; ++i) {
		_magicCircleObj[i].SetStartUp(false);
	}

	assert(_pBulletQueue != nullptr);
	if (_pBulletQueue != nullptr && _state != State::STID_Damage) {	// _pBulletQueue��null�łȂ��A���_���[�W��ԂłȂ�.

		_bulletBuffTimer.Update(delta);	// �ˌ��p������.

		if (_gamePad.buttonDown & GamePad::X) {
			_bulletBuffTimer.Reset();
		}

		if ((_gamePad.buttons & GamePad::X || !_bulletBuffTimer.IsReached()) && _blastRecastTime <= 0) {	// �ˌ��{�^����������Ă���A�܂��͎ˌ��p�����Ԓ��@���u���X�g�d�����łȂ�.

			if (_state == State::STID_Idle) {	// ������Ԃ̎�.
				_state = State::STID_Shot;		// �ˌ���Ԃ�.
			}
			_magicCircleObj[static_cast<int>(_direction)].SetOnActive(true);
			_magicCircleObj[static_cast<int>(_direction)].SetStartUp(true);

			if (_bulletIntervalTimer.IsReached()) {	// �o���b�g�ˌ��Ԋu���o�߂��Ă���.
				auto pos = _magicCircleObj[static_cast<int>(_direction)].GetWorldPosition();
				
				_bulletIntervalTimer.Reset();	// �ˌ��Ԋu��������.

				// �ꔭ�����ɐ������W��O�ɃY�����A���h���𐮂���.
				_isBulletOffset = !_isBulletOffset;	
				if (_isBulletOffset) {
					float rad = -0.7853981633974483f * static_cast<int>(_direction);

					pos.x += -16 * sinf(rad);
					pos.y += -16 * cosf(rad);
				}

				_pBulletQueue->Create(_direction, pos);

				/* �U������l�̃R���g���[���[���ƃN�h���Ȃ��Ă��܂��̂ŁA�o���b�g�̐U���͈�U����.
				if (_viv.wRightMotorSpeed < 65535.0) {
					_viv.wRightMotorSpeed = 768;	// �U���Ŏ�ݒ�.
					_vibrationTime = 1.0 / 60;
				}
				XInputSetState(GamePadId_1P, &_viv);
				*/
			}
		}
	}
}

/// <summary>
/// �u���X�g�֘A����.
/// </summary>
void Player::Blast() {
	assert(_pMagicBlast != nullptr);
	if (_pMagicBlast != nullptr && _state != State::STID_Damage) {
		if (_gamePad.buttonDown & GamePad::Y && _blastRecastTime <= 0) {
			_isUpRecoil = true;	// �u���X�g�����t���O.

			_canJump = false;	// �W�����v�s��.
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
				
				// �U����������@TODO �璷.
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

				// �U����������@TODO �璷.
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

				_hurtBox[0].SetOnActive(false);	// ��������܂Ŗ��G.
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
/// �}�W�b�N�Q�[�W�֘A����.
/// </summary>
/// <param name="delta">�o�ߎ���</param>
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
/// �n�`�`�F�b�N�ƍ��W�X�V.
/// </summary>
/// <param name="delta">�o�ߎ���</param>
void Player::TerrainCheck(double delta) {

	// ��������.
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

	// �����x���
	if (tempVelocity.x * tempVelocity.x > VELOCITY_MAX * VELOCITY_MAX) {
		tempVelocity.x = VELOCITY_MAX;
	}
	if (tempVelocity.y * tempVelocity.y > VELOCITY_MAX * VELOCITY_MAX) {
		tempVelocity.y = VELOCITY_MAX;
	}

	// ���W���X�V.
	Point2D prePosition = _position;
	_position.x += tempVelocity.x * delta;
	_position.y += tempVelocity.y * delta;

	// �n�`�ڐG�֘A.
	assert(_pBlockList != nullptr && _pThroughBlockList != nullptr);
	if (_pBlockList != nullptr && _pThroughBlockList != nullptr) {
		_topGroundObj = nullptr;

		// ����͋����ŏ㔻��.
		CeilingCheck(*_pBlockList);
		if (_isCeiling) {
			_velocity.y = 0;

			if (_ceilingObj.size() != 0) {
				const Block* buttomObj = _ceilingObj[0];	// �ŏ��̗v�f�ŏ�����.
				float buttom = buttomObj->GetWorldPosition().y + buttomObj->GetCollision().p.y + buttomObj->GetCollision().hl.y;
				for (auto itr = _ceilingObj.begin() + 1; itr != _ceilingObj.end(); ++itr) {
					AABB2DObj b = (*itr)->GetCollision();
					if (buttom < b.GetWorldPosition().y + b.hl.y) {
						buttom = b.GetWorldPosition().y + b.hl.y;
						buttomObj = *itr;
					}
				}
				_position.y = buttom - (_ceilingCheck.p.y - _ceilingCheck.hl.y);	// �q�b�g���Ă����n�`�̒��ōł��Ⴂ���ɍ��W�����킹��.
			}
		}

		// ��U�n�`�`�F�b�N.
		GroundCheck(*_pBlockList, *_pThroughBlockList);
		RightWallCheck(*_pBlockList);
		LeftWallCheck(*_pBlockList);

		// TODO ���̂ւ�F�X���ʂ��������񕪒T���I�ɂ���������ǂ����ǉ��P�͌��.
		if (tempVelocity.y != 0 && tempVelocity.x != 0 && _recoilVelocity.x != 0) {
			Float2 v = tempVelocity * delta / BLOCK_HIT_CHECK_NUM;	// ����̃t���[���̉����x�i�ړ��ʁj���`�F�b�N�񐔓���.
			for (int i = 0; (_isGround && (_isRightWall || _isLeftWall)) && i < BLOCK_HIT_CHECK_NUM - 1; ++i) {	// �Œ�ł�1 / BLOCK_HIT_CHECK_NUM �t���[�����͓�����.
				_position = _position - v;	// 1 / BLOCK_HIT_CHECK_NUM�t���[���������߂�.
				// �ēx�n�`�ƃq�b�g�`�F�b�N���J��Ԃ�.
				GroundCheck(*_pBlockList, *_pThroughBlockList);
				if (_isRightWall) {
					RightWallCheck(*_pBlockList);
				}
				if (_isLeftWall) {
					LeftWallCheck(*_pBlockList);
				}
			}


			// �Е������������Ă����ꍇ�A�������Ă��Ȃ���������1�t���[�����̈ړ��ɖ߂�.
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


		_velocity.y += GRAVITY * delta;	// ����.
		if (_isUpRecoil && _velocity.y >= 0) {
			_isUpRecoil = false;
		}

		if (!_isGround) {	// �󒆂ɂ��鎞.

			if (_coyoteTimeTimer.Update(delta)) {
				if (_state != State::STID_Damage) {
					// �R���[�e�^�C�����߂�����W�����v�s��.
					_state = State::STID_Jump;
					_canJump = false;
				}
			}

			_jumpBuffTimer.Update(delta);		// �W�����v�\��.

		}
		else if (_velocity.y >= 0.0f) {	// ���~���łȂ���Β��n���Ȃ�.
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
				_topGroundObj = _groundHitObj[0];	// �ŏ��̗v�f�ŏ�����.
				float top = _topGroundObj->GetWorldPosition().y + _topGroundObj->GetCollision().p.y - _topGroundObj->GetCollision().hl.y;
				for (auto itr = _groundHitObj.begin() + 1; itr != _groundHitObj.end(); ++itr) {
					AABB2D b = (*itr)->GetCollision();
					if (top > (*itr)->GetWorldPosition().y + b.p.y - b.hl.y) {
						top = (*itr)->GetWorldPosition().y + b.p.y - b.hl.y;
						_topGroundObj = *itr;	// GroundCheck�̍ۂ�_groundHitObj�ɐ�ɂ��蔲���s�u���b�N�����Ă��邽�߁A���蔲���s�u���b�N�ɐڐG���Ă���ꍇ�K�������炪�����Ă���.
					}
				}
				_position.y = top - (_groundCheck.p.y + _groundCheck.hl.y) + 1;	// �q�b�g���Ă����n�`�̒��ōł��������ɍ��W�����킹��.
			}
		}

		RightWallCheck(*_pBlockList);
		if (_isRightWall) {
			if (_rightWallObj.size() != 0) {
				float left = _rightWallObj[0]->GetWorldPosition().x + _rightWallObj[0]->GetCollision().p.x - _rightWallObj[0]->GetCollision().hl.x;	// �ŏ��̒l�ŏ�����.
				for (auto& e : _rightWallObj) {							// �q�b�g���Ă����n�`�̒��ōł������̕��ɍ��W�����킹��.
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
				float right = _leftWallObj[0]->GetWorldPosition().x + _leftWallObj[0]->GetCollision().p.x + _leftWallObj[0]->GetCollision().hl.x;	// �ŏ��̒l�ŏ�����.
				for (auto& e : _leftWallObj) {							// �q�b�g���Ă����n�`�̒��ōł��E���̕��ɍ��W�����킹��.
					AABB2D b = e->GetCollision();
					if (right < e->GetWorldPosition().x + b.p.x + b.hl.x) {
						right = e->GetWorldPosition().x + b.p.x + b.hl.x;
					}
					_position.x = right - (_leftWallCheck.p.x - _leftWallCheck.hl.x) - 1;
				}
			}
		}

	} // _blockList != nullptr;

	// ��ʒ[
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

		if (_position.y - 96 > bottomRight.y) {	// ���ɁA���ɗ���������ォ�畜�A
			_position.y = topLeft.y - 96;
		}
	}

	// �u���X�g�����U������.
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
				ret = e->Damage(10, this);	// ���̃_���[�W.
				ScoreManager::GetIncetance().ComboGaugeReset();
				AddMagicGauge(1);
			}
		}
	}

	return ret;
}