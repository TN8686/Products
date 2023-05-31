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

	// オブジェクトを生成.
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
	// 初期配置.
	int id = 0;
	for (int i = 0; id < BLOCK_OBJ_NUM; ++i) {

		/*
		if (i % 32 > 12 && i % 32 <= 18 &&	// 中央6 * 6の範囲は配置しない.
			i / 32 > 5 && i / 32 <= 11) {
			continue;
		}*/

		// ブロックを配置.
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
	// キー入力を取得.
	int key = GetJoypadInputState(DX_INPUT_PAD1);

	// リセット.
	if (key & PAD_INPUT_8 || CheckHitKey(KEY_INPUT_R)) {
		init();
	}

	objectManager_->update();

	// 一括の当たり判定チェック.
	CollisionManager::getInstance()->allHitCheck();
	
	objectManager_->lateUpdate();

	// escで終了.
	if (key & PAD_INPUT_7 || CheckHitKey(KEY_INPUT_ESCAPE)) {
		return Scene::EXIT_CODE_EXIT;
	}
	return Scene::EXIT_CODE_CONTINUE;
}

bool GameScene::render()
{
	objectManager_->render();
	
	// 当たり判定の描画.
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

	// とりあえずの情報表示.
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
