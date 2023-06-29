#include "GameObject.h"
#include "../Utility/hash.h"


namespace Engine {

	using namespace Utility;

	// Transform
	// 親を辿り、ワールド座標を返します.
	Vector3 Transform::getWorldPosition() const {
		auto pos = position;
		if (parent != nullptr) {
			auto ptf = parent->getTransform();				// 親オブジェクトの情報を取得.

			pos = pos * ptf.getWorldScale();				// スケールを反映.

			pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

			pos = pos + ptf.getWorldPosition();				// 親のワールド座標を反映.
		}
		return pos;
	};

	// 親を辿ったrotationを返します.
	Vector3 Transform::getWorldRotation() const {
		Vector3 rot = rotation;
		if (parent != nullptr) {
			rot = rot + parent->getTransform().getWorldRotation();
		}
		return rot;
	};

	// 親を辿ったscaleを返します.
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
	// 親を辿ったアクティブ状態を返します.
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