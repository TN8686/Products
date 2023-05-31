#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"


/**
* ƒ^ƒCƒgƒ‹‰æ–Ê.
*/
class GameClearScene : public Scene
{
public:
	static ScenePtr Create();

	virtual bool Load() override;
	virtual int Update(double delta) override;
	virtual void Draw() const override;

private:
	GameClearScene();
	GameClearScene(const GameClearScene&) = delete;
	GameClearScene& operator=(const GameClearScene&) = delete;

	Texture texture[2];
	std::vector<Sprite> sprBg;
	std::vector<Sprite> sprFont;
	CellList fontCellList;
	double waitTimer;

};
