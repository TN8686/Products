#pragma once

#include <functional>
#include <cassert>

namespace Utility {
	//////////////////////////////////
	// シンプルなコンテナクラス
	//////////////////////////////////
	template < class T >
	class MyArray {
		static constexpr int ADD_ALLOCSIZE = 128;

	protected:
		T* root_;		// リストルートポインタ.
		size_t	pos_;
		size_t	allocSize_;

		// コンストラクタ.
	public:
		explicit MyArray(size_t size = 0)
			: root_(nullptr)
			, pos_(0)
			, allocSize_(size)
		{
			root_ = new T[allocSize_];
		}

		// コピーコンストラクタ.
		explicit MyArray(const MyArray<T>& myArray)
			: root_(nullptr)
			, pos_(0)
			, allocSize_(0)
		{
			resize(myArray.allocSize_);
			auto end = myArray.root_ + myArray.pos_;
			for (int i = 0; myArray.root_ + i != end; ++i) {
				*(root_ + i) = *(myArray.root_ + i);
			}
			pos_ = myArray.pos_;
		}

		// 代入演算子.
		MyArray<T>& operator =(const MyArray<T>& myArray) {
			refresh();
			resize(myArray.allocSize_);
			auto end = myArray.root_ + myArray.pos_;
			for (int i = 0; myArray.root_ + i != end; ++i) {
				*(root_ + i) = *(myArray.root_ + i);
			}
			pos_ = myArray.pos_;

			return (*this);
		}

		// デストラクタ.
		~MyArray() {
			refresh();
		}

		// 要素アクセス.
		T& operator[](const int& id) {
			assert(id < pos_);
			return root_[id];
		}

		size_t size() {
			return pos_;
		}

		// 最初のポインタを返します.
		T* begin() {
			return root_;
		}

		// 末尾（最後の要素の一つ先）のポインタを返します.
		T* end() {
			return &root_[pos_];
		}

		// 全てのオブジェクトをpopします.
		void resetPos() {
			pos_ = 0;
		}

		// オブジェクトを追加します.
		// 領域が不足していた場合、新たに領域を確保します.
		void push(const T& obj = T()) {
			if (pos_ >= allocSize_) {							// 追加しようとした位置が確保領域以上だったら.
				auto temp = new T[allocSize_ + ADD_ALLOCSIZE];	// 新たにより大きな領域を確保する.
				for (unsigned int i = 0; i < allocSize_; ++i) {	// 新たな領域に全てコピー.
					temp[i] = root_[i];
				}
				if (root_ != nullptr) {							// 古い領域を削除.
					delete[] root_;
				}
				root_ = temp;									// 始点を変更.
				allocSize_ += ADD_ALLOCSIZE;					// 領域確保数を更新.

			}
			root_[pos_++] = obj;								// オブジェクトを代入して終端を一つ進める.
		}

		// 終端オブジェクトpopしつつ返します。
		T pop() {
			return root_[--pos_];								// 終端のオブジェクトを返して終端を一つ戻す.
		}

		// 領域をクリア.
		void refresh() {
			if (root_ != nullptr) {
				delete[] root_;
			}
			root_ = nullptr;
			pos_ = 0;
			allocSize_ = 0;
		}

		// 新たに領域を確保します.
		bool resize(size_t num) {
			size_t end = num < allocSize_ ? num : allocSize_;	// 小さい方を選択.

			auto temp = new T[num];								// 新たに領域を確保する.
			for (size_t i = 0; i < end; ++i) {					// 新たな領域に入るだけコピー.
				temp[i] = root_[i];
			}
			if (root_ != nullptr) {								// 古い領域を削除.
				delete[] root_;
			}
			root_ = temp;										// 始点を更新.
			allocSize_ = num;									// 領域確保数を更新.

			return true;
		}
	};
}