#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Audio.h"
#include "../CountDownTimer.h"
#include "../PtrVectorMgr.h"
#include "../NumDraw.h" 

/**
* ƒ^ƒCƒgƒ‹‰æ–Ê.
*/
class TitleScene : public Scene
{
public:
	static ScenePtr Create();

	virtual bool Load() override;
	virtual int Update(double delta) override;
	virtual void Draw() const override;

private:
	TitleScene();
	TitleScene(const TitleScene&) = delete;
	TitleScene& operator=(const TitleScene&) = delete;

	//AnimationList test = { { { {0, 8.0 / 60}, {1, 8.0 / 60}, {2, 8.0 / 60}, {3, 8.0 / 60}, {4, 8.0 / 60}, {5, 8.0 / 60}, {6, 8.0 / 60}, {7, 8.0 / 60} }, true } };
	
	SpriteResource2 _spriteResource2;
	SpriteObj _sprite;

	SpriteResource2 _blackResource;
	SpriteObj _spriteBlack;

	//SpriteResource _spriteResource;
	//SpriteHandle _spriteHandle;

	CountDownTimer _timer;

	NumDraw::NumDraw _numDraw;
};
