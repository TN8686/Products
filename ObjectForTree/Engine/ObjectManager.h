#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace Engine {
	class GameObject;
	struct Transform;

	class ObjectManager {
		struct ObjectListSet {
			std::vector<GameObject*> list;
			unsigned int freeSpaceNum;		// ���g�p�̈�J�E���^.
			bool isSorted;					// �\�[�g�ς݃t���O.

			ObjectListSet()
				: freeSpaceNum(0)
				, isSorted(false)
			{}
		};

		// �����o�ϐ�.
	private:
		// �V���O���g��.
		static ObjectManager instance_;

		std::vector<ObjectListSet> objectListSet_;	// �Ǘ����Ă���I�u�W�F�N�g�̃��X�g.
		std::vector<GameObject*> renderList_;	// �`�惊�X�g�̈�.

		int handle_;	//	���ݑ��쒆�̃��X�g�̃n���h��.


		// �R���X�g���N�^.
	private:
		ObjectManager()
			: handle_(-1)
		{};

		// �f�X�g���N�^.
	private:
		~ObjectManager() {
			// TODO �Œ�ł��I�u�W�F�N�g�j���֐������s����悤�ɂ���.�@
		}

		// ����.
	public:
		static ObjectManager* getInstance() { return &instance_; }

		// �V���ȃI�u�W�F�N�g���X�g�̈��ǉ����A���̃n���h����Ԃ��܂�.
		int addObjectList(size_t reserveNum = 256);

		// �n���h���̃I�u�W�F�N�g���X�g�ɕێ�����Ă���I�u�W�F�N�g��S�Ĕj�����܂�.
		bool deleteObjectList();
		
		// �I�u�W�F�N�g��j�����܂�.
		// TODO ���s�̃R���W�����Ƃ̐����������Ă��Ȃ����߁A�R���W���������������I�u�W�F�N�g��j�����Ȃ��悤�ɂ��Ă�������.
		// �R���W�����֘A�̏C�����I���Ζ����ł�.
		bool deleteObject(GameObject* obj);

		// ���X�g�ɐV���ȃI�u�W�F�N�g��ǉ����܂�.
		template <class T>
		bool createObject(const T& copyObject, GameObject** ref = nullptr);
		template <class T>
		bool createObject(const T& copyObject, T** ref = nullptr);

		// TODO ���O����I�u�W�F�N�g������.
		//bool getObjectByName(int handle_, const std::string& name, GameObject* ref);
		//bool getObjectByName(int handle_, size_t& hash, GameObject* ref);

		bool load();
		bool init();
		bool update();
		bool lateUpdate();
		bool render();
		bool unload();

	private:
		// �n���h�����L���Ȃ��̂��ǂ������`�F�b�N���܂�.
		bool handleCheck(int handle_);

		// ���ݑ��쒆�̃��X�g���\�[�g���܂�.
		void sort();

		// ����.
	public:

		// �I�u�W�F�N�g���X�g���擾���܂��@TODO �����Ă��������낤���c�@�Ȃ�ɂ����U����`�F�b�N�p��.
		bool getObjectList(std::vector<GameObject*>* ref) {
			if (!handleCheck(handle_)) {
				return false;
			}

			ref = &objectListSet_.at(handle_).list;
		}

		// ���ݑ��쒆�̃n���h����Ԃ��܂�.
		int getHandle() { return handle_; }

		// ���삷�郊�X�g�̃n���h����ݒ肵�܂�.
		bool setHandle(int handle) {
			if (!handleCheck(handle)) {
				return false;
			}
			handle_ = handle;
			return true;
		}
	};

	template <class T>
	bool ObjectManager::createObject(const T& copyObject, T** ref) {

		// �n���h�����L���Ȓl���`�F�b�N.
		if (!handleCheck(handle_)) {
			return false;
		}

		auto p = new T(copyObject);
		auto& listSet = objectListSet_.at(handle_);
		if (listSet.freeSpaceNum > 0) {		// �󂫂�����ꍇ.
			--listSet.freeSpaceNum;			// �J�E���^���f�N�������g.
			listSet.list.at(listSet.freeSpaceNum) = p;	// �󂫗̈�̒[���g�p.
		}
		else {
			listSet.list.emplace_back(p);	// �󂫂��Ȃ��ꍇ�A�̈��ǉ����I�u�W�F�N�g�𐶐�.
		}

		listSet.isSorted = false;	// ���\�[�g���.

		p->load();

		if (ref != nullptr) {
			*ref = p;
		}

		return true;
	}

	template <class T>
	bool ObjectManager::createObject(const T& copyObject, GameObject** ref) {
		T* p = nullptr;	// �ԋp�l�󂯎��p.

		if (!createObject(copyObject, &p)) {	// �ԋp�l��T*�Ŏ󂯎��֐��ɈϏ�.
			return false;
		}

		if (ref != nullptr) {
			*ref = p;
		}
		return true;
	}
}