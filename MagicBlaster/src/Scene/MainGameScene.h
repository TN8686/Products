#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Audio.h"
#include "../Player.h"
#include "../Camera.h"
#include "../Block.h"
#include "../Bullet.h"
#include "../BulletQueue.h"
#include "../EnemyFactory.h"
#include "../EnemyGenerator.h"
#include "../MagicBlast.h"

#include "../NumDraw.h"	// âº.

#include <memory>

class MainGameScene : public Scene
{
public:
	enum ExitCode
	{
		ExitCode_GameClear = ExitCode_User,
		ExitCode_GameOver,
		ExitCode_Pause,
		ExitCode_Title,
		ExitCode_Resoult,

	};

	bool Load() override;
	bool Unload() override;

	static ScenePtr Create();

	virtual int Update(double delta) override;
	virtual void Draw() const override;
private:
	MainGameScene();
	MainGameScene(const MainGameScene&) = delete;
	MainGameScene& operator=(const MainGameScene&) = delete;

	std::vector<std::unique_ptr<SpriteResource>> _spriteResourceList;	// àÍî‘êÊÇ…êÈåæÇµÇƒÇ®Ç≠éñÇ≈ÅASpriteHandleÇÃâï˙ñYÇÍÇÇ–Ç∆Ç‹Ç∏âÒîÇµÇ‚Ç∑Ç≠.

	EnemyFactory* _pEnemyFactory;

	Texture texture[5];
	Texture texMagicCircle;
	Texture texMagicBullet;
	CellList fontCellList;
	CellList witchCellList;
	CellList blockCellList;

	std::vector<Sprite> sprBackground;
	std::vector<Sprite> sprMagicCircle;
	std::vector<Sprite> sprBlock;
	std::vector<Sprite> sprMagicBullet;
	std::vector<Sprite> sprSquare;

	Camera _camera;
	Player _player;
	MagicBlast _magicBlast;
	std::vector<std::unique_ptr<EnemyGenerator>> _enemyGeneratorList;
	std::vector<Block> _blockObj;
	std::vector<Block> _throughBlockObj;
	std::vector<Bullet> _MagicBulletObj;
	std::unique_ptr<MagicBulletQueue> bulletQueue;

	SpriteResource2 _blackResource;
	SpriteObj _spriteBlack;
	CountDownTimer _blackTimer;

	NumDraw::NumDraw _numDraw;	// âºÇÃUI.
	SpriteResource2 _textResouce;
	std::vector<SpriteObj> _spriteText;

	CountDownTimer _mainTimer;
	CountDownTimer _startTimer;
};