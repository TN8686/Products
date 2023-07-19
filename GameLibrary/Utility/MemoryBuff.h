#pragma once
#include <algorithm>

#include "MyArray.h"

namespace Utility {

	////////////////////////////////////
	// �z�unew�p�̃��������m�ۂ���N���X
	////////////////////////////////////
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
		size_t capacity() {
			return capacity_;
		}

		// �ŏ��̃|�C���^��Ԃ��܂�.
		char* begin() {
			return root_;
		}

		// �����i�Ō�̗v�f�̎��j�̃|�C���^��Ԃ��܂�.
		char* end() {
			return &root_[capacity_];
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

	// MemoryPool��ŃI�u�W�F�N�g��j�����邽�߂̃C���^�[�t�F�[�X�N���X.
	class IMemoryPool {
	public:
		virtual ~IMemoryPool() = default;
	};

	////////////////////////////////////////////////////////////////////
	// ���܂����o�C�g���P�ʂŔz�unew�p�̃������v�[�����Ǘ�����N���X.
	// IMemoryPool���p�����Ă���I�u�W�F�N�g�ɂ̂ݑΉ����Ă��܂�.
	// 
	// �g�p���@.
	// �ŏ���init�ŗ̈���m�ۂ��Aplacement�ŃI�u�W�F�N�g�𐶐��A�z�u�ł��܂�.
	// removal�Ŕz�u�����I�u�W�F�N�g��j���ł��܂�.
	// ���̃I�u�W�F�N�g��j�����邩�Arefresh�őS�ẴI�u�W�F�N�g��j�����A�m�ۂ��Ă�����������������܂�.
	////////////////////////////////////////////////////////////////////
	class MemoryPool : protected MemoryBuff {
	private:
		MyArray<size_t> freeIdList_;	// ���g�p�C���f�b�N�X�̃��X�g.
		size_t objSize_;				// �I�u�W�F�N�g�������̃o�C�g��.

	public:
		MemoryPool()
			: objSize_(0)
		{}

		~MemoryPool() override {
			refresh();	// �����������.
		}

		// ����.
	public:
		/// <summary>
		/// �̈���m�ۂ��܂�.
		/// </summary>
		/// <param name="areaNum">�̈�̐�</param>
		/// <param name="objSize">�̈�������̃o�C�g��</param>
		/// <returns></returns>
		bool init(size_t areaNum, size_t objSize);

		/// <summary>
		/// ���g�p�̈�̈�ɃI�u�W�F�N�g�𐶐��A�z�u���A
		/// ���̗̈��̈�Ƃ��ċL�^��A�|�C���^��ԋp���܂�.
		/// </summary>
		/// <typeparam name="T"></typeparam>
		/// <typeparam name="...Args"></typeparam>
		/// <param name="ref">�|�C���^�ԋp�p�@�܂��A���̈����̌^�̃I�u�W�F�N�g�𐶐����܂�</param>
		/// <param name="...args">�z�u����I�u�W�F�N�g�̃R���X�g���N�^�ɓn������</param>
		/// <returns></returns>
		template<typename T, typename... Args>
		bool placement(T*& ref, Args&&... args);

		// �����ɗ^����ꂽ�|�C���^�̃I�u�W�F�N�g��j�����A�̈�𖢎g�p�̈�Ƃ��ċL�^���܂�.
		template<typename T>
		bool removal(T*& removal_pointer);

		// �̈�̑��o�C�g����Ԃ��܂�.
		size_t capacity() {
			return MemoryBuff::capacity();
		}

		// ���v�f����Ԃ��܂�.
		size_t size() {
			if (objSize_ <= 0) {
				return 0;
			}
			return MemoryBuff::capacity() / objSize_;
		}

		// �󂫗v�f����Ԃ��܂�.
		size_t freeCount() {
			return freeIdList_.size();
		}

		// �I�u�W�F�N�g��S�Ĕj�����A�m�ۂ��Ă��郁������������܂�.
		void refresh() override;
	};

	/// <summary>
	/// ���g�p�̈�̈�ɃI�u�W�F�N�g�𐶐��A�z�u���A
	/// ���̗̈��̈�Ƃ��ċL�^��A�|�C���^��ԋp���܂�.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="...Args"></typeparam>
	/// <param name="ref">�|�C���^�ԋp�p�@�܂��A���̈����̌^�̃I�u�W�F�N�g�𐶐����܂�</param>
	/// <param name="...args">�z�u����I�u�W�F�N�g�̃R���X�g���N�^�ɓn������</param>
	/// <returns></returns>
	template<typename T, typename ...Args>
	bool MemoryPool::placement(T*& ref, Args && ...args) {

		// IMemoryPool���p�����Ă��Ȃ��ꍇ�͊֐��������ł����R���p�C���G���[.
		{
			IMemoryPool* check = ref;
		}

		// �󂫗̈悪������Ύ��s.
		if (freeIdList_.size() <= 0) {
			return false;
		}

		// �I�u�W�F�N�g�̃o�C�g������v�f�̃o�C�g�����傫���Ǝ��s.
		if (sizeof(T) > objSize_) {
			return false;
		}


		// �z�u��̃|�C���^���擾.
		auto p = &root_[freeIdList_.pop() * objSize_];	// �󂫗̈�ID�����pop���A����ID�̕��I�t�Z�b�g�����|�C���^���Z�o.

		// �I�u�W�F�N�g��z�u���āA�|�C���^��ԋp�l�ɃZ�b�g.
		ref = new(p) T(std::forward<Args>(args)...);
		return true;
	}

	// �����ɗ^����ꂽ�|�C���^�̃I�u�W�F�N�g��j�����A�̈�𖢎g�p�̈�Ƃ��ċL�^���܂�.
	template<typename T>
	bool MemoryPool::removal(T*& removal_pointer)
	{

		// IMemoryPool���p�����Ă��Ȃ��ꍇ�͊֐��������ł����R���p�C���G���[.
		{
			IMemoryPool* type_check = removal_pointer;
		}

		// �|�C���^���Z�̂��߁Achar*�^�Ƃ��ĉ���.
		char* p = reinterpret_cast<char*>(removal_pointer);

		// �̈�O�`�F�b�N.
		if (p < root_ || p >= root_ + capacity_) {
			return false;
		}

		// �m�ۗ̈�������Ǌe�̈�̐擪����Ȃ������玸�s.
		if ((p - root_) % objSize_ != 0) {
			return false;
		}

		// �C���f�b�N�X���Z�o.
		auto id = (p - root_) / objSize_;

		// ���ɖ��g�p�̈惊�X�g�ɑ��݂��Ă����玸�s.
		auto begin = freeIdList_.begin();
		auto end = freeIdList_.end();
		if (std::find(begin, end, id) != end) {
			return false;
		}

		// �f�X�g���N�^�����s.
		removal_pointer->~T();

		// nullptr����.
		removal_pointer = nullptr;

		// ���g�p�̈�Ƃ��ċL�^.
		freeIdList_.push(id);

		return true;
	}
}