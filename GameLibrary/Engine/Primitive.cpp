#include <Math.h>
#include "Primitive.h"

namespace {
	static constexpr float PI = 3.141592f;
}

namespace Engine {
	Vector2::operator Vector3() { return Vector3(x, y, 0); }

	Vector3::operator Vector2() { return Vector2(x, y); }

	Vector4::operator Vector3() { return Vector3(x, y, z); }

	AABB2D::operator Rect() const
	{
		return Rect({ position.x - halfLength.x, position.y - halfLength.y },
					{ position.x + halfLength.x, position.y + halfLength.y });
	}

	// ‰ñ“]@Šp“x”Å.
	Vector2 RotationZ(const Vector2& position, const float& rotation)
	{
		Vector2 ret;
		auto rot = rotation * PI / 180.0f;								// ‰ñ“]‚ğƒ‰ƒWƒAƒ“‚É•ÏŠ·.
		ret.x = position.x * cosf(rot) - position.y * sinf(rot);		// xÀ•W‚Ì‰ñ“]ŒvZ.
		ret.y = position.x * sinf(rot) + position.y * cosf(rot);		// yÀ•W‚Ì‰ñ“]ŒvZ.
		return ret;
	}

	// ‰ñ“]@ƒ‰ƒWƒAƒ“”Å.
	Vector2 RotationZrad(const Vector2& position, const float& rad)
	{
		Vector2 ret;
		ret.x = position.x * cosf(rad) - position.y * sinf(rad);		// xÀ•W‚Ì‰ñ“]ŒvZ.
		ret.y = position.x * sinf(rad) + position.y * cosf(rad);		// yÀ•W‚Ì‰ñ“]ŒvZ.
		return ret;
	}
}