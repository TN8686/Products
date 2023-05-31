#include "PtrVectorMgr.h"

namespace PtrVector {
	// IManaged

	// �R�s�[�R���X�g���N�^.
	IManaged::IManaged(const IManaged& obj)
		: _isRegistered(false)
		, _index(SIZE_MAX)
		, _priority(obj._priority)
		, _pManager(obj._pManager)
	{
		if (obj._isRegistered) {			// �Ǘ��҂�������.
			assert(_pManager != nullptr);	// ������null�ɂ͂Ȃ�Ȃ��͂�.
			_pManager->Register(this);		// �����Ǘ��҂ɐV�K�ɓo�^.
		}
	}

	// �o�^����.
	bool IManaged::Unregister() {
		if (_pManager == nullptr) {	// �Ǘ��҂�null��������Ȃɂ����Ȃ�.
			return false;
		}

		_pManager->Unregister(this);	// ���ۂ̓o�^�������̂��̂͊Ǘ��҂ɔC����.
		return true;
	}


	// ������Z�q�@protected�����o�Ƃ��鎖�ŁA�p�������N���X������Ȃ��ƎQ�Ƃł��Ȃ����Ă���.
	// �p�������N���X�̑�����Z�q�̒��ŗ��p���鎖�ŁA�����^�̃I�u�W�F�N�g���m�̑���łȂ���Ύg�p�ł��Ȃ��悤�ɂł���.
	IManaged& IManaged::operator=(const IManaged& obj) {
		Unregister();						// ��U�o�^����.

		_isRegistered = false;				// �ꉞ������.
		_index = SIZE_MAX;					// �ꉞ������.
		_priority = obj._priority;			// �\�[�g�D��x���R�s�[.
		_pManager = obj._pManager;			// �Ǘ��I�u�W�F�N�g����.

		if (obj._isRegistered) {			// �Ǘ��҂�������.
			assert(_pManager != nullptr);	// ������null�ɂ͂Ȃ�Ȃ��͂�.
			_pManager->Register(this);		// �����Ǘ��҂ɐV�K�ɓo�^.
		}
		
		return *this;
	}
	
}