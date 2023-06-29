#include "ObjectManager.h"
#include "GameObject.h"

namespace Engine {

	using namespace std;
	using namespace Utility;

	// インスタンス.
	ObjectManager ObjectManager::instance_;

	ObjectManager::ObjectManager()
		: key_("")
		, keyHash_(HASH_DIGEST(""))
		, isDrawListUpdate(true)
	{}

	ObjectManager::~ObjectManager()
	{
		// 最悪リストの全削除は行う.

		MyArray<uint32_t> keyList(objectListSet_.size());

		// 全てのオブジェクトリストを破棄.
		for (const auto& e : objectListSet_) {
			keyList.push(e.first);
		}

		for (const auto& e : keyList) {
			keyHash_ = e;
			deleteObjectList();
		}
	}

	bool ObjectManager::addObjectList(string key, size_t reserveNum)
	{
		auto hash = Hash::GetDigest(key.c_str(), key.length());
		if (hash == HASH_DIGEST("")) {
			return false;
		}

		auto pair = objectListSet_.try_emplace(hash);			// 新たな領域を作成.
		if (!pair.second)
		{
			return false;										// 新たな領域が作成できなかったら失敗.
		}
		heapList_.try_emplace(hash);							// ヒープ上リストにも新たな領域を作成.

		pair.first->second.list.resize(reserveNum, nullptr);	// オブジェクトの領域.
		objectListSet_.at(hash).freeSpaceNum = static_cast<unsigned int>(reserveNum);	// 空き領域数を記録.
		objectListSet_.at(hash).isSorted = true;											// ソート済みフラグ.


		if (renderList_.capacity() < reserveNum) {	// 不足していれば描画用領域を確保.
			renderList_.reserve(reserveNum);
		}

		return true;
	}

	// 現在使用中のリストを破棄します.
	bool ObjectManager::deleteObjectList()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		//unload();	// 保持しているオブジェクトを全てunload.

		// 破棄.
		objectListSet_.erase(keyHash_);
		addList_.clear();
		deleteIdList_.clear();

		for (auto& e : heapList_.at(keyHash_)) {
			delete e;
		}
		heapList_.at(keyHash_).refresh();

		key_ = "";
		keyHash_ = HASH_DIGEST("");

		return true;
	}

	// 引数のオブジェクトを生成待ちリストに追加します.
	bool ObjectManager::createObject(GameObject* pObject)
	{
		if (keyHash_ == HASH_DIGEST("") || pObject == nullptr) {
			return false;
		}

		addList_.emplace_back(pObject);

		return true;
	}

	// 引数のオブジェクトを破棄待ちリストに追加します.
	// CreateObjectOnHeapで生成されたオブジェクトだった場合はメモリも解放します.
	bool ObjectManager::deleteObject(GameObject* obj)
	{
		if (obj == nullptr) {
			return false;
		}

		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto hitItr = std::find(addList_.begin(), addList_.end(), obj);// 待ちリストをチェック.
		if (hitItr != addList_.end()) {	// 追加待ちリストに存在している.
			*hitItr = nullptr;			// 追加しないようにnullptrに.

			// さらに、CreateObjectOnHeapで生成していた場合はdeleteする.
			auto& heapList = heapList_.at(keyHash_);
			auto end = heapList.end();
			auto pHit = std::find(heapList.begin(), end, obj);	// heapList_にあるかをチェック.
			if (pHit != end) {		// 存在したら.
				delete *pHit;		// 領域を解放.
				*pHit = *(end - 1);	// その位置に最後の要素をコピー.
				heapList.pop();		// popする事で最後の要素を破棄.
			}

			return true;				// 終了.
		}
	
		auto listSet = objectListSet_.at(keyHash_);
		auto first = listSet.list.begin() + listSet.freeSpaceNum;						// 空き領域を除いた範囲の始点.
		hitItr = std::lower_bound(first, listSet.list.end(), obj);						// ターゲット以上の値を探す.

		if (hitItr == listSet.list.end() || *hitItr != obj) {							// 最後までヒットしなかった、または対象が等価でない.
			return false;
		}

		(*hitItr)->setActive(false);		// 非アクティブに.

		deleteIdList_.emplace_back(std::distance(listSet.list.begin(), hitItr));	// インデックスを出してリストに追加.

		return true;
	}

	bool ObjectManager::init()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// 空き領域の分、offsetして走査.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			(*itr)->init();
		}

		return true;
	}

	bool ObjectManager::update()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		if (!deleteIdList_.empty()) {
			actuallyDelete();	// 破棄待ちを破棄.
		}
		if (!addList_.empty()) {
			actuallyCreate();	// 追加待ちを追加.
		}

		// 空き領域の分、offsetして走査.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->update();
			}
		}

		return true;
	}

	bool ObjectManager::lateUpdate()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// 空き領域の分、offsetして走査.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->lateUpdate();
			}
		}

		return true;
	}

	bool ObjectManager::render()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		if (isDrawListUpdate) {
			// 空き領域を除いたリストを複製.
			auto itr = objectListSet_.at(keyHash_).list.begin() + objectListSet_.at(keyHash_).freeSpaceNum;
			renderList_.assign(itr, objectListSet_.at(keyHash_).list.end());
			isDrawListUpdate = false;
		}

		// z順でソート.
		std::stable_sort(renderList_.begin(), renderList_.end(),
			[](const GameObject* a, const GameObject* b) {
				
				if (a->getDrawLayer() < b->getDrawLayer()) {		// 描画レイヤー昇順.
					return true;
				}
				else if(a->getDrawLayer() == b->getDrawLayer()) {	// 同じだったら.
					return a->getTransform().getWorldPosition().z < b->getTransform().getWorldPosition().z;	// Z座標昇順.
				}
				return false;
			}
		);

		// 描画.
		for (auto& e : renderList_) {
			if (e->getActive() && e->getDrawActive()) {
				e->render();
			}
		}

		return true;
	}

	bool ObjectManager::unload()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// 空き領域の分、offsetして走査.
		auto& objList = objectListSet_.at(keyHash_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->unload();
		}

		return true;
	}

	// 現在操作中のリストをソートします.
	void ObjectManager::sort()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return;
		}

		if (!objectListSet_.at(keyHash_).isSorted) {
			// ポインタでソート.
			std::stable_sort(objectListSet_.at(keyHash_).list.begin(), objectListSet_.at(keyHash_).list.end(),
				[](const GameObject* a, const GameObject* b) {
					return a < b;
				}
			);
		}
		return;
	}

	// 生成待ちリストを元に実際にオブジェクトを追加します.
	bool ObjectManager::actuallyCreate()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// 前もって一斉にロード.
		for (auto& e : addList_) {
			if (e == nullptr) {
				continue;
			}
			e->load();
		}

		// 追加しつつ初期化.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto& e : addList_) {
			if (e == nullptr) {
				continue;
			}
			if (listSet.freeSpaceNum > 0) {						// 空きがある場合.
				--listSet.freeSpaceNum;							// カウンタをデクリメント.
				listSet.list.at(listSet.freeSpaceNum) = e;		// 空き領域の端を使用.

				listSet.isSorted = false;	// 未ソート状態.
			}
			else {
				listSet.list.emplace_back(e);	// 空きがない場合、領域を追加しオブジェクトを生成.

				listSet.isSorted = false;	// 未ソート状態.
			}

			e->init();
		}
		addList_.clear();

		sort();
		isDrawListUpdate = true;

		return true;
	}

	bool ObjectManager::actuallyDelete()
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto& listSet = objectListSet_.at(keyHash_);

		auto& heapList = heapList_.at(keyHash_);
		auto begin = heapList.begin();
		auto end = heapList.end();
		for (auto& id : deleteIdList_) {
			auto& p = listSet.list.at(id);	// インデックスから対象のオブジェクトを取得.
			//p->unload();

			auto pHit = std::find(begin, end, p);	// heapList_にあるかをチェック.
			if (pHit != end) {		// 存在したら.
				delete *pHit;		// 領域を解放.
				*pHit = *(end - 1);	// その位置に最後の要素をコピー.
				heapList.pop();		// popする事で最後の要素を破棄.
			}

			p = nullptr;	// nullptrを代入.

		}
		listSet.freeSpaceNum += static_cast<unsigned int>(deleteIdList_.size());	// カウンタを増やす.

		deleteIdList_.clear();
		listSet.isSorted = false;
		sort();	// ソート.
		isDrawListUpdate = true;

		return false;
	}

	bool ObjectManager::setKey(std::string key)
	{
		auto hash = Hash::GetDigest(key.c_str(), key.length());
		if (!objectListSet_.contains(hash)) {	// 存在しなかったら失敗.
			return false;
		}

		if (!deleteIdList_.empty()) {
			actuallyDelete();	// 破棄待ちを破棄.
		}
		if (!addList_.empty()) {
			actuallyCreate();	// 追加待ちを追加.
		}

		key_ = key;
		keyHash_ = hash;
		isDrawListUpdate = true;
		return true;
	}

}