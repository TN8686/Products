#include "CollisionManager.h"
#include "GameObject.h"

#include "CollisionTrees.h"

namespace Engine {

	using namespace Utility;

	CollisionManager CollisionManager::instace_;

	CollisionManager::~CollisionManager() {
		clear();		// 最悪クリア関数は実行されるようにしておく.
	}

	bool CollisionManager::initialize()
	{
		static constexpr int INIT_CAPASITY = 1024;	// 仮の初期確保数領域数.

		// 全てのレイヤー毎に領域を確保.
		for (int i = 0; i < LAYER_NUM; ++i) {
			aabbCls2DList_[i].reserve(INIT_CAPASITY);
		}


		// 今は不要.
		{	// ここは外部からロードするようにすべきな気がする

			// 組合せの数を算出.
			combinationList_.resize(LAYER_NUM * (LAYER_NUM + 1) / 2);

			// TODO ここでチェックリストを作成.
		}

		if (!l4tree_.init(4, -32, -32, (float)WindowSize::WIDTH + 32, (float)WindowSize::HEIGHT + 32)) {
			return false;	// 四分木生成失敗.
		}

		return true;
	}


	bool CollisionManager::createAABBCollision(GameObject* const parent, const CllisionLayer& layer, AABBCollision2D*& ref) {

		if (layer >= LAYER_NUM || layer < 0 || parent == nullptr) {	// レイヤーと親オブジェクトの正常値チェック.
			return false;
		}

		
		auto* p = new AABBCollision2D();		// 生成.
		p->setParent(parent);					// 親オブジェクトを関連付け.
		p->setLayer(layer);						// レイヤーをセット.
		ref = p;								// 返却用引数にポインタを渡す.

		aabbCls2DList_[layer].emplace_back(p);	// リストに登録　TODO 空きを探索する仕組みを追加.
		

		return true;
	}

	// 全ての当たり判定のチェックを行います.
	// combinationListを基準にレイヤー同士でヒットチェックを行います.
	// 一回のゲームループで一度だけ実行してください.
	bool CollisionManager::allHitCheck()
	{
		static CollisionList<AABBCollision2D>* pList;
		auto colNum = l4tree_.getAllCollisionList(&pList);
		colNum /= 2;

		AABBCollision2D** pRoot = pList->getRootPtr();
		for (unsigned int i = 0; i < colNum; ++i) {
			if (AABBOnAABB(*pRoot[i * 2], *pRoot[i * 2 + 1])) {
				// ヒット関数実行.
				pRoot[i * 2]->getParent()->onAABBStay2D(pRoot[i * 2 + 1]);
				pRoot[i * 2 + 1]->getParent()->onAABBStay2D(pRoot[i * 2]);
			}
		}

		return true;
	}

	// 能動的な個別のAABB2Dヒットチェックを行います.
	// combinationListにおける設定は無視され、指定レイヤーのオブジェクトをチェックします.
	// 処理順に注意してください（オブジェクトAのupdateで利用した際、オブジェクトBのアップデートがまだの場合に想定された位置に無い場合があります）.
	bool CollisionManager::activeHitCheckAABB2D(AABBCollision2D* const collision, const CllisionLayer& layer)
	{
		if (layer < 0 || layer >= LAYER_NUM) {	// レイヤー不正チェック.
			return false;
		}

		for (auto& e : aabbCls2DList_[layer]) {
			if (!e->getActive() || collision == e) {	// アクティブ状態をチェック、同じオブジェクトかをチェック.
				continue;
			}
			if (AABBOnAABB(*collision, *e)) {
				// それぞれのヒット時関数実行　TODO EnterやExitも実装したい.
				collision->getParent()->onAABBStay2D(e);
				e->getParent()->onAABBStay2D(collision);
			}
		}

		return false;
	}

	// 全ての当たり判定を描画します.
	bool CollisionManager::render()
	{
		unsigned int c = 0;
		for (int i = 0; i < LAYER_NUM; ++i) {

			// レイヤーで色を変える.
			c = COLOR[i];

			for (auto& e : aabbCls2DList_[i]) {
				if (!e->getActive()) {
					continue;
				}
				e->render(c);
			}
		}
		return true;
	}

	// 全ての当たり判定オブジェクトを破棄します
	// 必ず全てのゲームオブジェクトの破棄を行ってから実行してください.
	bool CollisionManager::clear()
	{
		
		// 全ての要素を削除.
		for (int i = 0; i < LAYER_NUM; ++i) {
			for (auto& e : aabbCls2DList_[i]) {
				delete e;						// 個別の当たり判定オブジェクトをdelete.
			}
			aabbCls2DList_[i].clear();			// vector配列をクリア.
			aabbCls2DList_[i].shrink_to_fit();	// メモリを解放.
		}

		return true;
	}

	bool CollisionManager::AABBOnAABB(AABBCollision2D a, AABBCollision2D b)
	{

		// スケールを反映しつつ矩形に変換.
		Rect rect1, rect2;
		rect1 = a.getWorldAABB2D();
		rect2 = b.getWorldAABB2D();

		return	rect1.topLeft.y < rect2.bottomRight.y &&
				rect1.bottomRight.y > rect2.topLeft.y &&
				rect1.topLeft.x < rect2.bottomRight.x &&
				rect1.bottomRight.x > rect2.topLeft.x;
	}

	bool CollisionManager::CircleOnCircle(CircleCollision2D a, CircleCollision2D b)
	{
		auto ca = a.getWorldCircle2D();
		auto cb = b.getWorldCircle2D();

		// 円1の原点から円2の原点までの距離が双方の半径の合計未満だったら.
		return	(ca.position.x - cb.position.x) * (ca.position.x - cb.position.x) +
				(ca.position.y - cb.position.y) * (ca.position.y - cb.position.y) <
				(ca.radius + cb.radius) * (ca.radius + cb.radius);
	}

}