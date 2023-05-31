#include "Mummy.h"
#include "Engine/Texture.h"
#include "Player.h"
#include "Block.h"
#include "Engine/Graphics.h"
#include "ScoreManager.h"

// 静的変数群.
// TODO　出現アニメーションを用意.
const AnimationSequence Mummy::_spawn = {};
const AnimationSequence Mummy::_walk = { { {10, 8.0 / 60}, {11, 8.0 / 60}, {12, 8.0 / 60}, {13, 8.0 / 60}, {14, 8.0 / 60}, {15, 8.0 / 60} } };
const AnimationSequence Mummy::_death = { {{4, 15.0 / 60}, {5, 3.0 / 60}, {6, 3.0 / 60}, {7, 3.0 / 60}, {8, 3.0 / 60}, {9, 3.0 / 60}, }, false };

SpriteResource* Mummy::_pSpriteResource;

/*
* static
* マミー用のセルリストの作成、スプライト領域の確保と初期化、画像のロード、アニメーションリストの割り当てを行います.
*/
bool Mummy::Load() {
	// アニメーションリストの割り当て.
	AnimationList tempAnimList(3);

	tempAnimList.at(Spawn) = _spawn;
	tempAnimList.at(Walk) = _walk;
	tempAnimList.at(Death) = _death;
	_pSpriteResource->SetAnimationList(tempAnimList);

	if (!Graphics::Get().texMap.LoadFromFile(_pSpriteResource->GetTexture(), L"Res/ene_06.png")) {
		return false;
	}

	// スプライトの領域を確保.
	_pSpriteResource->pGetSpriteList()->reserve(32);	// 仮に32.

	// セルリストの作成.
	if (_pSpriteResource->CreateCellList(10, 3, 48, 72) != 0) {
		return false;
	};

	return true;
}

/*
* 個別のオブジェクトの初期化を行います.
* 
*/
bool Mummy::Init() {
	_hp = HP_INIT;
	_pSpriteResource->Register(_spriteHandle);	// スプライトをハンドラに登録.

	auto pSprite = _spriteHandle.pGetSprite();		// スプライトのポインタを取得.
	assert(pSprite != nullptr);
	if (pSprite == nullptr) {
		return false;
	}

	pSprite->scale = { 4.0f, 4.0f };
	pSprite->animeController.SetSeqIndex(Walk);
	pSprite->animeController.Reset();
	pSprite->animeController.Update((rand() % 48) / 60.0);	// 仮に適当にアニメーションをズラす　最終的には不要.
	return true;
}

void Mummy::Update(double delta) {
	if (!_onActive) {
		return;
	}

	// ホントはよくない形だけど、ひとまずの死亡処理.
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
		// 前方をチェックし、壁だったら反転.
		ForwardWallCheck();
		if (_isForwardWall) {
			_isLeft = !_isLeft;
		}

		// 一緒に画面端チェック.
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

		// プレイヤーとのヒットチェック.
		HitCheck();
	}

	SpriteUpdate(delta);

	_isDamage = false;
}

void Mummy::SpriteUpdate(double delta) {
	auto pSprite = _spriteHandle.pGetSprite();		// スプライトのポインタを取得.

	if (pSprite == nullptr) {
		return;
	}
	if (!_onActive) {	// オブジェクトがアクティブでなくなったら、スプライトも非アクティブにする.
		pSprite->onActive = false;
		return;
	}

	// 左右を適用.
	pSprite->flipHorizontal = !_isLeft;

	// スプライト座標の更新.
	pSprite->pos.x = _position.x;
	pSprite->pos.y = _position.y;

	// 加算色補正を作成　被ダメージ時に白くフラッシュ.
	DirectX::XMFLOAT4 tmpColor = {};
	if (_isDamage) {
		tmpColor = { 1,1,1, 0 };
	}
	pSprite->addColor = tmpColor;

	// アニメーションの更新.
	pSprite->animeController.Update(delta);

}

/*
* 被ダメージ処理.
* 
* @param damage		ダメージ量.
* @param pHitObj	接触オブジェクトのポインタ.
* 
* @return	ヒット有無.
*/
bool Mummy::Damage(int damage, DrawObj* pHitObj) {
	if (!_isDamageable) {
		return false;
	}

	// HPを減らす.
	_hp -= damage;
	if (_hp < 0) {	// HPが0を下回ったら.
		// 仮に死亡時のダメージ量に応じて吹っ飛び倍率を変える　バレットかブラストかを引数から取れるようにする？.
		float magnification = 1;
		if (damage > 1) {
			magnification = 1.4;
		}
		_isDamageable = false;
		_hurtBox[0].SetOnActive(false);	// コリジョンをOFFに.
		_deathTime = 0.5;
		_velocity.y = (-800 + (-800 * (rand() % 16) / 16)) * magnification;
		_isLeft = pHitObj->GetWorldPosition().x < _position.x;	// 接触したオブジェクトの座標の逆側に飛ぶ
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

// プレイヤーとの接触判定を行います.
bool Mummy::HitCheck() {
	assert(_pPlayer != nullptr);
	if (_pPlayer == nullptr) {
		return false;
	}
	bool ret = false;
	if (AABB2DObj::RectOnRect(_hurtBox[0], *_pPlayer->GetHurtBox().data())) {
		ret = _pPlayer->Damage(30, this);	// 仮に30ダメージ入れる.
	}

	return ret;
}

// 前方との接触判定を行います.
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
