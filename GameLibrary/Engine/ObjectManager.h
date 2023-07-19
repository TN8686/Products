#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include "../Utility/MyArray.h"
#include "../Utility/hash.h"

namespace Engine {


	class GameObject;
	struct Transform;

	using ObjectArray = Utility::MyArray<GameObject*>;

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

		std::unordered_map<uint32_t, ObjectListSet> objectListSet_;	// �Ǘ����Ă���I�u�W�F�N�g�̃��X�g.
		std::vector<GameObject*> renderList_;		// �`�惊�X�g.
		std::vector<GameObject*> addList_;			// �ǉ��҂����X�g.
		std::vector<size_t> deleteIdList_;			// �j���҂��̃C���f�b�N�X���X�g.
		std::unordered_map<uint32_t, ObjectArray> heapList_;		// ���g���q�[�v��Ɋm�ۂ����I�u�W�F�N�g�̃��X�g.
		std::string key_;							// ���ݑ��쒆�̃��X�g�̃L�[��.
		uint32_t keyHash_;							// ���ݑ��쒆�̃��X�g�̃L�[���̃n�b�V���l.
		bool isDrawListUpdate;						// �`��p���X�g���X�V����K�v�����邩�ǂ���.

		// �R���X�g���N�^.
	private:
		ObjectManager();

		// �f�X�g���N�^.
	private:
		~ObjectManager();

		// ����.
	public:
		static ObjectManager* getInstance() { return &instance_; }

		// �L�[�ɐV���ȃI�u�W�F�N�g���X�g�̈��ǉ����܂��@�L�[���d�����Ă���ꍇ�͎��s���܂�.
		bool addObjectList(std::string key, size_t reserveNum = 256);

		// ���ݎg�p���̃��X�g��j�����܂�.
		bool deleteObjectList();
		
		// �I�u�W�F�N�g��ǉ����܂�.
		bool createObject(GameObject* pObject);
		
		// �q�[�v��ɐV���ȃ��������m�ۂ��A�I�u�W�F�N�g��ǉ����܂�.
		// ���̊֐��Ő������ꂽ�I�u�W�F�N�g�́A�������Ǘ���GameObject���s���܂��B
		template<class T>
		bool createObjectOnHerp(const T& copyObject, T** ref);

		// �q�[�v��ɐV���ȃ��������m�ۂ��A�I�u�W�F�N�g��ǉ����܂�.
		// ���̊֐��Ő������ꂽ�I�u�W�F�N�g�́A�������Ǘ���GameObject���s���܂��B
		template<class T>
		bool createObjectOnHerp(const T& copyObject, GameObject** ref);

		// �I�u�W�F�N�g��j�����܂�.
		bool deleteObject(GameObject* obj);

		// TODO ���O����I�u�W�F�N�g������.
		//bool getObjectByName(const std::string& name, GameObject* ref);
		//bool getObjectByNameHash(size_t& hash, GameObject* ref);

		bool init();
		bool preUpdate();
		bool update();
		bool lateUpdate();
		bool render();
		bool unload();

	private:
		// ���ݑ��쒆�̃��X�g���\�[�g���܂�.
		void sort();

		// �����҂����X�g�����Ɏ��ۂɃI�u�W�F�N�g��ǉ����܂�.
		bool actuallyCreate();

		// �j���҂����X�g�����Ɏ��ۂɃI�u�W�F�N�g��j�����܂�.
		bool actuallyDelete();

		// ����.
	public:

		// �I�u�W�F�N�g���X�g���擾���܂��@TODO �����Ă��������낤���c�@�Ȃ�ɂ����U����`�F�b�N�p��.
		bool getObjectList(std::vector<GameObject*>* ref) {
			ref = &objectListSet_.at(keyHash_).list;
		}

		// ���ݑ��쒆�̃L�[����Ԃ��܂�.
		std::string getKey() { return key_; }

		// ���삷�郊�X�g�̃L�[��ݒ肵�܂�.
		bool setKey(std::string key);
	};

	// �q�[�v��ɐV���ȃ��������m�ۂ��A�I�u�W�F�N�g��ǉ����܂�.
	// ���̊֐��Ő������ꂽ�I�u�W�F�N�g�́A�������Ǘ���GameObject���s���܂��B
	template<class T>
	inline bool ObjectManager::createObjectOnHerp(const T& copyObject, T** ref)
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto p = new T(copyObject);

		heapList_.at(keyHash_).push(p);	// �m�ۂ����I�u�W�F�N�g�̃��X�g�ɓo�^.
		createObject(p);				// �{���X�g�o�^�����͋���.

		if (ref != nullptr) {
			*ref = p;
		}

		return true;
	}

	// �q�[�v��ɐV���ȃ��������m�ۂ��A�I�u�W�F�N�g��ǉ����܂�.
	// ���̊֐��Ő������ꂽ�I�u�W�F�N�g�́A�������Ǘ���ObjectManager���s���܂��B
	template<class T>
	inline bool ObjectManager::createObjectOnHerp(const T& copyObject, GameObject** ref)
	{
		T* p = nullptr;	// �ԋp�l�󂯎��p.

		if (!createObjectOnHerp(copyObject, &p)) {	// �ԋp�l��T*�Ŏ󂯎��֐��ɈϏ�.
			return false;
		}

		if (ref != nullptr) {
			*ref = p;
		}
		return true;
	}
}