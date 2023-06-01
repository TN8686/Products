#include "ObjectManager.h"
#include "GameObject.h"

namespace Engine {
	// �C���X�^���X.
	ObjectManager ObjectManager::instance_;

	int ObjectManager::addObjectList(size_t reserveNum)
	{
		/*
		TODO�@���@�󂫂�T������������.
		*/

		// TODO �n���h�����������@����ID�����̂܂ܓn���Ă���.
		int handle = static_cast<int>(objectListSet_.size());
		objectListSet_.emplace_back();
		objectListSet_.at(handle).list.resize(reserveNum, nullptr);

		objectListSet_.at(handle).freeSpaceNum = static_cast<unsigned int>(reserveNum);
		objectListSet_.at(handle).isSorted = true;

		if (renderList_.capacity() < reserveNum) {	// �s�����Ă���Ε`��p�̈���m��.
			renderList_.reserve(reserveNum);
		}

		return handle;
	}

	// �n���h���̃I�u�W�F�N�g���X�g�ɕێ�����Ă���I�u�W�F�N�g��S�Ĕj�����܂�.
	bool ObjectManager::deleteObjectList()
	{
		// �n���h�����L���Ȓl���`�F�b�N.
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		for (auto itr = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;
			itr != objectListSet_.at(handle_).list.end(); ++itr) {
			delete *itr;			// �I�u�W�F�N�g��j��.
		}
		objectListSet_.at(handle_).list.clear();			// �N���A.
		objectListSet_.at(handle_).list.shrink_to_fit();	// �̈�����.

		objectListSet_.at(handle_).freeSpaceNum = 0;

		// TODO handle_�������ɂȂ��������L�^.

		return true;
	}


	// �����̃I�u�W�F�N�g��j�����܂�.
	// TODO ���s�̃R���W�����Ƃ̐����������Ă��Ȃ����߁A�R���W���������������I�u�W�F�N�g��j�����Ȃ��悤�ɂ��Ă�������.
	// �R���W�����֘A�̏C�����I���Ζ����ł�.
	bool ObjectManager::deleteObject(GameObject* obj)
	{
		if (obj == nullptr) {
			return false;
		}

		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		auto first = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;		// �󂫗̈���������͈͂̎n�_.
		auto hitItr = std::lower_bound(first, objectListSet_.at(handle_).list.end(), obj);				// �^�[�Q�b�g�ȏ�̒l��T��.

		if (hitItr == objectListSet_.at(handle_).list.end() || *hitItr != obj) {	// �Ō�܂Ńq�b�g���Ȃ������A�܂��͑Ώۂ������łȂ�.
			return false;
		}

		delete *hitItr;		// �I�u�W�F�N�g��j��.
		*hitItr = nullptr;	// null����.
		++objectListSet_.at(handle_).freeSpaceNum;	// �J�E���^���C���N�������g.

		// TODO �ł���΂����Ń\�[�g���Ȃ��Ă����悤�ɂ��ׂ��ȋC������.
		sort();

		return true;
	}

	bool ObjectManager::load()
	{
		if (!handleCheck(handle_)) {
			return false;
		}
		
		sort();

		// �󂫗̈�̕�offset���đ���.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
				(*itr)->load();
		}

		return true;
	}

	bool ObjectManager::init()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// �󂫗̈�̕�offset���đ���.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->init();
		}

		return true;
	}

	bool ObjectManager::update()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// �󂫗̈�̕�offset���đ���.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->update();
			}
		}

		return true;
	}

	bool ObjectManager::lateUpdate()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// �󂫗̈�̕�offset���đ���.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			if ((*itr)->getActive()) {
				(*itr)->lateUpdate();
			}
		}

		return true;
	}

	bool ObjectManager::render()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// �󂫗̈�����������X�g�𕡐�.
		auto itr = objectListSet_.at(handle_).list.begin() + objectListSet_.at(handle_).freeSpaceNum;
		renderList_.assign(itr, objectListSet_.at(handle_).list.end());

		// z���Ń\�[�g.
		std::stable_sort(renderList_.begin(), renderList_.end(),
			[](const GameObject* a, const GameObject* b) {
				return a->getTransform().getWorldPosition().z < b->getTransform().getWorldPosition().z;
			}
		);

		for (auto& e : renderList_) {
			if (e->getActive() && e->getDrawActive()) {
				e->render();
			}
		}

		return true;
	}

	bool ObjectManager::unload()
	{
		if (!handleCheck(handle_)) {
			return false;
		}

		sort();

		// �󂫗̈�̕�offset���đ���.
		auto& objList = objectListSet_.at(handle_);
		for (auto itr = objList.list.begin() + objList.freeSpaceNum;
			itr != objList.list.end(); ++itr) {
			(*itr)->unload();
		}

		return true;
	}

	// �n���h�����L���Ȃ��̂��ǂ������`�F�b�N���܂�.
	bool ObjectManager::handleCheck(int handle)
	{
		// TODO ������.
		if (handle == -1) {	// -1���������Ȓl�Ƃ��Ĉ���.
			return false;
		}
		return true;
	}

	// ���ݑ��쒆�̃��X�g���\�[�g���܂�.
	void ObjectManager::sort()
	{
		if (!handleCheck(handle_)) {
			return;
		}

		if (!objectListSet_.at(handle_).isSorted) {
			// �|�C���^�Ń\�[�g.
			std::stable_sort(objectListSet_.at(handle_).list.begin(), objectListSet_.at(handle_).list.end(),
				[](const GameObject* a, const GameObject* b) {
					return a < b;
				}
			);
		}
		return;
	}

}