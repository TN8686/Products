#include "GameObject.h"



namespace Engine {

	// Transform
	// �e��H��A���[���h���W��Ԃ��܂�.
	Float3 Transform::getWorldPosition() const {
		auto pos = position;
		if (parent_ != nullptr) {
			auto ptf = parent_->getTransform();				// �e�I�u�W�F�N�g�̏����擾.

			pos = pos * ptf.getWorldScale();				// �X�P�[���𔽉f.

			pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

			pos = pos + ptf.getWorldPosition();				// �e�̃��[���h���W�𔽉f.
		}
		return pos;
	};

	// �e��H����rotation��Ԃ��܂�.
	Float3 Transform::getWorldRotation() const {
		Float3 rot = rotation;
		if (parent_ != nullptr) {
			rot = rot + parent_->getTransform().getWorldRotation();
		}
		return rot;
	};

	// �e��H����scale��Ԃ��܂�.
	Float3 Transform::getWorldScale() const {
		Float3 scl = scale;
		if (parent_ != nullptr) {
			scl = scl * parent_->getTransform().getWorldScale();
		}
		return scl;
	};


	// GameObject
	// �e��H�����A�N�e�B�u��Ԃ�Ԃ��܂�.
	bool GameObject::getActive() const {
		
		if (!isActive_) {
			return false;
		}

		if (transform_.parent_ != nullptr) {
			if (!transform_.parent_->getActive()) {
				return false;
			}
		}

		return true;
	}
}