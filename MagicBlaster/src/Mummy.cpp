#include "Mummy.h"
#include "Engine/Texture.h"
#include "Player.h"
#include "Block.h"
#include "Engine/Graphics.h"
#include "ScoreManager.h"

// �ÓI�ϐ��Q.
// TODO�@�o���A�j���[�V������p��.
const AnimationSequence Mummy::_spawn = {};
const AnimationSequence Mummy::_walk = { { {10, 8.0 / 60}, {11, 8.0 / 60}, {12, 8.0 / 60}, {13, 8.0 / 60}, {14, 8.0 / 60}, {15, 8.0 / 60} } };
const AnimationSequence Mummy::_death = { {{4, 15.0 / 60}, {5, 3.0 / 60}, {6, 3.0 / 60}, {7, 3.0 / 60}, {8, 3.0 / 60}, {9, 3.0 / 60}, }, false };

SpriteResource* Mummy::_pSpriteResource;

/*
* static
* �}�~�[�p�̃Z�����X�g�̍쐬�A�X�v���C�g�̈�̊m�ۂƏ������A�摜�̃��[�h�A�A�j���[�V�������X�g�̊��蓖�Ă��s���܂�.
*/
bool Mummy::Load() {
	// �A�j���[�V�������X�g�̊��蓖��.
	AnimationList tempAnimList(3);

	tempAnimList.at(Spawn) = _spawn;
	tempAnimList.at(Walk) = _walk;
	tempAnimList.at(Death) = _death;
	_pSpriteResource->SetAnimationList(tempAnimList);

	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/ene_06.png")) {
		return false;
	}

	// �X�v���C�g�̗̈���m��.
	_pSpriteResource->pGetSpriteList()->reserve(32);	// ����32.

	// �Z�����X�g�̍쐬.
	if (_pSpriteResource->CreateCellList(10, 3, 48, 72) != 0) {
		return false;
	};

	return true;
}

/*
* �ʂ̃I�u�W�F�N�g�̏��������s���܂�.
* 
*/
bool Mummy::Init() {
	_hp = HP_INIT;
	_pSpriteResource->Register(_spriteHandle);	// �X�v���C�g���n���h���ɓo�^.

	auto pSprite = _spriteHandle.pGetSprite();		// �X�v���C�g�̃|�C���^���擾.
	assert(pSprite != nullptr);
	if (pSprite == nullptr) {
		return false;
	}

	pSprite->scale = { 4.0f, 4.0f };
	pSprite->animeController.SetSeqIndex(Walk);
	pSprite->animeController.Reset();
	pSprite->animeController.Update((rand() % 48) / 60.0);	// ���ɓK���ɃA�j���[�V�������Y�����@�ŏI�I�ɂ͕s�v.
	return true;
}

void Mummy::Update(double delta) {
	if (!_onActive) {
		return;
	}

	// �z���g�͂悭�Ȃ��`�����ǁA�ЂƂ܂��̎��S����.
	if (_hp < 0) {
		_velocity.y -= GRAVITY * delta;
		_deathTime -= delta;
		if (_deathTime <= 0) {
			_onActive = false;
		}

	}
	else {

		if (_isLeft) {
			_velocity.x = -MOVE_SPEED;
		}
		else {
			_velocity.x = MOVE_SPEED;
		}
	}
	_position.y += _velocity.y * delta;
	_position.x += _velocity.x * delta;

	if (_hp >= 0) {
		// �O�����`�F�b�N���A�ǂ������甽�].
		ForwardWallCheck();
		if (_isForwardWall) {
			_isLeft = !_isLeft;
		}

		// �ꏏ�ɉ�ʒ[�`�F�b�N.
		if (_position.x + _hurtBox[0].hl.x > clientWidth * 2) {
			_isLeft = true;
		}
		if (_position.x - _hurtBox[0].hl.x < 0) {
			_isLeft = false;
		}

		if (_isLeft) {
			_forwardWallCheck.p.x = -32;
		}
		else {
			_forwardWallCheck.p.x = 32;
		}

		// �v���C���[�Ƃ̃q�b�g�`�F�b�N.
		HitCheck();
	}

	SpriteUpdate(delta);

	_isDamage = false;
}

void Mummy::SpriteUpdate(double delta) {
	auto pSprite = _spriteHandle.pGetSprite();		// �X�v���C�g�̃|�C���^���擾.

	if (pSprite == nullptr) {
		return;
	}
	if (!_onActive) {	// �I�u�W�F�N�g���A�N�e�B�u�łȂ��Ȃ�����A�X�v���C�g����A�N�e�B�u�ɂ���.
		pSprite->onActive = false;
		return;
	}

	// ���E��K�p.
	pSprite->flipHorizontal = !_isLeft;

	// �X�v���C�g���W�̍X�V.
	pSprite->pos.x = _position.x;
	pSprite->pos.y = _position.y;

	// ���Z�F�␳���쐬�@��_���[�W���ɔ����t���b�V��.
	DirectX::XMFLOAT4 tmpColor = {};
	if (_isDamage) {
		tmpColor = { 1,1,1, 0 };
	}
	pSprite->addColor = tmpColor;

	// �A�j���[�V�����̍X�V.
	pSprite->animeController.Update(delta);

}

/*
* ��_���[�W����.
* 
* @param damage		�_���[�W��.
* @param pHitObj	�ڐG�I�u�W�F�N�g�̃|�C���^.
* 
* @return	�q�b�g�L��.
*/
bool Mummy::Damage(int damage, DrawObj* pHitObj) {
	if (!_isDamageable) {
		return false;
	}

	// HP�����炷.
	_hp -= damage;
	if (_hp < 0) {	// HP��0�����������.
		// ���Ɏ��S���̃_���[�W�ʂɉ����Đ�����є{����ς���@�o���b�g���u���X�g���������������悤�ɂ���H.
		float magnification = 1;
		if (damage > 1) {
			magnification = 1.4;
		}
		_isDamageable = false;
		_hurtBox[0].SetOnActive(false);	// �R���W������OFF��.
		_deathTime = 0.5;
		_velocity.y = (-800 + (-800 * (rand() % 16) / 16)) * magnification;
		_isLeft = pHitObj->GetWorldPosition().x < _position.x;	// �ڐG�����I�u�W�F�N�g�̍��W�̋t���ɔ��
		if (_isLeft) {
			_velocity.x = ((900.0 / 8) * ((rand() % 8))) * magnification;
		}
		else {
			_velocity.x = ((-900.0 / 8) * ((rand() % 8))) * magnification;
		}

		_spriteHandle.pGetSprite()->animeController.SetSeqIndex(Death);

		ScoreManager::GetIncetance().AddScore(_score);
	}
	_isDamage = true;
	return true;
}

// �v���C���[�Ƃ̐ڐG������s���܂�.
bool Mummy::HitCheck() {
	assert(_pPlayer != nullptr);
	if (_pPlayer == nullptr) {
		return false;
	}
	bool ret = false;
	if (AABB2DObj::RectOnRect(_hurtBox[0], *_pPlayer->GetHurtBox().data())) {
		ret = _pPlayer->Damage(30, this);	// ����30�_���[�W�����.
	}

	return ret;
}

// �O���Ƃ̐ڐG������s���܂�.
void Mummy::ForwardWallCheck() {
	_isForwardWall = false;

	assert(_pBlockList != nullptr);
	if (_pBlockList != nullptr) {
		for (auto& e : *_pBlockList) {
			if (AABB2DObj::RectOnRect(_forwardWallCheck, e.GetCollision())) {
				_isForwardWall = true;
				break;
			}
		}
	}
}
