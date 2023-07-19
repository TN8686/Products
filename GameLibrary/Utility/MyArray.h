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

		// 先頭のポインタを返します.
		T* begin() {
			return root_;
		}

		// 末尾の要素の次のポインタを返します.
		T* end() {
			return &root_[pos_];
		}
		
		// 先頭の要素への参照を返します.
		T& front() {
			return root_;
		}

		// 末尾の要素への参照を返します.
		T& back() {
			return root_[pos_ - 1];
		}

		// 全てのオブジェクトをpopします.
		void resetPos() {
			pos_ = 0;
		}

		// コンテナが空かどうか判定します.
		bool empty() {
			return (pos_ == 0);
		}

		// オブジェクトを末尾に追加します.
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

		// TODO 作り途中.
		// オブジェクトを直接構築で末尾に追加します.
		// 領域が不足していた場合、新たに領域を確保します.
		template<typename... Args>
		void emplace(const Args&&... args) {
			if (pos_ >= allocSize_) {							// 追加しようとした位置が確保領域以上だったら.
				T* temp = static_cast<T*>(new char[(allocSize_ + ADD_ALLOCSIZE) * sizeof(T)]);	// 新たにより大きな領域を確保する.
			
				for (unsigned int i = 0;i < allocSize_; ++i) {	// 新たな領域に全てコピー.
					temp[i] = root_[i];
				}

				new(temp[pos_++]) T(std::forward<Args>(args)...);	// オブジェクトを構築して終端を一つ進める.

				for (int i = pos_; i < allocSize_ + ADD_ALLOCSIZE; ++i) {	// 残りの領域を埋める.
					new(temp[i]) T();
				}

				if (root_ != nullptr) {							// 古い領域を削除.
					delete[] root_;
				}
				root_ = temp;									// 始点を変更.
				allocSize_ += ADD_ALLOCSIZE;					// 領域確保数を更新.

			}
			else {
				root_[pos_]->~T();
				new(root_[pos_++]) T(std::forward<Args>(args)...);	// オブジェクトを構築して終端を一つ進める.
			}
		}

		// 終端オブジェクトをpopしつつ返します。
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