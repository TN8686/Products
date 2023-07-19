#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "../Utility/MyArray.h"
#include "../Utility/hash.h"

namespace Engine {


	class GameObject;
	struct Transform;

	using ObjectArray = Utility::MyArray<GameObject*>;

	class ObjectManager {
		struct ObjectListSet {
			std::vector<GameObject*> list;
			unsigned int freeSpaceNum;		// 未使用領域カウンタ.
			bool isSorted;					// ソート済みフラグ.

			ObjectListSet()
				: freeSpaceNum(0)
				, isSorted(false)
			{}
		};

		// メンバ変数.
	private:
		// シングルトン.
		static ObjectManager instance_;

		std::unordered_map<uint32_t, ObjectListSet> objectListSet_;	// 管理しているオブジェクトのリスト.
		std::vector<GameObject*> renderList_;		// 描画リスト.
		std::vector<GameObject*> addList_;			// 追加待ちリスト.
		std::vector<size_t> deleteIdList_;			// 破棄待ちのインデックスリスト.
		std::unordered_map<uint32_t, ObjectArray> heapList_;		// 自身がヒープ上に確保したオブジェクトのリスト.
		std::string key_;							// 現在操作中のリストのキー名.
		uint32_t keyHash_;							// 現在操作中のリストのキー名のハッシュ値.
		bool isDrawListUpdate;						// 描画用リストを更新する必要があるかどうか.

		// コンストラクタ.
	private:
		ObjectManager();

		// デストラクタ.
	private:
		~ObjectManager();

		// 操作.
	public:
		static ObjectManager* getInstance() { return &instance_; }

		// キーに新たなオブジェクトリスト領域を追加します　キーが重複している場合は失敗します.
		bool addObjectList(std::string key, size_t reserveNum = 256);

		// 現在使用中のリストを破棄します.
		bool deleteObjectList();
		
		// オブジェクトを追加します.
		bool createObject(GameObject* pObject);
		
		// ヒープ上に新たなメモリを確保し、オブジェクトを追加します.
		// この関数で生成されたオブジェクトは、メモリ管理をGameObjectが行います。
		template<class T>
		bool createObjectOnHerp(const T& copyObject, T** ref);

		// ヒープ上に新たなメモリを確保し、オブジェクトを追加します.
		// この関数で生成されたオブジェクトは、メモリ管理をGameObjectが行います。
		template<class T>
		bool createObjectOnHerp(const T& copyObject, GameObject** ref);

		// オブジェクトを破棄します.
		bool deleteObject(GameObject* obj);

		// TODO 名前からオブジェクトを検索.
		//bool getObjectByName(const std::string& name, GameObject* ref);
		//bool getObjectByNameHash(size_t& hash, GameObject* ref);

		bool init();
		bool preUpdate();
		bool update();
		bool lateUpdate();
		bool render();
		bool unload();

	private:
		// 現在操作中のリストをソートします.
		void sort();

		// 生成待ちリストを元に実際にオブジェクトを追加します.
		bool actuallyCreate();

		// 破棄待ちリストを元に実際にオブジェクトを破棄します.
		bool actuallyDelete();

		// 属性.
	public:

		// オブジェクトリストを取得します　TODO あっていい物だろうか…　なんにせよ一旦動作チェック用に.
		bool getObjectList(std::vector<GameObject*>* ref) {
			ref = &objectListSet_.at(keyHash_).list;
		}

		// 現在操作中のキー名を返します.
		std::string getKey() { return key_; }

		// 操作するリストのキーを設定します.
		bool setKey(std::string key);
	};

	// ヒープ上に新たなメモリを確保し、オブジェクトを追加します.
	// この関数で生成されたオブジェクトは、メモリ管理をGameObjectが行います。
	template<class T>
	inline bool ObjectManager::createObjectOnHerp(const T& copyObject, T** ref)
	{
		// キーが有効な値かチェック.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto p = new T(copyObject);

		heapList_.at(keyHash_).push(p);	// 確保したオブジェクトのリストに登録.
		createObject(p);				// 本リスト登録処理は共通.

		if (ref != nullptr) {
			*ref = p;
		}

		return true;
	}

	// ヒープ上に新たなメモリを確保し、オブジェクトを追加します.
	// この関数で生成されたオブジェクトは、メモリ管理をObjectManagerが行います。
	template<class T>
	inline bool ObjectManager::createObjectOnHerp(const T& copyObject, GameObject** ref)
	{
		T* p = nullptr;	// 返却値受け取り用.

		if (!createObjectOnHerp(copyObject, &p)) {	// 返却値をT*で受け取る関数に委譲.
			return false;
		}

		if (ref != nullptr) {
			*ref = p;
		}
		return true;
	}
}