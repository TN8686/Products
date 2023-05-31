#include "ObjectManager.h"
#include "GameObject.h"

namespace Engine {
	// インスタンス.
	ObjectManager ObjectManager::instance_;

	int ObjectManager::addObjectList(size_t reserveNum)
	{
		/*
		TODO　仮　空きを探す処理を入れる.
		*/

		// TODO ハンドル生成処理　現状IDをそのまま渡している.
		int handle = static_cast<int>(objectListSet_.size());
		objectListSet_.emplace_back();
		objectListSet_.at(handle).list.resize(reserveNum, nullptr);

		objectListSet_.at(handle).freeSpaceNum = static_cast<unsigned int>(reserveNum);
		objectListSet_.at(handle).isSorted = true;

		if (renderList_.capacity() < reserveNum) {	// 不足していれば描画用領域を確保.
			renderList_.reserve(reserveNum);
		}

		return handle;
	}

	// ハンドルのオブジェクトリストに保持されているオブジェクトを全て破棄します.
	bool ObjectManager::deleteObjectList()
	{
		// ハンドルが有効な値かチェック.
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		for (auto itr = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;
			itr != objectListSet_.at(handle_).list.end(); ++itr) {
			delete *itr;			// オブジェクトを破棄.
		}
		objectListSet_.at(handle_).list.clear();			// クリア.
		objectListSet_.at(handle_).list.shrink_to_fit();	// 領域を解放.

		objectListSet_.at(handle_).freeSpaceNum = 0;

		// TODO handle_が無効になった事を記録.

		return true;
	}


	// 引数のオブジェクトを破棄します.
	// TODO 現行のコリジョンとの整合性が取れていないため、コリジョンを持たせたオブジェクトを破棄しないようにしてください.
	// コリジョン関連の修正が終われば無問題です.
	bool ObjectManager::deleteObject(GameObject* obj)
	{
		if (obj == nullptr) {
			return false;
		}

		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		auto first = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;		// 空き領域を除いた範囲の始点.
		auto hitItr = std::lower_bound(first, objectListSet_.at(handle_).list.end(), obj);				// ターゲット以上の値を探す.

		if (hitItr == objectListSet_.at(handle_).list.end() || *hitItr != obj) {	// 最後までヒットしなかった、または対象が等価でない.
			return false;
		}

		delete *hitItr;		// オブジェクトを破棄.
		*hitItr = nullptr;	// nullを代入.
		++objectListSet_.at(handle_).freeSpaceNum;	// カウンタをインクリメント.

		// TODO できればここでソートしなくていいようにすべきな気がする.
		sort();

		return true;
	}

	bool ObjectManager::load()
	{
		if (!handleCheck(handle_)) {
			return false;
		}
		
		sort();

		// 空き領域の分offsetして走査.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
				(*itr)->load();
		}

		return true;
	}

	bool ObjectManager::init()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// 空き領域の分offsetして走査.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->init();
		}

		return true;
	}

	bool ObjectManager::update()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// 空き領域の分offsetして走査.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->update();
			}
		}

		return true;
	}

	bool ObjectManager::lateUpdate()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// 空き領域の分offsetして走査.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->lateUpdate();
			}
		}

		return true;
	}

	bool ObjectManager::render()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// 空き領域を除いたリストを複製.
		auto itr = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;
		renderList_.assign(itr, objectListSet_.at(handle_).list.end());

		// z順でソート.
		std::stable_sort(renderList_.begin(), renderList_.end(),
			[](const GameObject* a, const GameObject* b) {
				return a->getTransform().getWorldPosition().z < b->getTransform().getWorldPosition().z;
			}
		);

		for (auto& e : renderList_) {
			if (e->getActive() && e->getDrawActive()) {
				e->render();
			}
		}

		return true;
	}

	bool ObjectManager::unload()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// 空き領域の分offsetして走査.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->unload();
		}

		return true;
	}

	// ハンドルが有効なものかどうかをチェックします.
	bool ObjectManager::handleCheck(int handle)
	{
		// TODO 未実装.
		if (handle == -1) {	// -1だけ無効な値として扱う.
			return false;
		}
		return true;
	}

	// 現在操作中のリストをソートします.
	void ObjectManager::sort()
	{
		if (!handleCheck(handle_)) {
			return;
		}

		if (!objectListSet_.at(handle_).isSorted) {
			// ポインタでソート.
			std::stable_sort(objectListSet_.at(handle_).list.begin(), objectListSet_.at(handle_).list.end(),
				[](const GameObject* a, const GameObject* b) {
					return a < b;
				}
			);
		}
		return;
	}

}