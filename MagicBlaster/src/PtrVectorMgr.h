#pragma once

#include <vector>
#include <memory>
#include <assert.h>

namespace PtrVector {

	// �O���錾.
	class IManaged;

	template <typename T>
	class PtrVectorMgr;

	namespace {
		class IPtrVectorMgrBase;
	}

	/// <summary>
	/// PtrVectorMgr�ɊǗ������I�u�W�F�N�g�̃C���^�[�t�F�[�X.
	/// �p�����鎖��PtrVectorMgr�y�т��̔h���N���X�ɊǗ�����鎖���ł���.
	/// IManaged���p���������炩�̔h���N���XCChild�ƁA������Ǘ�����PtrVectorMgr&lt;CChild&gt;�A�܂���PtrVectorMgr&lt;CChild&gt;���p�������N���X�̃Z�b�g�ŗ��p����.
	/// </summary>
	class IManaged {
		friend IPtrVectorMgrBase;		// �t�����h�N���X�Ƃ��鎖��IPtrVectorMgrBase����΃A�N�Z�X�ł���悤��.

	private:
		// private�����o�ɂ��Čp�������N���X����͒l�ɃA�N�Z�X�ł��Ȃ��悤��.
		IPtrVectorMgrBase* _pManager;	// �Ǘ��҂̃|�C���^.
		bool _isRegistered;				// �o�^�ς݃t���O.
		size_t _index;					// �Ǘ�����Ă��郊�X�g��ł̃C���f�b�N�X.
		int _priority;					// �\�[�g�D��x�i���݂̓\�[�g�������j.

	protected:
		// ������Z�q�@protected�����o�ɂ��鎖�ŁA�p�����������^�̃I�u�W�F�N�g���m�̑���łȂ���ΎQ�Ƃł��Ȃ����Ă���B
		IManaged& operator=(const IManaged& obj);

		// ���ɁAIManaged���p�������h���N���XCChild�ő�����Z�q�𖾎��I�ɒ�`�������ꍇ�́A�ȉ��̂悤��PtrVector::IManaged::operator=()���Ăяo���Ă�������.
		/*
		CChild& operator=(const CChild& obj) {
			PtrVector::IManaged::operator=(obj);	// �����I��PtrVector::IManaged::operator=()���Ăяo��.

			// �h���N���X�Ǝ��̏���.

			return *this;
		};
		*/

	public:
		explicit IManaged()
			: _pManager(nullptr)
			, _isRegistered(false)
			, _index(SIZE_MAX)			// ��o�^���̃C���f�b�N�X�͍ő�ɂ��Ă���.
			, _priority(0)
		{}

		// �R�s�[�R���X�g���N�^�@�Öق̌^�ϊ����֎~.
		explicit IManaged(const IManaged& obj);

		// �f�X�g���N�^.
		~IManaged() {
			Unregister();	// �I�u�W�F�N�g�̔j�����Ɏ��g�̓o�^�������ŉ���.
		};

		// �\�[�g�D��x���擾���܂�.
		const int& GetPriority() { return _priority; }

		// �\�[�g�D��x��ݒ肵�܂�.
		void SetPriority(const int& priority) { _priority = priority; }

		// �Ǘ��҂��o�^����Ă���ꍇ�A���g�̓o�^���������܂�.
		bool Unregister();
	};


	// �������O���.
	namespace {
		/// <summary>
		/// IManaged��PtrVectorMgr���Q�Ƃ��APtrVectorMgr��IManaged�̒l�����������邽�߂̒���C���^�[�t�F�[�X.
		/// �������鎖��IManaged���e���v���[�g�N���X�ł���K�v�������Ȃ���.
		/// </summary>
		class IPtrVectorMgrBase {
			friend IManaged;	// IManaged����̂݃A�N�Z�X�\��.
		protected:
			// ���ۂɓo�^���s�����߂̊֐����`.
			virtual bool Register(IManaged* pManaged) = 0;

			// ���ۂɓo�^�������s�����߂̊֐����`.
			virtual bool Unregister(IManaged* pManaged) = 0;

			/// IManaged�̊e�l�����̃C���^�[�t�F�[�X����ď��������\��.
			IPtrVectorMgrBase*& rpGetManager(IManaged* obj) { return obj->_pManager; }
			bool& rGetIsRegistered(IManaged* obj) { return obj->_isRegistered; }
			size_t& rGetIndex(IManaged* obj) { return obj->_index; }
			int& rGetPriority(IManaged* obj) { return obj->_priority; }

		};
	}


	/// <summary>
	/// IManaged�y�т��̔h���N���X���Ǘ�����@�\�����e���v���[�g�N���X.
	/// IManaged���p���������炩�̔h���N���XCChild�ƁA������Ǘ�����PtrVectorMgr&lt;CChild&gt;�A�܂���PtrVectorMgr&lt;CChild&gt;���p�������N���X�̃Z�b�g�ŗ��p����.
	/// �ΏۂƂ��Ă���IManaged���p�������N���X�̃I�u�W�F�N�g��Register�֐��œo�^����Ƃ��̃|�C���^�����X�g����.
	/// �o�^����Ă���I�u�W�F�N�g���j�����ꂽ���ɂ͎����I�Ƀ��X�g����o�^���������܂��B
	///
	/// TODO �\�[�g�@�\�̎����@���@�_�~�[�I�u�W�F�N�g�������ɗ���悤�Ɂ@�����A�~���ɍ��킹�\�[�g�O�Ƀ_�~�[�̃v���C�I���e�B��ύX����H�@�|�C���^�𒼂Ƀ_�~�[�Ɣ�r����H.
	/// TODO �K�x�[�W�R���N�V�����̎����@���@�\�[�g�㖖���̗v�f���폜���Ă����Ashrink_to_fit().
	/// </summary>
	/// <typeparam name="T">IManaged���p�������N���X</typeparam>
	template <typename T>
	class PtrVectorMgr : public IPtrVectorMgrBase {
	private:
		std::vector<T*> _ptrList;				// �Ǘ��ΏۃI�u�W�F�N�g�̃|�C���^���X�g.
		std::vector<std::size_t> _freeIDList;	// _ptrList���́A�g�p��ɃI�u�W�F�N�g���j������ċ󂢂��C���f�b�N�X�̃��X�g.
		T _dummy;								// ���g�p�C���f�b�N�X�̗v�f�Ƃ��Ďg�p����_�~�[�I�u�W�F�N�g 
												// TODO ���̂܂܂��ƃf�t�H���g�R���X�g���N�^���Ȃ��N���X�Ŏg�p�ł��Ȃ�����Ȃ�炩�̕ύX���K�v�@�\�[�g��������Ǝ�ԂɂȂ邯�ǃ_�~�[����Ȃ�nullptr�����H.
												//		�ǂ݂̂���������悤�ȏ�����ǉ�����Ȃ牽�炩�̌`��dummy�Ƃ̔�r���K�v�ɂȂ邵�Anull�`�F�b�N�ł������̂ł�.

	protected:
		bool Register(IManaged* obj) override;
		bool Unregister(IManaged* obj) override;

	public:
		PtrVectorMgr()
		{
			rGetPriority(&_dummy) = INT_MAX;		// �_�~�[�̃\�[�g�D��x���ЂƂ܂��Œ��.
		};

		/// <summary>
		/// �����̃��X�g��reserve���s���܂�.
		/// </summary>
		/// <param name="reserveNum">�m�ۂ������̈�̐�</param>
		PtrVectorMgr(const size_t& reserveNum) {
			rGetPriority(&_dummy) = INT_MAX;		// �_�~�[�̃\�[�g�D��x���ЂƂ܂��Œ��.
			Reserve(reserveNum);					// ���U�[�u.
		}
		~PtrVectorMgr();

		// �������X�g�̃|�C���^���擾���܂�.
		std::vector<T*>* pGetList() { return &_ptrList; }
		const std::vector<T*>* pGetList() const { return &_ptrList; }

		// �������֎~.
		PtrVectorMgr(const PtrVectorMgr&) = delete;
		const PtrVectorMgr& operator=(const PtrVectorMgr&) = delete;

		// �����̃��X�g��reserve���s���܂�.
		void Reserve(const size_t& reserveNum) {
			_ptrList.reserve(reserveNum);
			_freeIDList.reserve(reserveNum);
		}

		// �Ǘ��I�u�W�F�N�g��o�^���܂�.
		bool Register(T* obj) {
			return Register(static_cast<IManaged*>(obj));	// IManaged*�������Ƃ�����̊֐��ɔC���� ���̂܂ܓn���ƍċA�ɂȂ��Ă��܂��̂Ŗ����I�ɃL���X�g.
		}

		// �Ǘ��I�u�W�F�N�g��o�^�������܂�.
		bool Unregister(T* obj) {
			return Unegister(static_cast<IManaged*>(obj));	// IManaged*�������Ƃ�����̊֐��ɔC���� ���̂܂ܓn���ƍċA�ɂȂ��Ă��܂��̂Ŗ����I�ɃL���X�g.
		}
	};


	// IPtrVectorMgr

	// �f�X�g���N�^.
	template <typename T>
	PtrVectorMgr<T>::~PtrVectorMgr()
	{
		if (_freeIDList.size() != _ptrList.size()) {	// �t���[ID�̐������X�g�̃T�C�Y�ƈ�v���Ă���ꍇ�A�S�ċ�̂͂��Ȃ̂ŉ������Ȃ�.
			for (auto& e : _ptrList) {					// �S�Ă̊Ǘ��Ώۂɑ΂��ēo�^������ʒm.
				e->Unregister();
			}
		}
	}

	// �o�^.
	// ������IManaged*��T*�ɃL���X�g���Ă��镔�������邪�A
	// ���̊֐���protected�̂��߁A�O������͈�����T*�̊֐�����Ă����Ăяo����.
	// ��������͓o�^�ς݂�IManaged���炵���Ăяo���Ȃ����߁A�o�^���������S�Ȃ���S�Ȃ͂�.
	template <typename T>
	bool PtrVectorMgr<T>::Register(IManaged* obj) {
		// �e�p�����[�^�̎Q�Ƃ����.
		auto& rpManager = rpGetManager(obj);
		auto& rIsRegistered = rGetIsRegistered(obj);
		auto& rIndex = rGetIndex(obj);

		// �o�^�ς݂̏ꍇ�A�Ǘ��҂����邩���`�F�b�N.
		if (rIsRegistered) {
			if (rpManager == this) {						// �Ǘ��҂����g�������牽�����Ȃ�.
				return false;
			}
			else {											// �Ǘ��҂����g�łȂ�������.
				assert(rpManager != nullptr);				// ������null�ɂ͂Ȃ�Ȃ��͂�.
				obj->Unregister();							// �o�^������.
			}
		}

		// �o�^����.
		if (_freeIDList.empty()) {							// ���g�p�C���f�b�N�X���X�g����̏ꍇ.
			rIndex = _ptrList.size();						// �C���f�b�N�X��ێ��iemplace_back�̌�Ȃ�size() - 1���ǉ������C���f�b�N�X�@�����Ȃ�distance���g��Ȃ��Ă��ǂ��j.
			_ptrList.emplace_back(static_cast<T*>(obj));	// ���X�g�Ƀ|�C���^��o�^.
		}
		else {												// ���g�p�C���f�b�N�X������ꍇ.
			rIndex = _freeIDList.back();					// ���g�p�̃C���f�b�N�X�����pop.
			_freeIDList.pop_back();
			_ptrList.at(rIndex) = static_cast<T*>(obj);		// ���X�g�Ƀ|�C���^��o�^.
		}

		// ���g���Ǘ��҂ɓo�^���o�^�ς݂�.
		rpManager = this;
		rIsRegistered = true;

		return true;
	}

	// �o�^����.
	template <typename T>
	bool PtrVectorMgr<T>::Unregister(IManaged* obj) {
		
		// �C���f�b�N�X�̎Q�Ƃ����.
		auto& rIndex = rGetIndex(obj);

		// �s��index�`�F�b�N.
		assert(!(rIndex < 0 || rIndex >= _ptrList.size()));
		if (rIndex < 0 || rIndex >= _ptrList.size()) {
			return false;
		}

		// ���X�g����폜.
		_ptrList.at(rIndex) = &_dummy;		// �g�p���Ă����C���f�b�N�X�̗v�f���_�~�[��.
		_freeIDList.emplace_back(rIndex);	// �g�p���Ă����C���f�b�N�X�𖢎g�p���X�g�ɓo�^.
		rGetIsRegistered(obj) = false;		// �o�^����.
		rpGetManager(obj) = nullptr;		// �Ǘ��҂�nullptr��.

		return true;
	}
}