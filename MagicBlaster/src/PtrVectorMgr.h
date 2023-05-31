#pragma once

#include <vector>
#include <memory>
#include <assert.h>

namespace PtrVector {

	// 前方宣言.
	class IManaged;

	template <typename T>
	class PtrVectorMgr;

	namespace {
		class IPtrVectorMgrBase;
	}

	/// <summary>
	/// PtrVectorMgrに管理されるオブジェクトのインターフェース.
	/// 継承する事でPtrVectorMgr及びその派生クラスに管理される事ができる.
	/// IManagedを継承した何らかの派生クラスCChildと、それを管理するPtrVectorMgr&lt;CChild&gt;、またはPtrVectorMgr&lt;CChild&gt;を継承したクラスのセットで利用する.
	/// </summary>
	class IManaged {
		friend IPtrVectorMgrBase;		// フレンドクラスとする事でIPtrVectorMgrBaseを介せばアクセスできるように.

	private:
		// privateメンバにして継承したクラスからは値にアクセスできないように.
		IPtrVectorMgrBase* _pManager;	// 管理者のポインタ.
		bool _isRegistered;				// 登録済みフラグ.
		size_t _index;					// 管理されているリスト上でのインデックス.
		int _priority;					// ソート優先度（現在はソート未実装）.

	protected:
		// 代入演算子　protectedメンバにする事で、継承した同じ型のオブジェクト同士の代入でなければ参照できなくしている。
		IManaged& operator=(const IManaged& obj);

		// 仮に、IManagedを継承した派生クラスCChildで代入演算子を明示的に定義したい場合は、以下のようにPtrVector::IManaged::operator=()を呼び出してください.
		/*
		CChild& operator=(const CChild& obj) {
			PtrVector::IManaged::operator=(obj);	// 明示的にPtrVector::IManaged::operator=()を呼び出す.

			// 派生クラス独自の処理.

			return *this;
		};
		*/

	public:
		explicit IManaged()
			: _pManager(nullptr)
			, _isRegistered(false)
			, _index(SIZE_MAX)			// 非登録時のインデックスは最大にしておく.
			, _priority(0)
		{}

		// コピーコンストラクタ　暗黙の型変換を禁止.
		explicit IManaged(const IManaged& obj);

		// デストラクタ.
		~IManaged() {
			Unregister();	// オブジェクトの破棄時に自身の登録を自動で解除.
		};

		// ソート優先度を取得します.
		const int& GetPriority() { return _priority; }

		// ソート優先度を設定します.
		void SetPriority(const int& priority) { _priority = priority; }

		// 管理者が登録されている場合、自身の登録を解除します.
		bool Unregister();
	};


	// 無名名前空間.
	namespace {
		/// <summary>
		/// IManagedがPtrVectorMgrを参照し、PtrVectorMgrがIManagedの値を書き換えるための仲介インターフェース.
		/// こうする事でIManagedがテンプレートクラスである必要が無くなった.
		/// </summary>
		class IPtrVectorMgrBase {
			friend IManaged;	// IManagedからのみアクセス可能に.
		protected:
			// 実際に登録を行うための関数を定義.
			virtual bool Register(IManaged* pManaged) = 0;

			// 実際に登録解除を行うための関数を定義.
			virtual bool Unregister(IManaged* pManaged) = 0;

			/// IManagedの各値をこのインターフェースを介して書き換え可能に.
			IPtrVectorMgrBase*& rpGetManager(IManaged* obj) { return obj->_pManager; }
			bool& rGetIsRegistered(IManaged* obj) { return obj->_isRegistered; }
			size_t& rGetIndex(IManaged* obj) { return obj->_index; }
			int& rGetPriority(IManaged* obj) { return obj->_priority; }

		};
	}


	/// <summary>
	/// IManaged及びその派生クラスを管理する機能を持つテンプレートクラス.
	/// IManagedを継承した何らかの派生クラスCChildと、それを管理するPtrVectorMgr&lt;CChild&gt;、またはPtrVectorMgr&lt;CChild&gt;を継承したクラスのセットで利用する.
	/// 対象としているIManagedを継承したクラスのオブジェクトをRegister関数で登録するとそのポインタをリスト化し.
	/// 登録されているオブジェクトが破棄された時には自動的にリストから登録を解除します。
	///
	/// TODO ソート機能の実装　→　ダミーオブジェクトが末尾に来るように　昇順、降順に合わせソート前にダミーのプライオリティを変更する？　ポインタを直にダミーと比較する？.
	/// TODO ガベージコレクションの実装　→　ソート後末尾の要素を削除していき、shrink_to_fit().
	/// </summary>
	/// <typeparam name="T">IManagedを継承したクラス</typeparam>
	template <typename T>
	class PtrVectorMgr : public IPtrVectorMgrBase {
	private:
		std::vector<T*> _ptrList;				// 管理対象オブジェクトのポインタリスト.
		std::vector<std::size_t> _freeIDList;	// _ptrList内の、使用後にオブジェクトが破棄されて空いたインデックスのリスト.
		T _dummy;								// 未使用インデックスの要素として使用するダミーオブジェクト 
												// TODO このままだとデフォルトコンストラクタがないクラスで使用できないからなんらかの変更が必要　ソートがちょっと手間になるけどダミーじゃなくnullptr入れる？.
												//		どのみち走査するような処理を追加するなら何らかの形でdummyとの比較が必要になるし、nullチェックでもいいのでは.

	protected:
		bool Register(IManaged* obj) override;
		bool Unregister(IManaged* obj) override;

	public:
		PtrVectorMgr()
		{
			rGetPriority(&_dummy) = INT_MAX;		// ダミーのソート優先度をひとまず最低に.
		};

		/// <summary>
		/// 内部のリストのreserveを行います.
		/// </summary>
		/// <param name="reserveNum">確保したい領域の数</param>
		PtrVectorMgr(const size_t& reserveNum) {
			rGetPriority(&_dummy) = INT_MAX;		// ダミーのソート優先度をひとまず最低に.
			Reserve(reserveNum);					// リザーブ.
		}
		~PtrVectorMgr();

		// 内部リストのポインタを取得します.
		std::vector<T*>* pGetList() { return &_ptrList; }
		const std::vector<T*>* pGetList() const { return &_ptrList; }

		// 複製を禁止.
		PtrVectorMgr(const PtrVectorMgr&) = delete;
		const PtrVectorMgr& operator=(const PtrVectorMgr&) = delete;

		// 内部のリストのreserveを行います.
		void Reserve(const size_t& reserveNum) {
			_ptrList.reserve(reserveNum);
			_freeIDList.reserve(reserveNum);
		}

		// 管理オブジェクトを登録します.
		bool Register(T* obj) {
			return Register(static_cast<IManaged*>(obj));	// IManaged*を引数とする方の関数に任せる そのまま渡すと再帰になってしまうので明示的にキャスト.
		}

		// 管理オブジェクトを登録解除します.
		bool Unregister(T* obj) {
			return Unegister(static_cast<IManaged*>(obj));	// IManaged*を引数とする方の関数に任せる そのまま渡すと再帰になってしまうので明示的にキャスト.
		}
	};


	// IPtrVectorMgr

	// デストラクタ.
	template <typename T>
	PtrVectorMgr<T>::~PtrVectorMgr()
	{
		if (_freeIDList.size() != _ptrList.size()) {	// フリーIDの数がリストのサイズと一致している場合、全て空のはずなので何もしない.
			for (auto& e : _ptrList) {					// 全ての管理対象に対して登録解除を通知.
				e->Unregister();
			}
		}
	}

	// 登録.
	// 内部でIManaged*をT*にキャストしている部分があるが、
	// この関数はprotectedのため、外部からは引数がT*の関数を介してしか呼び出せず.
	// 内部からは登録済みのIManagedからしか呼び出せないため、登録方式が安全なら安全なはず.
	template <typename T>
	bool PtrVectorMgr<T>::Register(IManaged* obj) {
		// 各パラメータの参照を取る.
		auto& rpManager = rpGetManager(obj);
		auto& rIsRegistered = rGetIsRegistered(obj);
		auto& rIndex = rGetIndex(obj);

		// 登録済みの場合、管理者があるかをチェック.
		if (rIsRegistered) {
			if (rpManager == this) {						// 管理者が自身だったら何もしない.
				return false;
			}
			else {											// 管理者が自身でなかったら.
				assert(rpManager != nullptr);				// 性質上nullにはならないはず.
				obj->Unregister();							// 登録を解除.
			}
		}

		// 登録処理.
		if (_freeIDList.empty()) {							// 未使用インデックスリストが空の場合.
			rIndex = _ptrList.size();						// インデックスを保持（emplace_backの後ならsize() - 1が追加したインデックス　末尾ならdistanceを使わなくても良い）.
			_ptrList.emplace_back(static_cast<T*>(obj));	// リストにポインタを登録.
		}
		else {												// 未使用インデックスがある場合.
			rIndex = _freeIDList.back();					// 未使用のインデックスを一つpop.
			_freeIDList.pop_back();
			_ptrList.at(rIndex) = static_cast<T*>(obj);		// リストにポインタを登録.
		}

		// 自身を管理者に登録し登録済みに.
		rpManager = this;
		rIsRegistered = true;

		return true;
	}

	// 登録解除.
	template <typename T>
	bool PtrVectorMgr<T>::Unregister(IManaged* obj) {
		
		// インデックスの参照を取る.
		auto& rIndex = rGetIndex(obj);

		// 不正indexチェック.
		assert(!(rIndex < 0 || rIndex >= _ptrList.size()));
		if (rIndex < 0 || rIndex >= _ptrList.size()) {
			return false;
		}

		// リストから削除.
		_ptrList.at(rIndex) = &_dummy;		// 使用していたインデックスの要素をダミーに.
		_freeIDList.emplace_back(rIndex);	// 使用していたインデックスを未使用リストに登録.
		rGetIsRegistered(obj) = false;		// 登録解除.
		rpGetManager(obj) = nullptr;		// 管理者をnullptrに.

		return true;
	}
}