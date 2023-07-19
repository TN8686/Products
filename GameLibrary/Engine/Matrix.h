#pragma once
#include <cmath>

#include "Primitive.h"

namespace Engine {
	struct Matrix4x4
	{
		Matrix4x4()
			: m{ 0.0f }
		{}

		float m[4][4];

		// Vector4との乗算.
		Engine::Vector4 operator *(const Engine::Vector4& v) const;

		Engine::Vector3 operator *(const Engine::Vector3& v) const;

		// マトリクス4x4同士の乗算.
		Matrix4x4 operator *(const Matrix4x4& mat) const;
		Matrix4x4& operator *=(const Matrix4x4& mat);


		// 単位マトリクスを生成します.
		static Matrix4x4 Identity();

		// スケールマトリクスを生成します.
		static Matrix4x4 Scale(const float& x, const float& y, const float& z);

		// スケールマトリクスを生成します.
		static Matrix4x4 Scale(const Engine::Vector3& v) {
			return Scale(v.x, v.y, v.x);
		}

		// X軸回転マトリクスを生成します.
		static Matrix4x4 RotationX(const float& radX);

		// Y軸回転マトリクスを生成します.
		static Matrix4x4 RotationY(const float& radY);

		// X軸回転マトリクスを生成します.
		static Matrix4x4 RotationZ(const float& radZ);

		// 平行移動マトリクスを生成します.
		static Matrix4x4 Translate(const float& tx, const float& ty, const float& tz);

		// 平行移動マトリクスを生成します.
		static Matrix4x4 Translate(const Engine::Vector3& v) {
			return Translate(v.x, v.y, v.x);
		}

		// 逆マトリクスを生成します。
		static Matrix4x4 Inverse(const Matrix4x4& m);

		// 転置マトリクスを生成します.
		static Matrix4x4 Transpose(const Matrix4x4& m);


		// 逆マトリクスを生成します。
		Matrix4x4 inverse() const {
			return Inverse(*this);
		}

		// 転置マトリクスを生成します.
		Matrix4x4 transpose() const;
	};
}