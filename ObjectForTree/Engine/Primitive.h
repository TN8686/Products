#pragma once

namespace Engine {

	// 前方宣言.
	struct Float2;
	struct Float3;
	struct AABB2D;
	struct Rect;

	struct Float2
	{
		float x, y;
		Float2()
			: x(0.0f)
			, y(0.0f)
		{}
		Float2(float x, float y)
			: x(x)
			, y(y)
		{}

		// とりあえずの四則演算.
		Float2 operator +(const Float2& r) const {
			return Float2(x + r.x, y + r.y);
		}

		Float2 operator -(const Float2& r) const {
			return Float2(x - r.x, y - r.y);
		}

		Float2 operator -() const {
			return Float2(x * -1.0f, y * -1.0f);
		}

		Float2 operator *(const Float2& r) const {
			return Float2(x * r.x, y * r.y);
		}

		Float2 operator /(const Float2& r) const {
			return Float2(x / r.x, y / r.y);
		}

		Float2 operator *(float r) const {
			return Float2(x * r, y * r);
		}

		Float2 operator /(float r) const {
			return Float2(x / r, y / r);
		}

		// 変換.
		operator Float3();

	};

	struct Float3
	{
		float x, y, z;
		Float3()
			: x(0.0f)
			, y(0.0f)
			, z(0.0f)
		{}
		Float3(float x, float y, float z)
			: x(x)
			, y(y)
			, z(z)
		{}

		// とりあえずの四則演算.
		Float3 operator +(const Float3& r) const {
			return Float3(x + r.x, y + r.y, z + r.z);
		}

		Float3 operator -(const Float3& r) const {
			return Float3(x - r.x, y - r.y, z - r.z);
		}

		Float3 operator -() const {
			return Float3(x * -1.0f, y * -1.0f, z * -1.0f);
		}

		Float3 operator *(const Float3& r) const {
			return Float3(x * r.x, y * r.y, z * r.z);
		}

		Float3 operator /(const Float3& r) const {
			return Float3(x / r.x, y / r.y, z / r.z);
		}

		Float3 operator *(float r) const {
			return Float3(x * r, y * r, z * r);
		}

		Float3 operator /(float r) const {
			return Float3(x / r, y / r, z / r);
		}

		// 変換.
		operator Float2();
	};

	// 円
	struct Circle2D {
		Float2 position;
		float radius;	// 半径
		Circle2D() : position(0.0f, 0.0f), radius(0.5f) {}
		Circle2D(const Float2& position, float radius)
			: position(position)
			, radius(radius)
		{}
		~Circle2D() {}
	};

	// AABBだが、値の持ち方が違う.
	struct Rect {
		Float2 topLeft, bottomRight;	// top left, bottom right

		Rect() = default;
		Rect(const Float2& topLeft, const Float2& bottomRight)
			: topLeft(topLeft)
			, bottomRight(bottomRight)
		{}
	};

	// 軸並行矩形.
	struct AABB2D
	{
		Float2 position;
		Float2 halfLength;

		AABB2D() = default;
		AABB2D(const Float2& position, const Float2& halfLength) : position(position), halfLength(halfLength) {}

		// 辺の長さを取得
		float lenX() const { return halfLength.x * 2.0f; }
		float lenY() const { return halfLength.y * 2.0f; }

		// 頂点を取得.
		Float2 topLeft() const { return Float2(position.x - halfLength.x, position.y - halfLength.y); }
		Float2 bottomRight() const { return Float2(position.x + halfLength.x, position.y + halfLength.y); }


		operator Rect() const;
	};

	Float2 RotationZ(Float2 position, float rotation);
}