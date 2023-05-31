#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace Engine {
	class GameObject;
	struct Transform;

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

		std::vector<ObjectListSet> objectListSet_;	// 管理しているオブジェクトのリスト.
		std::vector<GameObject*> renderList_;	// 描画リスト領域.

		int handle_;	//	現在操作中のリストのハンドル.


		// コンストラクタ.
	private:
		ObjectManager()
			: handle_(-1)
		{};

		// デストラクタ.
	private:
		~ObjectManager() {
			// TODO 最低でもオブジェクト破棄関数を実行するようにする.　
		}

		// 操作.
	public:
		static ObjectManager* getInstance() { return &instance_; }

		// 新たなオブジェクトリスト領域を追加し、そのハンドルを返します.
		int addObjectList(size_t reserveNum = 256);

		// ハンドルのオブジェクトリストに保持されているオブジェクトを全て破棄します.
		bool deleteObjectList();
		
		// オブジェクトを破棄します.
		// TODO 現行のコリジョンとの整合性が取れていないため、コリジョンを持たせたオブジェクトを破棄しないようにしてください.
		// コリジョン関連の修正が終われば無問題です.
		bool deleteObject(GameObject* obj);

		// リストに新たなオブジェクトを追加します.
		template <class T>
		bool createObject(const T& copyObject, GameObject** ref = nullptr);
		template <class T>
		bool createObject(const T& copyObject, T** ref = nullptr);

		// TODO 名前からオブジェクトを検索.
		//bool getObjectByName(int handle_, const std::string& name, GameObject* ref);
		//bool getObjectByName(int handle_, size_t& hash, GameObject* ref);

		bool load();
		bool init();
		bool update();
		bool lateUpdate();
		bool render();
		bool unload();

	private:
		// ハンドルが有効なものかどうかをチェックします.
		bool handleCheck(int handle_);

		// 現在操作中のリストをソートします.
		void sort();

		// 属性.
	public:

		// オブジェクトリストを取得します　TODO あっていい物だろうか…　なんにせよ一旦動作チェック用に.
		bool getObjectList(std::vector<GameObject*>* ref) {
			if (!handleCheck(handle_)) {
				return false;
			}

			ref = &objectListSet_.at(handle_).list;
		}

		// 現在操作中のハンドルを返します.
		int getHandle() { return handle_; }

		// 操作するリストのハンドルを設定します.
		bool setHandle(int handle) {
			if (!handleCheck(handle)) {
				return false;
			}
			handle_ = handle;
			return true;
		}
	};

	template <class T>
	bool ObjectManager::createObject(const T& copyObject, T** ref) {

		// ハンドルが有効な値かチェック.
		if (!handleCheck(handle_)) {
			return false;
		}

		auto p = new T(copyObject);
		auto& listSet = objectListSet_.at(handle_);
		if (listSet.freeSpaceNum > 0) {		// 空きがある場合.
			--listSet.freeSpaceNum;			// カウンタをデクリメント.
			listSet.list.at(listSet.freeSpaceNum) = p;	// 空き領域の端を使用.
		}
		else {
			listSet.list.emplace_back(p);	// 空きがない場合、領域を追加しオブジェクトを生成.
		}

		listSet.isSorted = false;	// 未ソート状態.

		p->load();

		if (ref != nullptr) {
			*ref = p;
		}

		return true;
	}

	template <class T>
	bool ObjectManager::createObject(const T& copyObject, GameObject** ref) {
		T* p = nullptr;	// 返却値受け取り用.

		if (!createObject(copyObject, &p)) {	// 返却値をT*で受け取る関数に委譲.
			return false;
		}

		if (ref != nullptr) {
			*ref = p;
		}
		return true;
	}
}