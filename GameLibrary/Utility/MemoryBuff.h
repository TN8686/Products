#pragma once
#include <algorithm>

#include "MyArray.h"

namespace Utility {

	////////////////////////////////////
	// 配置new用のメモリを確保するクラス
	////////////////////////////////////
	class MemoryBuff {
	protected:
		size_t capacity_;
		char* root_;

		// コンストラクタ.
	public:
		MemoryBuff() 
			: capacity_(0)
			, root_(nullptr)
		{}
		// デストラクタ.
		virtual ~MemoryBuff() {
			refresh();	// メモリを解放.
		}

		// 領域を確保します.
		bool memoryAlloc(size_t capacity)
		{
			if (root_ != nullptr) {
				return false;
			}
			capacity_ = capacity;
			root_ = new char[capacity_];	// 領域を確保.
			return true;
		}

		// 領域の総バイト数を返します.
		size_t capacity() {
			return capacity_;
		}

		// 最初のポインタを返します.
		char* begin() {
			return root_;
		}

		// 末尾（最後の要素の次）のポインタを返します.
		char* end() {
			return &root_[capacity_];
		}

		// 領域を解放します 領域内のオブジェクトのデストラクタは呼び出されない事に注意してください.
		virtual void refresh() {
			if (root_ != nullptr) {
				delete[] root_;
			}
			root_ = nullptr;
			capacity_ = 0;
		}
	};

	// MemoryPool上でオブジェクトを破棄するためのインターフェースクラス.
	class IMemoryPool {
	public:
		virtual ~IMemoryPool() = default;
	};

	////////////////////////////////////////////////////////////////////
	// 決まったバイト数単位で配置new用のメモリプールを管理するクラス.
	// IMemoryPoolを継承しているオブジェクトにのみ対応しています.
	// 
	// 使用方法.
	// 最初にinitで領域を確保し、placementでオブジェクトを生成、配置できます.
	// removalで配置したオブジェクトを破棄できます.
	// このオブジェクトを破棄するか、refreshで全てのオブジェクトを破棄し、確保していたメモリを解放します.
	////////////////////////////////////////////////////////////////////
	class MemoryPool : protected MemoryBuff {
	private:
		MyArray<size_t> freeIdList_;	// 未使用インデックスのリスト.
		size_t objSize_;				// オブジェクト一つあたりのバイト数.

	public:
		MemoryPool()
			: objSize_(0)
		{}

		~MemoryPool() override {
			refresh();	// メモリを解放.
		}

		// 操作.
	public:
		/// <summary>
		/// 領域を確保します.
		/// </summary>
		/// <param name="areaNum">領域の数</param>
		/// <param name="objSize">領域一つあたりのバイト数</param>
		/// <returns></returns>
		bool init(size_t areaNum, size_t objSize);

		/// <summary>
		/// 未使用領域の一つにオブジェクトを生成、配置し、
		/// その領域を領域として記録後、ポインタを返却します.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="ref">ポインタ返却用　また、この引数の型のオブジェクトを生成します</param>
		/// <param name="...args">配置するオブジェクトのコンストラクタに渡す引数</param>
		/// <returns></returns>
		template<typename T, typename... Args>
		bool placement(T*& ref, Args&&... args);

		// 引数に与えられたポインタのオブジェクトを破棄し、領域を未使用領域として記録します.
		template<typename T>
		bool removal(T*& removal_pointer);

		// 領域の総バイト数を返します.
		size_t capacity() {
			return MemoryBuff::capacity();
		}

		// 総要素数を返します.
		size_t size() {
			if (objSize_ <= 0) {
				return 0;
			}
			return MemoryBuff::capacity() / objSize_;
		}

		// 空き要素数を返します.
		size_t freeCount() {
			return freeIdList_.size();
		}

		// オブジェクトを全て破棄し、確保しているメモリを解放します.
		void refresh() override;
	};

	/// <summary>
	/// 未使用領域の一つにオブジェクトを生成、配置し、
	/// その領域を領域として記録後、ポインタを返却します.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="ref">ポインタ返却用　また、この引数の型のオブジェクトを生成します</param>
	/// <param name="...args">配置するオブジェクトのコンストラクタに渡す引数</param>
	/// <returns></returns>
	template<typename T, typename ...Args>
	bool MemoryPool::placement(T*& ref, Args && ...args) {

		// IMemoryPoolを継承していない場合は関数が生成できずコンパイルエラー.
		{
			IMemoryPool* check = ref;
		}

		// 空き領域が無ければ失敗.
		if (freeIdList_.size() <= 0) {
			return false;
		}

		// オブジェクトのバイト数が一要素のバイト数より大きいと失敗.
		if (sizeof(T) > objSize_) {
			return false;
		}


		// 配置先のポインタを取得.
		auto p = &root_[freeIdList_.pop() * objSize_];	// 空き領域IDを一つpopし、そのIDの分オフセットしたポインタを算出.

		// オブジェクトを配置して、ポインタを返却値にセット.
		ref = new(p) T(std::forward<Args>(args)...);
		return true;
	}

	// 引数に与えられたポインタのオブジェクトを破棄し、領域を未使用領域として記録します.
	template<typename T>
	bool MemoryPool::removal(T*& removal_pointer)
	{

		// IMemoryPoolを継承していない場合は関数が生成できずコンパイルエラー.
		{
			IMemoryPool* type_check = removal_pointer;
		}

		// ポインタ演算のため、char*型として解釈.
		char* p = reinterpret_cast<char*>(removal_pointer);

		// 領域外チェック.
		if (p < root_ || p >= root_ + capacity_) {
			return false;
		}

		// 確保領域内だけど各領域の先頭じゃなかったら失敗.
		if ((p - root_) % objSize_ != 0) {
			return false;
		}

		// インデックスを算出.
		auto id = (p - root_) / objSize_;

		// 既に未使用領域リストに存在していたら失敗.
		auto begin = freeIdList_.begin();
		auto end = freeIdList_.end();
		if (std::find(begin, end, id) != end) {
			return false;
		}

		// デストラクタを実行.
		removal_pointer->~T();

		// nullptrを代入.
		removal_pointer = nullptr;

		// 未使用領域として記録.
		freeIdList_.push(id);

		return true;
	}
}