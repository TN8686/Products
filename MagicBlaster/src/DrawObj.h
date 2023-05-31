#pragma once
#include "GameObj.h"
#include "Engine/Sprite.h"
#include "assert.h"

// TODO スプライト関連を作り直しきったら色々と組みなおす必要がありそう.

// スプライト、アニメーションリスト、セルリスト、テクスチャについて.
// 外部またはstaticなメンバ変数としてListを作り、スプライトは個別に割り当て、アニメーションリストはスプライトに設定.
// セルリスト、テクスチャはDraw関数に引き渡して下さい.

class DrawObj : public virtual GameObj {
protected:
	Sprite* _pSprite;		// 使用するスプライトを割り当てて下さい TODO 破棄予定 代わりにSpriteResourceとSpriteHandorerをセットで使用してください.
	Camera* _pCamera;

public:
	DrawObj(Point2D position = {0, 0}) :
		GameObj(position),
		_pSprite(nullptr),
		_pCamera(nullptr)		
	{};
	virtual ~DrawObj() = default;


	virtual void SpriteUpdate(double delta) = 0;

	void SetOnActive(bool onActive) override { 
		_onActive = onActive;
		assert(_pSprite != nullptr);
		if (_pSprite != nullptr) {
			_pSprite->onActive = onActive;
		}
	}
	void SetSprOnActive(bool onActive) {
		assert(_pSprite != nullptr);
		if (_pSprite != nullptr) {
			_pSprite->onActive = onActive;
		}
	}
	void pSetSprite(Sprite* sprite) { _pSprite = sprite; }
	void pSetCamera(Camera* pCamera) { _pCamera = pCamera; }

	bool GetSprOnActive() const { return _pSprite->onActive; }

};