#include "GameObject.h"



namespace Engine {

	// Transform
	// 親を辿り、ワールド座標を返します.
	Float3 Transform::getWorldPosition() const {
		auto pos = position;
		if (parent_ != nullptr) {
			auto ptf = parent_->getTransform();				// 親オブジェクトの情報を取得.

			pos = pos * ptf.getWorldScale();				// スケールを反映.

			pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

			pos = pos + ptf.getWorldPosition();				// 親のワールド座標を反映.
		}
		return pos;
	};

	// 親を辿ったrotationを返します.
	Float3 Transform::getWorldRotation() const {
		Float3 rot = rotation;
		if (parent_ != nullptr) {
			rot = rot + parent_->getTransform().getWorldRotation();
		}
		return rot;
	};

	// 親を辿ったscaleを返します.
	Float3 Transform::getWorldScale() const {
		Float3 scl = scale;
		if (parent_ != nullptr) {
			scl = scl * parent_->getTransform().getWorldScale();
		}
		return scl;
	};


	// GameObject
	// 親を辿ったアクティブ状態を返します.
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