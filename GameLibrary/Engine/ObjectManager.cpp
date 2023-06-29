#include "ObjectManager.h"
#include "GameObject.h"

namespace Engine {

	using namespace std;
	using namespace Utility;

	// �C���X�^���X.
	ObjectManager ObjectManager::instance_;

	ObjectManager::ObjectManager()
		: key_("")
		, keyHash_(HASH_DIGEST(""))
		, isDrawListUpdate(true)
	{}

	ObjectManager::~ObjectManager()
	{
		// �ň����X�g�̑S�폜�͍s��.

		MyArray<uint32_t> keyList(objectListSet_.size());

		// �S�ẴI�u�W�F�N�g���X�g��j��.
		for (const auto& e : objectListSet_) {
			keyList.push(e.first);
		}

		for (const auto& e : keyList) {
			keyHash_ = e;
			deleteObjectList();
		}
	}

	bool ObjectManager::addObjectList(string key, size_t reserveNum)
	{
		auto hash = Hash::GetDigest(key.c_str(), key.length());
		if (hash == HASH_DIGEST("")) {
			return false;
		}

		auto pair = objectListSet_.try_emplace(hash);			// �V���ȗ̈���쐬.
		if (!pair.second)
		{
			return false;										// �V���ȗ̈悪�쐬�ł��Ȃ������玸�s.
		}
		heapList_.try_emplace(hash);							// �q�[�v�ナ�X�g�ɂ��V���ȗ̈���쐬.

		pair.first->second.list.resize(reserveNum, nullptr);	// �I�u�W�F�N�g�̗̈�.
		objectListSet_.at(hash).freeSpaceNum = static_cast<unsigned int>(reserveNum);	// �󂫗̈搔���L�^.
		objectListSet_.at(hash).isSorted = true;											// �\�[�g�ς݃t���O.


		if (renderList_.capacity() < reserveNum) {	// �s�����Ă���Ε`��p�̈���m��.
			renderList_.reserve(reserveNum);
		}

		return true;
	}

	// ���ݎg�p���̃��X�g��j�����܂�.
	bool ObjectManager::deleteObjectList()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		//unload();	// �ێ����Ă���I�u�W�F�N�g��S��unload.

		// �j��.
		objectListSet_.erase(keyHash_);
		addList_.clear();
		deleteIdList_.clear();

		for (auto& e : heapList_.at(keyHash_)) {
			delete e;
		}
		heapList_.at(keyHash_).refresh();

		key_ = "";
		keyHash_ = HASH_DIGEST("");

		return true;
	}

	// �����̃I�u�W�F�N�g�𐶐��҂����X�g�ɒǉ����܂�.
	bool ObjectManager::createObject(GameObject* pObject)
	{
		if (keyHash_ == HASH_DIGEST("") || pObject == nullptr) {
			return false;
		}

		addList_.emplace_back(pObject);

		return true;
	}

	// �����̃I�u�W�F�N�g��j���҂����X�g�ɒǉ����܂�.
	// CreateObjectOnHeap�Ő������ꂽ�I�u�W�F�N�g�������ꍇ�̓�������������܂�.
	bool ObjectManager::deleteObject(GameObject* obj)
	{
		if (obj == nullptr) {
			return false;
		}

		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto hitItr = std::find(addList_.begin(), addList_.end(), obj);// �҂����X�g���`�F�b�N.
		if (hitItr != addList_.end()) {	// �ǉ��҂����X�g�ɑ��݂��Ă���.
			*hitItr = nullptr;			// �ǉ����Ȃ��悤��nullptr��.

			// ����ɁACreateObjectOnHeap�Ő������Ă����ꍇ��delete����.
			auto& heapList = heapList_.at(keyHash_);
			auto end = heapList.end();
			auto pHit = std::find(heapList.begin(), end, obj);	// heapList_�ɂ��邩���`�F�b�N.
			if (pHit != end) {		// ���݂�����.
				delete *pHit;		// �̈�����.
				*pHit = *(end - 1);	// ���̈ʒu�ɍŌ�̗v�f���R�s�[.
				heapList.pop();		// pop���鎖�ōŌ�̗v�f��j��.
			}

			return true;				// �I��.
		}
	
		auto listSet = objectListSet_.at(keyHash_);
		auto first = listSet.list.begin() + listSet.freeSpaceNum;						// �󂫗̈���������͈͂̎n�_.
		hitItr = std::lower_bound(first, listSet.list.end(), obj);						// �^�[�Q�b�g�ȏ�̒l��T��.

		if (hitItr == listSet.list.end() || *hitItr != obj) {							// �Ō�܂Ńq�b�g���Ȃ������A�܂��͑Ώۂ������łȂ�.
			return false;
		}

		(*hitItr)->setActive(false);		// ��A�N�e�B�u��.

		deleteIdList_.emplace_back(std::distance(listSet.list.begin(), hitItr));	// �C���f�b�N�X���o���ă��X�g�ɒǉ�.

		return true;
	}

	bool ObjectManager::init()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// �󂫗̈�̕��Aoffset���đ���.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			(*itr)->init();
		}

		return true;
	}

	bool ObjectManager::update()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		if (!deleteIdList_.empty()) {
			actuallyDelete();	// �j���҂���j��.
		}
		if (!addList_.empty()) {
			actuallyCreate();	// �ǉ��҂���ǉ�.
		}

		// �󂫗̈�̕��Aoffset���đ���.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->update();
			}
		}

		return true;
	}

	bool ObjectManager::lateUpdate()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// �󂫗̈�̕��Aoffset���đ���.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto itr = listSet.list.begin() + listSet.freeSpaceNum;
			itr != listSet.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->lateUpdate();
			}
		}

		return true;
	}

	bool ObjectManager::render()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		if (isDrawListUpdate) {
			// �󂫗̈�����������X�g�𕡐�.
			auto itr = objectListSet_.at(keyHash_).list.begin() + objectListSet_.at(keyHash_).freeSpaceNum;
			renderList_.assign(itr, objectListSet_.at(keyHash_).list.end());
			isDrawListUpdate = false;
		}

		// z���Ń\�[�g.
		std::stable_sort(renderList_.begin(), renderList_.end(),
			[](const GameObject* a, const GameObject* b) {
				
				if (a->getDrawLayer() < b->getDrawLayer()) {		// �`�惌�C���[����.
					return true;
				}
				else if(a->getDrawLayer() == b->getDrawLayer()) {	// ������������.
					return a->getTransform().getWorldPosition().z < b->getTransform().getWorldPosition().z;	// Z���W����.
				}
				return false;
			}
		);

		// �`��.
		for (auto& e : renderList_) {
			if (e->getActive() && e->getDrawActive()) {
				e->render();
			}
		}

		return true;
	}

	bool ObjectManager::unload()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// �󂫗̈�̕��Aoffset���đ���.
		auto& objList = objectListSet_.at(keyHash_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->unload();
		}

		return true;
	}

	// ���ݑ��쒆�̃��X�g���\�[�g���܂�.
	void ObjectManager::sort()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return;
		}

		if (!objectListSet_.at(keyHash_).isSorted) {
			// �|�C���^�Ń\�[�g.
			std::stable_sort(objectListSet_.at(keyHash_).list.begin(), objectListSet_.at(keyHash_).list.end(),
				[](const GameObject* a, const GameObject* b) {
					return a < b;
				}
			);
		}
		return;
	}

	// �����҂����X�g�����Ɏ��ۂɃI�u�W�F�N�g��ǉ����܂�.
	bool ObjectManager::actuallyCreate()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		// �O�����Ĉ�ĂɃ��[�h.
		for (auto& e : addList_) {
			if (e == nullptr) {
				continue;
			}
			e->load();
		}

		// �ǉ���������.
		auto& listSet = objectListSet_.at(keyHash_);
		for (auto& e : addList_) {
			if (e == nullptr) {
				continue;
			}
			if (listSet.freeSpaceNum > 0) {						// �󂫂�����ꍇ.
				--listSet.freeSpaceNum;							// �J�E���^���f�N�������g.
				listSet.list.at(listSet.freeSpaceNum) = e;		// �󂫗̈�̒[���g�p.

				listSet.isSorted = false;	// ���\�[�g���.
			}
			else {
				listSet.list.emplace_back(e);	// �󂫂��Ȃ��ꍇ�A�̈��ǉ����I�u�W�F�N�g�𐶐�.

				listSet.isSorted = false;	// ���\�[�g���.
			}

			e->init();
		}
		addList_.clear();

		sort();
		isDrawListUpdate = true;

		return true;
	}

	bool ObjectManager::actuallyDelete()
	{
		// �L�[���L���Ȓl���`�F�b�N.
		if (keyHash_ == HASH_DIGEST("")) {
			return false;
		}

		auto& listSet = objectListSet_.at(keyHash_);

		auto& heapList = heapList_.at(keyHash_);
		auto begin = heapList.begin();
		auto end = heapList.end();
		for (auto& id : deleteIdList_) {
			auto& p = listSet.list.at(id);	// �C���f�b�N�X����Ώۂ̃I�u�W�F�N�g���擾.
			//p->unload();

			auto pHit = std::find(begin, end, p);	// heapList_�ɂ��邩���`�F�b�N.
			if (pHit != end) {		// ���݂�����.
				delete *pHit;		// �̈�����.
				*pHit = *(end - 1);	// ���̈ʒu�ɍŌ�̗v�f���R�s�[.
				heapList.pop();		// pop���鎖�ōŌ�̗v�f��j��.
			}

			p = nullptr;	// nullptr����.

		}
		listSet.freeSpaceNum += static_cast<unsigned int>(deleteIdList_.size());	// �J�E���^�𑝂₷.

		deleteIdList_.clear();
		listSet.isSorted = false;
		sort();	// �\�[�g.
		isDrawListUpdate = true;

		return false;
	}

	bool ObjectManager::setKey(std::string key)
	{
		auto hash = Hash::GetDigest(key.c_str(), key.length());
		if (!objectListSet_.contains(hash)) {	// ���݂��Ȃ������玸�s.
			return false;
		}

		if (!deleteIdList_.empty()) {
			actuallyDelete();	// �j���҂���j��.
		}
		if (!addList_.empty()) {
			actuallyCreate();	// �ǉ��҂���ǉ�.
		}

		key_ = key;
		keyHash_ = hash;
		isDrawListUpdate = true;
		return true;
	}

}