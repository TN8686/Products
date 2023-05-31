#include "EnemyGenerator.h"
#include "Engine/Graphics.h"
#include "Engine/Sprite.h"

const AnimationSequence EnemyGenerator::ANIM = { { {0, 3.0 / 60}, {1, 3.0 / 60},{2, 3.0 / 60},{3, 3.0 / 60},{4, 3.0 / 60},{5, 3.0 / 60},{6, 3.0 / 60},{7, 3.0 / 60},{8, 3.0 / 60}} };

SpriteResource* EnemyGenerator::_pSpriteResource;

bool EnemyGenerator::Load()
{
	assert(_pSpriteResource != nullptr);
	if (_pSpriteResource == nullptr) {
		return false;
	}
	// �A�j���[�V�������X�g�̊��蓖��.
	_pSpriteResource->SetAnimationList({ ANIM });

	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/generator.png")) {
		return false;
	}

	// �X�v���C�g�̗̈���m��.
	_pSpriteResource->pGetSpriteList()->reserve(5);	// ����5.

	// �Z�����X�g�̍쐬.
	if (_pSpriteResource->CreateCellList(9, 1, 498, 498) != 0) {
		return false;
	};

	return true;
}

// ������.
bool EnemyGenerator::Init()
{
	assert(_pSpriteResource != nullptr);
	if (_pSpriteResource == nullptr) {
		return false;
	}
	_pSpriteResource->Register(_spriteHandle);
	_spriteHandle.pGetSprite()->scale = { 0.5f, 0.5f };
	_queue.id = EnemyID::Mummy;
	_queue.pCamera = _pCamera;
	_queue.pPlayer = _pPlayer;
	return true;
}

void EnemyGenerator::Update(double delta) {
	if (_generateTimer.Update(delta)) {
		_queue.point = _position;						// ���݃W�F�l���[�^�̍��W��.
		_queue.isRight = (rand() % 2) ? true : false;	// ���E�̓����_��.
		EnemyFactory::pGetInstance()->Enqueue(_queue);	// �����˗�.
	}

	SpriteUpdate(delta);
}

void EnemyGenerator::SpriteUpdate(double delta)
{
	auto pSprite = _spriteHandle.pGetSprite();		// �X�v���C�g�̃|�C���^���擾.

	if (pSprite == nullptr) {
		return;
	}
	if (!_onActive) {	// �I�u�W�F�N�g���A�N�e�B�u�łȂ��Ȃ�����A�X�v���C�g����A�N�e�B�u�ɂ���.
		pSprite->onActive = false;
		return;
	}

	// �X�v���C�g���W�̍X�V.
	pSprite->pos.x = _position.x;
	pSprite->pos.y = _position.y;

	// �A�j���[�V�����̍X�V.
	pSprite->animeController.Update(delta);
}
