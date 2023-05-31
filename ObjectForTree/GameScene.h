#pragma once
#include "Engine/Scene.h"

namespace Object {
	class Block;
}

class GameScene : public Engine::Scene {
	
	// �萔.
private:
	static constexpr int PLAYER_OBJ_NUM = 3;
	static constexpr int BLOCK_OBJ_NUM = 2703; // 540

	Object::Block* block[BLOCK_OBJ_NUM];

	// �����o�ϐ�.
private:
	// ����.
public:
	bool load() override;
	bool init() override;
	int	update() override;
	bool render() override;
	bool unload() override;
};