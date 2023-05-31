#pragma once
#include <vector>
#include <queue>
#include <memory>
#include <assert.h>

#include "Engine/primitive2d.h"

class Block;

enum class EnemyID {
	Mummy,
	Bat,
	EnumEnemyID_Num
};

class Enemy;
class Camera;
class Player;
class SpriteResource;

typedef struct _EnemyCreateQueue {
	EnemyID id;
	Point2D point;
	Camera* pCamera;
	Player* pPlayer;
	bool isRight = false;
} EnemyCreateQueue;

class EnemyFactory {
private:
	static EnemyFactory _instace;
	static constexpr int ENEMY_MAX = 32 * 5;	// 仮.

	const std::vector<Block>* _pBlockList;								// 個別のエネミーに登録するための地形オブジェクトリスト.
	const std::vector<Block>* _pThroughBlockList;						// 同じくすり抜け地形オブジェクトリスト.
	std::vector<std::unique_ptr<SpriteResource>>* _pSpriteResourceList;	// スプライトリソースリストのポインタ.

	std::vector<EnemyCreateQueue> _queue;								// キューだけど一旦残りを置いておいて次のフレームで処理…とかはやらないのでvector
	std::vector<std::unique_ptr<Enemy>> _enemyList;						// 敵のリスト.

	EnemyFactory() = default;	// シングルトンにするためprivate.
public:
	~EnemyFactory() = default;

	static EnemyFactory* pGetInstance() { return &_instace; }

	bool Load();
	void Finalize();
	std::vector<std::unique_ptr<Enemy>>& GetEnemyList() { return _enemyList; }

	void pSetBlockList(const std::vector<Block>* pBlockList) { _pBlockList = pBlockList; }
	void pSetThroughBlockList(const std::vector<Block>* pThroughBlockList) { _pThroughBlockList = pThroughBlockList; }
	void pSetSpriteResourceList(std::vector<std::unique_ptr<SpriteResource>>* pList) { _pSpriteResourceList = pList; }

	bool Create();
	bool Enqueue(EnemyID id, Point2D point, Camera* pCamera, Player* pPlayer, bool isRight = false);
	bool Enqueue(EnemyCreateQueue queue);

	void EnemyInit(Enemy* pEnemyObj, EnemyCreateQueue queue);
};