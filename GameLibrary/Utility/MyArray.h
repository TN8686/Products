#pragma once

#include <functional>
#include <cassert>

namespace Utility {
	//////////////////////////////////
	// �V���v���ȃR���e�i�N���X
	//////////////////////////////////
	template < class T >
	class MyArray {
		static constexpr int ADD_ALLOCSIZE = 128;

	protected:
		T* root_;		// ���X�g���[�g�|�C���^.
		size_t	pos_;
		size_t	allocSize_;

		// �R���X�g���N�^.
	public:
		explicit MyArray(size_t size = 0)
			: root_(nullptr)
			, pos_(0)
			, allocSize_(size)
		{
			root_ = new T[allocSize_];
		}

		// �R�s�[�R���X�g���N�^.
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

		// ������Z�q.
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

		// �f�X�g���N�^.
		~MyArray() {
			refresh();
		}

		// �v�f�A�N�Z�X.
		T& operator[](const int& id) {
			assert(id < pos_);
			return root_[id];
		}

		size_t size() {
			return pos_;
		}

		// �ŏ��̃|�C���^��Ԃ��܂�.
		T* begin() {
			return root_;
		}

		// �����i�Ō�̗v�f�̈��j�̃|�C���^��Ԃ��܂�.
		T* end() {
			return &root_[pos_];
		}

		// �S�ẴI�u�W�F�N�g��pop���܂�.
		void resetPos() {
			pos_ = 0;
		}

		// �I�u�W�F�N�g��ǉ����܂�.
		// �̈悪�s�����Ă����ꍇ�A�V���ɗ̈���m�ۂ��܂�.
		void push(const T& obj = T()) {
			if (pos_ >= allocSize_) {							// �ǉ����悤�Ƃ����ʒu���m�ۗ̈�ȏゾ������.
				auto temp = new T[allocSize_ + ADD_ALLOCSIZE];	// �V���ɂ��傫�ȗ̈���m�ۂ���.
				for (unsigned int i = 0; i < allocSize_; ++i) {	// �V���ȗ̈�ɑS�ăR�s�[.
					temp[i] = root_[i];
				}
				if (root_ != nullptr) {							// �Â��̈���폜.
					delete[] root_;
				}
				root_ = temp;									// �n�_��ύX.
				allocSize_ += ADD_ALLOCSIZE;					// �̈�m�ې����X�V.

			}
			root_[pos_++] = obj;								// �I�u�W�F�N�g�������ďI�[����i�߂�.
		}

		// �I�[�I�u�W�F�N�gpop���Ԃ��܂��B
		T pop() {
			return root_[--pos_];								// �I�[�̃I�u�W�F�N�g��Ԃ��ďI�[����߂�.
		}

		// �̈���N���A.
		void refresh() {
			if (root_ != nullptr) {
				delete[] root_;
			}
			root_ = nullptr;
			pos_ = 0;
			allocSize_ = 0;
		}

		// �V���ɗ̈���m�ۂ��܂�.
		bool resize(size_t num) {
			size_t end = num < allocSize_ ? num : allocSize_;	// ����������I��.

			auto temp = new T[num];								// �V���ɗ̈���m�ۂ���.
			for (size_t i = 0; i < end; ++i) {					// �V���ȗ̈�ɓ��邾���R�s�[.
				temp[i] = root_[i];
			}
			if (root_ != nullptr) {								// �Â��̈���폜.
				delete[] root_;
			}
			root_ = temp;										// �n�_���X�V.
			allocSize_ = num;									// �̈�m�ې����X�V.

			return true;
		}
	};
}