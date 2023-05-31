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
		auto rot = rotation * PI / 180.0f;								// ‰ñ“]‚ğƒ‰ƒWƒAƒ“‚É•ÏŠ·.
		float x = position.x * cosf(rot) - position.y * sinf(rot);		// xÀ•W‚Ì‰ñ“]ŒvZ@yÀ•W‚ÌŒvZ‚É‰e‹¿‚µ‚¿‚á‚¤‚Ì‚Åˆê’U•Ê‚Ì•Ï”‚Åó‚¯~‚ß‚é.
		position.y = position.x * sinf(rot) + position.y * cosf(rot);	// yÀ•W‚Ì‰ñ“]ŒvZ‚Æ”½‰f.
		position.x = x;													// ‰ü‚ß‚ÄxÀ•W‚ğ”½‰f.
		return position;
	}
}