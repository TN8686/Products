#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Audio.h"
#include "../NumDraw.h"
#include "../CountDownTimer.h"
#include "../Reference.h"

class ResoultScene : public Scene
{
public:
	enum ExitCode
	{
		ExitCode_Title = ExitCode_User,
	};

	static ScenePtr Create();

	virtual bool Load() override;
	virtual int Update(double delta) override;
	virtual void Draw() const override;

private:
	ResoultScene();
	ResoultScene(const ResoultScene&) = delete;
	ResoultScene& operator=(const ResoultScene&) = delete;

	Texture _rect;
	Sprite _spriteBg;
	Sprite _spriteBlack;

	CountDownTimer _blackTimer;
	CountDownTimer _scoreTimer;

	NumDraw::NumDraw _numDraw;

	SpriteResource2 _textResouce;
	std::vector<SpriteObj> _spriteText;

	bool _isHighScoreUpdate;
};
