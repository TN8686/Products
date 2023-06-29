#pragma once
#include "MyArray.h"

namespace Utility {

	/////////////////////////////
	// 配置newを用いたメモリ確保用クラス
	/////////////////////////////
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
		size_t getCapacity() {
			return capacity_;
		}

		// 最初のポインタを返します.
		char* begin() {
			return root_;
		}

		// 末尾（最後の要素の次）のポインタを返します.
		char* end() {
			return root_ + capacity_;
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

	/////////////////////////////////////////////////
	// 決まったバイト数単位でメモリプールを管理するオブジェクト
	/////////////////////////////////////////////////
	class MemoryPool : public MemoryBuff {
	private:
		MyArray<size_t> unUseIdList_;	// 未使用インデックスのリスト.
		size_t objSize_;	// オブジェクト一つあたりのバイト数.

	public:
		MemoryPool()
			: objSize_(0)
		{}

		~MemoryPool() {
			refresh();
		}

		// 操作.
	public:
		// 領域を確保します.
		bool init(size_t objNum, size_t objSize) {
			if (!memoryAlloc(objNum * objSize)) {
				return false;
			}
			objSize_ = objSize;
			unUseIdList_.resize(objNum);

			// 管理情報を埋める.
			for (size_t i = objNum; i > 0; --i) {
				unUseIdList_.push(i - 1);
			}
			return true;
		}

		// 未使用領域のポインタを使用中領域として記録して、その領域の先頭のポインタを返却します.
		bool getUnused(void** ref){
			if (unUseIdList_.size() <= 0) {	// 空き領域が無い.
				return false;
			}

			*ref = root_ + (unUseIdList_.pop() * objSize_);	// 空き領域IDを一つpopしサイズを掛け、その分offsetしたポインタを返却値にセット.
			return true;
		}

		// 引数に与えられたポインタの領域を未使用領域として記録します.
		bool setUnused(void* point) {
			if (point < root_ || point >= root_ + capacity_) {	//	領域外チェック.
				return false;
			}

			auto num = (static_cast<char*>(point) - root_) / objSize_;	// インデックスを算出.

			auto p = unUseIdList_.begin();
			auto end = unUseIdList_.end();
			auto ret = std::find(p, end, num);	// 未使用リストに存在するかチェック.
			if (ret != end) {						// 既に未使用として記録されている場合は失敗.
				return false;
			}

			// 未使用領域として記録.
			unUseIdList_.push(num);

			return true;
		}

		void refresh() override {
			MemoryBuff::refresh();
			objSize_ = 0;
			unUseIdList_.refresh();
		}
	};

}