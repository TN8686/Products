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
	// アニメーションリストの割り当て.
	_pSpriteResource->SetAnimationList({ ANIM });

	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/generator.png")) {
		return false;
	}

	// スプライトの領域を確保.
	_pSpriteResource->pGetSpriteList()->reserve(5);	// 仮に5.

	// セルリストの作成.
	if (_pSpriteResource->CreateCellList(9, 1, 498, 498) != 0) {
		return false;
	};

	return true;
}

// 初期化.
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
		_queue.point = _position;						// 現在ジェネレータの座標に.
		_queue.isRight = (rand() % 2) ? true : false;	// 左右はランダム.
		EnemyFactory::pGetInstance()->Enqueue(_queue);	// 生成依頼.
	}

	SpriteUpdate(delta);
}

void EnemyGenerator::SpriteUpdate(double delta)
{
	auto pSprite = _spriteHandle.pGetSprite();		// スプライトのポインタを取得.

	if (pSprite == nullptr) {
		return;
	}
	if (!_onActive) {	// オブジェクトがアクティブでなくなったら、スプライトも非アクティブにする.
		pSprite->onActive = false;
		return;
	}

	// スプライト座標の更新.
	pSprite->pos.x = _position.x;
	pSprite->pos.y = _position.y;

	// アニメーションの更新.
	pSprite->animeController.Update(delta);
}
