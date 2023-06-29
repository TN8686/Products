#include "GameObject.h"
#include "../Utility/hash.h"


namespace Engine {

	using namespace Utility;

	// Transform
	// �e��H��A���[���h���W��Ԃ��܂�.
	Vector3 Transform::getWorldPosition() const {
		auto pos = position;
		if (parent != nullptr) {
			auto ptf = parent->getTransform();				// �e�I�u�W�F�N�g�̏����擾.

			pos = pos * ptf.getWorldScale();				// �X�P�[���𔽉f.

			pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

			pos = pos + ptf.getWorldPosition();				// �e�̃��[���h���W�𔽉f.
		}
		return pos;
	};

	// �e��H����rotation��Ԃ��܂�.
	Vector3 Transform::getWorldRotation() const {
		Vector3 rot = rotation;
		if (parent != nullptr) {
			rot = rot + parent->getTransform().getWorldRotation();
		}
		return rot;
	};

	// �e��H����scale��Ԃ��܂�.
	Vector3 Transform::getWorldScale() const {
		Vector3 scl = scale;
		if (parent != nullptr) {
			scl = scl * parent->getTransform().getWorldScale();
		}
		return scl;
	};

	////////////////
	// GameObject
	////////////////
	GameObject::GameObject(std::string name)
		: isActive_(true)
		, isDraw_(true)
		, lifeTime_(0)
		, name_(name)
		, nameHash_(HASH_DIGEST(name.c_str()))
		, tagHash_(-1)
		, drawLayer_(0)
	{}

	// GameObject
	// �e��H�����A�N�e�B�u��Ԃ�Ԃ��܂�.
	bool GameObject::getActive() const {
		
		if (!isActive_) {
			return false;
		}

		if (transform_.parent != nullptr) {
			if (!transform_.parent->getActive()) {
				return false;
			}
		}

		return true;
	}

	void GameObject::setName(const std::string& name) {
		name_ = name;
		nameHash_ = Hash::GetDigest(name);
	}
	void GameObject::setTag(const std::string& tag) {
		tag_ = tag;
		tagHash_ = Hash::GetDigest(tag);
	}
}