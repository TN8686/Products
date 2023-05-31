#include "GameScene.h"

#include "DxLib.h"
#include "Utility/Reference.h"

#include "Engine/ObjectManager.h"
#include "Engine/GameObject.h"
#include "Engine/CollisionManager.h"

#include "Object/Player.h"
#include "Object/Ball.h"
#include "Object/Block.h"
#include "Object/WindowWall.h"

using namespace Utility;
using namespace Engine;
using namespace Object;

bool GameScene::load()
{
	objMgrHandle_ = objectManager_->addObjectList(BLOCK_OBJ_NUM * 5 + PLAYER_OBJ_NUM);
	objectManager_->setHandle(objMgrHandle_);

	// �I�u�W�F�N�g�𐶐�.
	Block b;
	for (int i = 0; i < BLOCK_OBJ_NUM; ++i) {
		objectManager_->createObject(b, &block[i]);
		block[i]->setName("Block " + std::to_string(i));
	}

	GameObject* p = nullptr;
	for (int i = 0; i < 100; ++i) {
		objectManager_->createObject(Player(), &p);
		Float3 pos = { 50.0f * (i % 20), 50.0f * (i / 20), 0 };
		p->setPosition(pos);
	}
	objectManager_->createObject(WindowWall(), nullptr);
	return true;
}

bool GameScene::init()
{
	// �����z�u.
	int id = 0;
	for (int i = 0; id < BLOCK_OBJ_NUM; ++i) {

		/*
		if (i % 32 > 12 && i % 32 <= 18 &&	// ����6 * 6�͈͔̔͂z�u���Ȃ�.
			i / 32 > 5 && i / 32 <= 11) {
			continue;
		}*/

		// �u���b�N��z�u.
		block[id]->setPosition({ (float)((i) % 64 * 20 + 10), (float)((i) / 64 * 20 + 10), 0 });
		block[id]->setActive(true);
		++id;

		/*
		if (i == BLOCK_OBJ_NUM / 2) {
			i = 0;
		}*/
	}

	objectManager_->init();

	return true;
}

int GameScene::update()
{
	// �L�[���͂��擾.
	int key = GetJoypadInputState(DX_INPUT_PAD1);

	// ���Z�b�g.
	if (key & PAD_INPUT_8 || CheckHitKey(KEY_INPUT_R)) {
		init();
	}

	objectManager_->update();

	// �ꊇ�̓����蔻��`�F�b�N.
	CollisionManager::getInstance()->allHitCheck();
	
	objectManager_->lateUpdate();

	// esc�ŏI��.
	if (key & PAD_INPUT_7 || CheckHitKey(KEY_INPUT_ESCAPE)) {
		return Scene::EXIT_CODE_EXIT;
	}
	return Scene::EXIT_CODE_CONTINUE;
}

bool GameScene::render()
{
	objectManager_->render();
	
	// �����蔻��̕`��.
	static bool isDraw = false;
	static bool prev = false;

	int key = GetJoypadInputState(DX_INPUT_KEY_PAD1);
	if (key & PAD_INPUT_10) {
		if (!prev) {
			isDraw = !isDraw;
		}
	}

	if (isDraw) {
		CollisionManager::getInstance()->render();
	}

	prev = key & PAD_INPUT_10;

	// �Ƃ肠�����̏��\��.
	std::string str = "ARROW or WASD : MOVE    R : RESET    SPACE : DRAW COLLISION    ESC : EXIT";
	auto strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
	DrawString(WindowSize::WIDTH - strWidth, WindowSize::HEIGHT - GetFontSize(), str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

	return true;
}

bool GameScene::unload()
{
	objectManager_->unload();
	objectManager_->deleteObjectList();
	return true;
}
