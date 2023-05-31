#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Audio.h"
#include "../CountDownTimer.h"
#include "../PtrVectorMgr.h"

namespace {
	constexpr int SPR_NUM = 5;
}
/**
* タイトル画面.
*/
class TutorialScene : public Scene
{
public:
	static ScenePtr Create();

	virtual bool Load() override;
	virtual int Update(double delta) override;
	virtual void Draw() const override;

private:
	TutorialScene();
	TutorialScene(const TutorialScene&) = delete;
	TutorialScene& operator=(const TutorialScene&) = delete;

	int _page;	// ページ数.

	// 背景.
	SpriteResource2 _resBack;
	SpriteObj _sprBack;

	// チュートリアル画像.
	SpriteResource2 _res[SPR_NUM];
	std::vector<SpriteObj> _spr;
	GameObj _parent;
	bool _toRight;

	SpriteResource2 _resBlack;
	SpriteObj _sprBlack;

	CountDownTimer _timer;
	CountDownTimer _blackTimer;
	bool _toBlack;
};
