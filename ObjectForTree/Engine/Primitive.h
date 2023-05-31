#pragma once

namespace Engine {

	// �O���錾.
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

		// �Ƃ肠�����̎l�����Z.
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

		// �ϊ�.
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

		// �Ƃ肠�����̎l�����Z.
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

		// �ϊ�.
		operator Float2();
	};

	// �~
	struct Circle2D {
		Float2 position;
		float radius;	// ���a
		Circle2D() : position(0.0f, 0.0f), radius(0.5f) {}
		Circle2D(const Float2& position, float radius)
			: position(position)
			, radius(radius)
		{}
		~Circle2D() {}
	};

	// AABB�����A�l�̎��������Ⴄ.
	struct Rect {
		Float2 topLeft, bottomRight;	// top left, bottom right

		Rect() = default;
		Rect(const Float2& topLeft, const Float2& bottomRight)
			: topLeft(topLeft)
			, bottomRight(bottomRight)
		{}
	};

	// �����s��`.
	struct AABB2D
	{
		Float2 position;
		Float2 halfLength;

		AABB2D() = default;
		AABB2D(const Float2& position, const Float2& halfLength) : position(position), halfLength(halfLength) {}

		// �ӂ̒������擾
		float lenX() const { return halfLength.x * 2.0f; }
		float lenY() const { return halfLength.y * 2.0f; }

		// ���_���擾.
		Float2 topLeft() const { return Float2(position.x - halfLength.x, position.y - halfLength.y); }
		Float2 bottomRight() const { return Float2(position.x + halfLength.x, position.y + halfLength.y); }


		operator Rect() const;
	};

	Float2 RotationZ(Float2 position, float rotation);
}