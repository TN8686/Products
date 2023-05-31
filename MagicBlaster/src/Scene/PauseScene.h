#pragma once
#include "../Engine/Scene.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Audio.h"

/**
* ƒ^ƒCƒgƒ‹‰æ–Ê.
*/
class PauseScene : public Scene
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
	PauseScene();
	PauseScene(const PauseScene&) = delete;
	PauseScene& operator=(const PauseScene&) = delete;

	Texture texture;
	CellList fontCellList;
	std::vector<Sprite> sprFont;
};
