#pragma once

#include <math.h>

namespace Engine {

	static constexpr float EPSILON = 0.00001f;	// ���e�덷.


	// �O���錾.
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

		// ���ς�Ԃ��܂�.
		float dot(const Vector2& r) const {
			return x * r.x + y * r.y;
		}

		// �O�ς�Ԃ��܂�.
		float cross(const Vector2& r) const {
			return x * r.y - y * r.x;
		}

		// �x�N�g���̒�����Ԃ��܂�.
		float length() const {
			return sqrtf(lengthSquare());
		}

		// �x�N�g���̒����̓���Ԃ��܂�.
		float lengthSquare() const {
			return x * x + y * y;
		}

		// �l�𐳋K�����܂�.
		void normalize() {
			const float len = length();
			if (len > 0.0f) {
				x /= len;
				y /= len;
			}
		}

		// ���K�����ꂽ�x�N�g����Ԃ��܂�.
		Vector2 getNormalized() const {
			const float len = length();
			if (len > 0.0f) {
				return Vector2(x / len, y / len);
			}
			return Vector2(0.0f, 0.0f);
		}

		// �����֌W�ɂ��邩��Ԃ��܂�.
		bool isVertical(const Vector2& r) const {
			float d = dot(r);
			return ((d > -EPSILON) && (d < EPSILON));	// �덷�͈͓��Ȃ琂���Ɣ���.
		}

		// ���s�֌W�ɂ��邩��Ԃ��܂�.
		bool isParallel(const Vector2& r) const {
			float d = cross(r);
			return ((d > -EPSILON) && (d < EPSILON));	// �덷�͈͓��Ȃ���s�Ɣ���.
		}

		// �s�p�֌W�ɂ��邩��Ԃ��܂�.
		bool isSharpAngle(const Vector2& r) const {
			return (dot(r) >= 0.0f);
		}

		// �ϊ�.
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

		// �����֌W�ɂ��邩��Ԃ��܂�.
		bool isVertical(const Vector3& r) const {
			float d = dot(r);
			return ((d > -EPSILON) && (d < EPSILON));	// �덷�͈͓��Ȃ琂���Ɣ���.
		}

		// ���s�֌W�ɂ��邩��Ԃ��܂�.
		bool isParallel(const Vector3& r) const {
			float d = cross(r).lengthSquare();
			return ((d > -EPSILON) && (d < EPSILON));	// �덷�͈͓��Ȃ畽�s�Ɣ���.
		}

		// �s�p�֌W�ɂ��邩��Ԃ��܂�.
		bool isSharpAngle(const Vector3& r) const {
			return (dot(r) >= 0.0f);
		}

		// �ϊ� z�̒l��؂�̂Ăđ�����܂�.
		operator Vector2();
	};

	// ����
	struct Line2D {
		Vector2 position;
		Vector2 vector;		// �����x�N�g��
		Line2D() : position(0.0f, 0.0f), vector(0.0f, 0.0f) {}
		Line2D(const Vector2& position, const Vector2& vector)
			: position(position)
			, vector(vector)
		{}
		~Line2D() = default;

		// ����̍��W���擾
		// �����̓x�N�g���Ɋ|���Z����W��
		Vector2 getPoint(float t) const {
			return position + t * vector;
		}
	};

	// ����.
	struct Segment2D : public Line2D {
		Segment2D() {}
		Segment2D(const Vector2& position, const Vector2& vector)
			: Line2D(position, vector)
		{}

		// �I�_���擾
		Vector2 getEndPoint() const {
			return position + vector;
		}
	};

	// �~
	struct Circle2D {
		Vector2 position;
		float radius;	// ���a.
		Circle2D() : position(0.0f, 0.0f), radius(0.5f) {}
		Circle2D(const Vector2& position, float radius)
			: position(position)
			, radius(radius)
		{}
		~Circle2D() {}
	};

	// �J�v�Z��
	struct Capsule2D {
		Segment2D segment;
		float radius;	// ���a.
		Capsule2D() : radius(0.5f) {}
		Capsule2D(const Segment2D& segment, float radius) : segment(segment), radius(radius) {}
		Capsule2D(const Vector2& position, const Vector2& vector, float radius) : segment(position, vector), radius(radius) {}
		~Capsule2D() {}
	};

	// AABB�����A�l�̎��������Ⴄ.
	struct Rect {
		Vector2 topLeft, bottomRight;	// top left, bottom right

		Rect() = default;
		Rect(const Vector2& topLeft, const Vector2& bottomRight)
			: topLeft(topLeft)
			, bottomRight(bottomRight)
		{}
	};

	// �����s��`.
	struct AABB2D
	{
		Vector2 position;
		Vector2 halfLength;

		AABB2D() = default;
		AABB2D(const Vector2& position, const Vector2& halfLength) : position(position), halfLength(halfLength) {}

		// �ӂ̒������擾
		float lenX() const { return halfLength.x * 2.0f; }
		float lenY() const { return halfLength.y * 2.0f; }

		// ���_���擾.
		Vector2 topLeft() const { return Vector2(position.x - halfLength.x, position.y - halfLength.y); }
		Vector2 bottomRight() const { return Vector2(position.x + halfLength.x, position.y + halfLength.y); }


		operator Rect() const;
	};

	// ��]�@�p�x��.
	Vector2 RotationZ(const Vector2& position, const float& rotation);

	// ��]�@���W�A����.
	Vector2 RotationZrad(const Vector2& position, const float& radian);
}