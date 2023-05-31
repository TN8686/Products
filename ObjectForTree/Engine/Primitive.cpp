#include <Math.h>
#include "Primitive.h"

namespace {
	static constexpr float PI = 3.141592f;
}

namespace Engine {
	Float2::operator Float3() { return Float3(x, y, 0); }

	Float3::operator Float2() { return Float2(x, y); }

	AABB2D::operator Rect() const
	{
		return Rect({ position.x - halfLength.x, position.y - halfLength.y },
					{ position.x + halfLength.x, position.y + halfLength.y });
	}

	Float2 RotationZ(Float2 position, float rotation)
	{
		auto rot = rotation * PI / 180.0f;								// ��]�����W�A���ɕϊ�.
		float x = position.x * cosf(rot) - position.y * sinf(rot);		// x���W�̉�]�v�Z�@y���W�̌v�Z�ɉe�������Ⴄ�̂ň�U�ʂ̕ϐ��Ŏ󂯎~�߂�.
		position.y = position.x * sinf(rot) + position.y * cosf(rot);	// y���W�̉�]�v�Z�Ɣ��f.
		position.x = x;													// ���߂�x���W�𔽉f.
		return position;
	}
}