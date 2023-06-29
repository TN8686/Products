#pragma once

#include <math.h>

namespace Engine {

	static constexpr float EPSILON = 0.00001f;	// 許容誤差.


	// 前方宣言.
	struct Vector2;
	struct Vector3;
	struct AABB2D;
	struct Rect;

	struct Vector2
	{
		float x, y;
		Vector2()
			: x(0.0f)
			, y(0.0f)
		{}
		Vector2(float x, float y)
			: x(x)
			, y(y)
		{}

		Vector2 operator +(const Vector2& r) const {
			return Vector2(x + r.x, y + r.y);
		}

		Vector2 operator -(const Vector2& r) const {
			return Vector2(x - r.x, y - r.y);
		}

		Vector2 operator -() const {
			return Vector2(x * -1.0f, y * -1.0f);
		}

		Vector2 operator *(const Vector2& r) const {
			return Vector2(x * r.x, y * r.y);
		}

		Vector2 operator /(const Vector2& r) const {
			return Vector2(x / r.x, y / r.y);
		}

		Vector2 operator *(float r) const {
			return Vector2(x * r, y * r);
		}

		Vector2 operator /(float r) const {
			return Vector2(x / r, y / r);
		}

		friend Vector2 operator *(float l, const Vector2& r) {
			return Vector2(r.x * l, r.y * l);
		}

		friend Vector2 operator /(float l, const Vector2& r) {
			return Vector2(r.x / l, r.y / l);
		}

		// 内積を返します.
		float dot(const Vector2& r) const {
			return x * r.x + y * r.y;
		}

		// 外積を返します.
		float cross(const Vector2& r) const {
			return x * r.y - y * r.x;
		}

		// ベクトルの長さを返します.
		float length() const {
			return sqrtf(lengthSquare());
		}

		// ベクトルの長さの二乗を返します.
		float lengthSquare() const {
			return x * x + y * y;
		}

		// 値を正規化します.
		void normalize() {
			const float len = length();
			if (len > 0.0f) {
				x /= len;
				y /= len;
			}
		}

		// 正規化されたベクトルを返します.
		Vector2 getNormalized() const {
			const float len = length();
			if (len > 0.0f) {
				return Vector2(x / len, y / len);
			}
			return Vector2(0.0f, 0.0f);
		}

		// 垂直関係にあるかを返します.
		bool isVertical(const Vector2& r) const {
			float d = dot(r);
			return ((d > -EPSILON) && (d < EPSILON));	// 誤差範囲内なら垂直と判定.
		}

		// 平行関係にあるかを返します.
		bool isParallel(const Vector2& r) const {
			float d = cross(r);
			return ((d > -EPSILON) && (d < EPSILON));	// 誤差範囲内なら並行と判定.
		}

		// 鋭角関係にあるかを返します.
		bool isSharpAngle(const Vector2& r) const {
			return (dot(r) >= 0.0f);
		}

		// 変換.
		operator Vector3();

	};

	struct Vector3
	{
		float x, y, z;
		Vector3()
			: x(0.0f)
			, y(0.0f)
			, z(0.0f)
		{}
		Vector3(float x, float y, float z)
			: x(x)
			, y(y)
			, z(z)
		{}

		Vector3 operator +(const Vector3& r) const {
			return Vector3(x + r.x, y + r.y, z + r.z);
		}

		Vector3 operator -(const Vector3& r) const {
			return Vector3(x - r.x, y - r.y, z - r.z);
		}

		Vector3 operator -() const {
			return Vector3(x * -1.0f, y * -1.0f, z * -1.0f);
		}

		Vector3 operator *(const Vector3& r) const {
			return Vector3(x * r.x, y * r.y, z * r.z);
		}

		Vector3 operator /(const Vector3& r) const {
			return Vector3(x / r.x, y / r.y, z / r.z);
		}

		Vector3 operator *(float r) const {
			return Vector3(x * r, y * r, z * r);
		}

		Vector3 operator /(float r) const {
			return Vector3(x / r, y / r, z / r);
		}

		float dot(const Vector3& r) const {
			return x * r.x + y * r.y + z * r.z;
		}

		Vector3 cross(const Vector3& r) const {
			return Vector3(y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x);
		}

		float length() const {
			return sqrtf(lengthSquare());
		}

		float lengthSquare() const {
			return x * x + y * y + z * z;
		}

		void normalize() {
			const float len = length();
			if (len > 0.0f) {
				x /= len;
				y /= len;
				z /= len;
			}
		}

		Vector3 getNormalized() const {
			const float len = length();
			if (len > 0.0f) {
				return Vector3(x / len, y / len, z / len);
			}
			return Vector3(0.0f, 0.0f, 0.0f);
		}

		// 垂直関係にあるかを返します.
		bool isVertical(const Vector3& r) const {
			float d = dot(r);
			return ((d > -EPSILON) && (d < EPSILON));	// 誤差範囲内なら垂直と判定.
		}

		// 平行関係にあるかを返します.
		bool isParallel(const Vector3& r) const {
			float d = cross(r).lengthSquare();
			return ((d > -EPSILON) && (d < EPSILON));	// 誤差範囲内なら平行と判定.
		}

		// 鋭角関係にあるかを返します.
		bool isSharpAngle(const Vector3& r) const {
			return (dot(r) >= 0.0f);
		}

		// 変換 zの値を切り捨てて代入します.
		operator Vector2();
	};

	// 直線
	struct Line2D {
		Vector2 position;
		Vector2 vector;		// 方向ベクトル
		Line2D() : position(0.0f, 0.0f), vector(0.0f, 0.0f) {}
		Line2D(const Vector2& position, const Vector2& vector)
			: position(position)
			, vector(vector)
		{}
		~Line2D() = default;

		// 線上の座標を取得
		// 引数はベクトルに掛け算する係数
		Vector2 getPoint(float t) const {
			return position + t * vector;
		}
	};

	// 線分.
	struct Segment2D : public Line2D {
		Segment2D() {}
		Segment2D(const Vector2& position, const Vector2& vector)
			: Line2D(position, vector)
		{}

		// 終点を取得
		Vector2 getEndPoint() const {
			return position + vector;
		}
	};

	// 円
	struct Circle2D {
		Vector2 position;
		float radius;	// 半径.
		Circle2D() : position(0.0f, 0.0f), radius(0.5f) {}
		Circle2D(const Vector2& position, float radius)
			: position(position)
			, radius(radius)
		{}
		~Circle2D() {}
	};

	// カプセル
	struct Capsule2D {
		Segment2D segment;
		float radius;	// 半径.
		Capsule2D() : radius(0.5f) {}
		Capsule2D(const Segment2D& segment, float radius) : segment(segment), radius(radius) {}
		Capsule2D(const Vector2& position, const Vector2& vector, float radius) : segment(position, vector), radius(radius) {}
		~Capsule2D() {}
	};

	// AABBだが、値の持ち方が違う.
	struct Rect {
		Vector2 topLeft, bottomRight;	// top left, bottom right

		Rect() = default;
		Rect(const Vector2& topLeft, const Vector2& bottomRight)
			: topLeft(topLeft)
			, bottomRight(bottomRight)
		{}
	};

	// 軸並行矩形.
	struct AABB2D
	{
		Vector2 position;
		Vector2 halfLength;

		AABB2D() = default;
		AABB2D(const Vector2& position, const Vector2& halfLength) : position(position), halfLength(halfLength) {}

		// 辺の長さを取得
		float lenX() const { return halfLength.x * 2.0f; }
		float lenY() const { return halfLength.y * 2.0f; }

		// 頂点を取得.
		Vector2 topLeft() const { return Vector2(position.x - halfLength.x, position.y - halfLength.y); }
		Vector2 bottomRight() const { return Vector2(position.x + halfLength.x, position.y + halfLength.y); }


		operator Rect() const;
	};

	// 回転　角度版.
	Vector2 RotationZ(const Vector2& position, const float& rotation);

	// 回転　ラジアン版.
	Vector2 RotationZrad(const Vector2& position, const float& radian);
}