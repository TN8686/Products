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
		auto rot = rotation * PI / 180.0f;								// 回転をラジアンに変換.
		float x = position.x * cosf(rot) - position.y * sinf(rot);		// x座標の回転計算　y座標の計算に影響しちゃうので一旦別の変数で受け止める.
		position.y = position.x * sinf(rot) + position.y * cosf(rot);	// y座標の回転計算と反映.
		position.x = x;													// 改めてx座標を反映.
		return position;
	}
}