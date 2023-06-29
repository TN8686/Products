#pragma once
#include "MyArray.h"

namespace Utility {

	/////////////////////////////
	// �z�unew��p�����������m�ۗp�N���X
	/////////////////////////////
	class MemoryBuff {
	protected:
		size_t capacity_;
		char* root_;

		// �R���X�g���N�^.
	public:
		MemoryBuff() 
			: capacity_(0)
			, root_(nullptr)
		{}
		// �f�X�g���N�^.
		virtual ~MemoryBuff() {
			refresh();	// �����������.
		}

		// �̈���m�ۂ��܂�.
		bool memoryAlloc(size_t capacity)
		{
			if (root_ != nullptr) {
				return false;
			}
			capacity_ = capacity;
			root_ = new char[capacity_];	// �̈���m��.
			return true;
		}

		// �̈�̑��o�C�g����Ԃ��܂�.
		size_t getCapacity() {
			return capacity_;
		}

		// �ŏ��̃|�C���^��Ԃ��܂�.
		char* begin() {
			return root_;
		}

		// �����i�Ō�̗v�f�̎��j�̃|�C���^��Ԃ��܂�.
		char* end() {
			return root_ + capacity_;
		}

		// �̈��������܂� �̈���̃I�u�W�F�N�g�̃f�X�g���N�^�͌Ăяo����Ȃ����ɒ��ӂ��Ă�������.
		virtual void refresh() {
			if (root_ != nullptr) {
				delete[] root_;
			}
			root_ = nullptr;
			capacity_ = 0;
		}
	};

	/////////////////////////////////////////////////
	// ���܂����o�C�g���P�ʂŃ������v�[�����Ǘ�����I�u�W�F�N�g
	/////////////////////////////////////////////////
	class MemoryPool : public MemoryBuff {
	private:
		MyArray<size_t> unUseIdList_;	// ���g�p�C���f�b�N�X�̃��X�g.
		size_t objSize_;	// �I�u�W�F�N�g�������̃o�C�g��.

	public:
		MemoryPool()
			: objSize_(0)
		{}

		~MemoryPool() {
			refresh();
		}

		// ����.
	public:
		// �̈���m�ۂ��܂�.
		bool init(size_t objNum, size_t objSize) {
			if (!memoryAlloc(objNum * objSize)) {
				return false;
			}
			objSize_ = objSize;
			unUseIdList_.resize(objNum);

			// �Ǘ����𖄂߂�.
			for (size_t i = objNum; i > 0; --i) {
				unUseIdList_.push(i - 1);
			}
			return true;
		}

		// ���g�p�̈�̃|�C���^���g�p���̈�Ƃ��ċL�^���āA���̗̈�̐擪�̃|�C���^��ԋp���܂�.
		bool getUnused(void** ref){
			if (unUseIdList_.size() <= 0) {	// �󂫗̈悪����.
				return false;
			}

			*ref = root_ + (unUseIdList_.pop() * objSize_);	// �󂫗̈�ID�����pop���T�C�Y���|���A���̕�offset�����|�C���^��ԋp�l�ɃZ�b�g.
			return true;
		}

		// �����ɗ^����ꂽ�|�C���^�̗̈�𖢎g�p�̈�Ƃ��ċL�^���܂�.
		bool setUnused(void* point) {
			if (point < root_ || point >= root_ + capacity_) {	//	�̈�O�`�F�b�N.
				return false;
			}

			auto num = (static_cast<char*>(point) - root_) / objSize_;	// �C���f�b�N�X���Z�o.

			auto p = unUseIdList_.begin();
			auto end = unUseIdList_.end();
			auto ret = std::find(p, end, num);	// ���g�p���X�g�ɑ��݂��邩�`�F�b�N.
			if (ret != end) {						// ���ɖ��g�p�Ƃ��ċL�^����Ă���ꍇ�͎��s.
				return false;
			}

			// ���g�p�̈�Ƃ��ċL�^.
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