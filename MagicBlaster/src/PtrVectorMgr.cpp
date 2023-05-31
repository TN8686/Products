#include "PtrVectorMgr.h"

namespace PtrVector {
	// IManaged

	// コピーコンストラクタ.
	IManaged::IManaged(const IManaged& obj)
		: _isRegistered(false)
		, _index(SIZE_MAX)
		, _priority(obj._priority)
		, _pManager(obj._pManager)
	{
		if (obj._isRegistered) {			// 管理者が居たら.
			assert(_pManager != nullptr);	// 性質上nullにはならないはず.
			_pManager->Register(this);		// 同じ管理者に新規に登録.
		}
	}

	// 登録解除.
	bool IManaged::Unregister() {
		if (_pManager == nullptr) {	// 管理者がnullだったらなにもしない.
			return false;
		}

		_pManager->Unregister(this);	// 実際の登録解除そのものは管理者に任せる.
		return true;
	}


	// 代入演算子　protectedメンバとする事で、継承したクラス内を介さないと参照できなくしている.
	// 継承したクラスの代入演算子の中で利用する事で、同じ型のオブジェクト同士の代入でなければ使用できないようにできる.
	IManaged& IManaged::operator=(const IManaged& obj) {
		Unregister();						// 一旦登録解除.

		_isRegistered = false;				// 一応初期化.
		_index = SIZE_MAX;					// 一応初期化.
		_priority = obj._priority;			// ソート優先度をコピー.
		_pManager = obj._pManager;			// 管理オブジェクトを代入.

		if (obj._isRegistered) {			// 管理者が居たら.
			assert(_pManager != nullptr);	// 性質上nullにはならないはず.
			_pManager->Register(this);		// 同じ管理者に新規に登録.
		}
		
		return *this;
	}
	
}