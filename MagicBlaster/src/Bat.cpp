#include "Bat.h"
#include "Engine/Texture.h"
#include "Player.h"

#include <time.h>

// �A�j���[�V����
const AnimationSequence Bat::_stay = { { {8, 8.0 / 60} } };
const AnimationSequence Bat::_fly = { { {0, 8.0 / 60}, {1, 8.0 / 60}, {2, 8.0 / 60}, {3, 8.0 / 60}, {4, 8.0 / 60}, {5, 8.0 / 60} } };
const AnimationSequence Bat::_death = { {{4, 15.0 / 60}, {5, 3.0 / 60}, {6, 3.0 / 60}, {7, 3.0 / 60}, {8, 3.0 / 60}, {9, 3.0 / 60}, }, false };

int Bat::_batCount = 0;

std::vector<Sprite> Bat::_spriteList;
AnimationList Bat::_animationList;
CellList Bat::_cellList;
Texture Bat::_texture;

std::vector<Sprite>* Bat::GetSpriteListPtr() { return &_spriteList; }
const CellList* Bat::GetCellListPtr() { return &_cellList; }
Texture* Bat::GetTexturePtr() { return &_texture; }

bool Bat::Load() {
	Sprite spr(&_animationList, DirectX::XMFLOAT3(0, 0, 0), 0.0f, { 4.0f, 4.0f });
	spr.isUsed = false;
	spr.onActive = false;
	_spriteList.resize(32, spr);

	_cellList.list.resize(24);
	if (CreateCellList(24, 6, 4, 32, 32, _cellList) != 0) {
		return false;
	};

	// �A�j���[�V�������X�g�쐬
	_animationList.resize(3);

	_animationList[Stay] = _stay;
	_animationList[Fly] = _fly;
	_animationList[Death] = _death;

	// TODO �e�N�X�`�����[�_�[���P��͂����Ń��[�h���s���H.

	return true;
}

bool Bat::Init() {
	_hp = HP_INIT;
	bool hasSprite = false;
	int num = 0;
	for (auto& e : _spriteList) {	// �X�v���C�g���X�g�𑖍����A�󂫂�T��.
		if (!e.isUsed) {				// �������炻���ɓ����.
			_pSprite = &e;
			e.isUsed = true;
			e.onActive = true;
			hasSprite = true;
			break;
		}
		num++;
	}

	if (_pSprite == nullptr) {
		return false;
	}
	_pSprite->animeController.SetSeqIndex(Stay);
	_pSprite->animeController.Reset();
	_pSprite->animeController.Update((rand() % 48) / 60.0);
	return true;
}

void Bat::Update(double delta) {
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
		// �ړ��֌W
		/*if (_isLeft) {
			_velocity.x = -MOVE_SPEED;
		}
		else {
			_velocity.x = MOVE_SPEED;
		}
		*/
	}
	
	Attack(delta);
	if (!_attack) {
		PlayerCheck();
	}
	
	/*// ��ʒ[
	if (_pCamera != nullptr) {
		auto cameraPos = _pCamera->GetWorldPosition();
		auto halfWidth = clientWidth / 2;
		auto halfHeight = clientHeight / 2;
		if (_position.x - _hurtBox[0].hl.x > cameraPos.x + halfWidth + 128) {
			_onActive = false;
		}
		if (_position.x + _hurtBox[0].hl.x < cameraPos.x - halfWidth - 128) {
			_onActive = false;
		}
		if (_position.y - 96 > cameraPos.y + halfHeight) {	// ���ɁA���ɗ���������ォ�畜�A
			_position.y = -halfHeight - 96;
		}
	}
	*/

	SpriteUpdate(delta);

	_isDamage = false;
}

void Bat::SpriteUpdate(double delta) {

	if (_pSprite == nullptr) {
		return;
	}
	if (!_onActive) {
		_pSprite->onActive = false;
		_pSprite->isUsed = false;
		return;
	}

	_pSprite->flipHorizontal = !_isLeft;

	// �X�v���C�g���W�̍X�V.
	_pSprite->pos.x = _position.x;
	_pSprite->pos.y = _position.y;

	DirectX::XMFLOAT4 tmpColor = {};
	if (_isDamage) {
		tmpColor = { 1,1,1, 0 };
	}
	_pSprite->addColor = tmpColor;

	// �A�j���[�V�����̍X�V.
	_pSprite->animeController.Update(delta);

}

bool Bat::Damage(int damage, DrawObj* pHitObj) {
	if (!_isDamageable) {
		return false;
	}

	// HP�����炷.
	_hp -= damage;
	if (_hp < 0) {	// HP��0�����������.
			// ���Ɏ��S���̃_���[�W�ʂɉ����Đ�����є{����ς���@�o���b�g���u���X�g���������������悤�ɂ���H.
		float magnification = 1;
		if (damage > 1) {
			magnification = 1.4f;
		}
		_isDamageable = false;
		_deathTime = 0.5;
		_velocity.y = (-800 + (-800 * (rand() % 16) / 16)) * magnification;
		_isLeft = pHitObj->GetWorldPosition().x < _position.x;	// �ڐG�����I�u�W�F�N�g�̍��W�̋t���ɔ��
		if (_isLeft) {
			_velocity.x = ((900.0f / 8) * ((rand() % 8))) * magnification;
		}
		else {
			_velocity.x = ((-900.0f / 8) * ((rand() % 8))) * magnification;
		}

		_pSprite->animeController.SetSeqIndex(Death);
	}
	_isDamage = true;
	return true;
}

void Bat::Attack(double delta) {
	

	// �U����ԂłȂ��A���v���C���[����苗���ȓ��ɂ��鎞�ɁA�U����Ԃɐ؂�ւ��.
	if (!_attack && _inPlayerX) {
		_velocity.y = JUMP_FORCE;
		_attack = true;
		_pSprite->animeController.SetSeqIndex(Fly);
	}

	// �U��������.
	if (_attack) {
		if (_isLeft) {
			_velocity.x = -MOVE_SPEED;
		}
		else {
			_velocity.x = MOVE_SPEED;
		}
		_velocity.y += GRAVITY * delta;


		if (_velocity.y < 0) {


			// TODO ������W�i�{���͒n�`����j�ɐڐG������U���t���O��false�ɂ��Ē�~.
			// �ڐG������
			// ���̒n�`��������Y���W _yBat ��p�̓����蔻�肪�K�v
			if (CeilingCheck()) {	// �ڐG��.
				// ��ԉ��̍��W
				// _ceilingObj.at(0)->GetWorldPosition().y �I�u�W�F�N�g�̒��S���W
				// _ceilingObj.at(0)->GetCollision().GetWorldPosition() �R���W�����̍��W
				float buttom = _ceilingObj.at(0)->GetCollision().GetWorldPosition().y + _ceilingObj.at(0)->GetCollision().hl.y;
				for (auto itr = _ceilingObj.begin() + 1; itr != _ceilingObj.end(); ++itr) {
					AABB2DObj a = (*itr)->GetCollision();
					if (buttom < a.GetWorldPosition().y + a.hl.y) {
						buttom = a.GetWorldPosition().y + a.hl.y;
					}
				}
				_position.y = buttom - (_ceilingCheck.p.y - _ceilingCheck.hl.y) ;

				_pSprite->animeController.SetSeqIndex(Stay);
				_velocity.y = 0;
				_velocity.x = 0;
				_attack = false;
			}
		}
		_position.x += _velocity.x * delta;
		_position.y += _velocity.y * delta;


	}

	HitCheck();

}

bool Bat::PlayerCheck() {
	if (_pPlayer == nullptr) {
		return false;
	}

	int _checkX;	// �v���C���[�ƓG�̍��W��
	int _checkY;
	
	// �v���C���[�Ƃ̍��W�����擾
	_checkX = _position.x - _pPlayer->GetWorldPosition().x;
	_checkY = _position.y - _pPlayer->GetWorldPosition().y;


	if (_checkX * _checkX <= PLAYER_RANGE * PLAYER_RANGE) {
		_inPlayerX = true;	// �v���C���[���߂��ɂ���.

		// �i�s�����̐ݒ�.
		if (_checkX > 0){			// ���W���r���ăv���C���[�̂�����Ɍ�����.
			_isLeft = true;
		
		}
		else {
			_isLeft = false;
		}
	}
	else {
		_inPlayerX = false;
	}

	return _inPlayerX;
}

bool Bat::CeilingCheck() {
	_isCeiling = false;
	_ceilingObj.clear();	// �q�b�g�����I�u�W�F�N�g���ꗗ�����Ă���

	// e�͂Ȃ�ł����� �u���b�N���X�g�������Ă���
	for (auto& e : *_pBlockList) {
		// �u���b�N���̓����蔻������Ă���
		if (AABB2DObj::RectOnRect(_ceilingCheck, e.GetCollision())) {
			_isCeiling = true;
			_ceilingObj.emplace_back(&e);
		}
	}
	// _pThroughBlockList ���蔲���\�ȏ�
	for (auto& e : *_pThroughBlockList) {
		// �u���b�N���̓����蔻������Ă���
		if (AABB2DObj::RectOnRect(_ceilingCheck, e.GetCollision())) {
			_isCeiling = true;
			_ceilingObj.emplace_back(&e);
		}
	}

	return _isCeiling;

}

bool Bat::HitCheck() {
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