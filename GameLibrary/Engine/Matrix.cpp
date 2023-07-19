#include "Matrix.h"

namespace Engine {
	Engine::Vector4 Matrix4x4::operator*(const Engine::Vector4& v) const
	{
		Engine::Vector4 r;

		r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
		r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
		r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
		r.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;

		return r;
	}

	// Vector3との乗算　座標として扱う（Vector4のwが1.0fであるものとして扱う）.
	Engine::Vector3 Matrix4x4::operator*(const Engine::Vector3& v) const
	{
		Engine::Vector3 r;

		r.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3];
		r.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3];
		r.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3];

		return r;
	}

	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& mat) const
	{
		Matrix4x4 r;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					r.m[i][j] += m[i][k] * mat.m[k][j];
				}
			}
		}

		return r;
	}

	Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& mat)
	{
		Matrix4x4 r;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				for (int k = 0; k < 4; ++k) {
					r.m[i][j] += m[i][k] * mat.m[k][j];
				}
			}
		}

		*this = r;

		return *this;
	}

	Matrix4x4 Matrix4x4::Identity()
	{
		Matrix4x4 m;
		m.m[0][0] = 1.0f;
		m.m[1][1] = 1.0f;
		m.m[2][2] = 1.0f;
		m.m[3][3] = 1.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::Scale(const float& x, const float& y, const float& z)
	{
		Matrix4x4 m;
		m.m[0][0] = x;
		m.m[1][1] = y;
		m.m[2][2] = z;
		m.m[3][3] = 1.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::RotationX(const float& radX)
	{
		Matrix4x4 m;
		m.m[0][0] = 1.0f;

		m.m[1][1] = std::cosf(radX);
		m.m[1][2] = -std::sinf(radX);

		m.m[2][1] = std::sinf(radX);
		m.m[2][2] = std::cosf(radX);

		m.m[3][3] = 1.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::RotationY(const float& radY)
	{
		Matrix4x4 m;

		m.m[0][0] = std::cosf(radY);
		m.m[0][2] = std::sinf(radY);

		m.m[1][1] = 1.0f;

		m.m[2][0] = -std::sinf(radY);
		m.m[2][2] = std::cosf(radY);

		m.m[3][3] = 1.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::RotationZ(const float& radZ)
	{
		Matrix4x4 m;

		m.m[0][0] = std::cosf(radZ);
		m.m[0][1] = -std::sinf(radZ);

		m.m[1][0] = std::sinf(radZ);
		m.m[1][1] = std::cosf(radZ);

		m.m[2][2] = 1.0f;

		m.m[3][3] = 1.0f;

		return m;
	}

	Matrix4x4 Matrix4x4::Translate(const float& tx, const float& ty, const float& tz)
	{
		Matrix4x4 m = Matrix4x4::Identity();

		m.m[0][3] = tx;
		m.m[1][3] = ty;
		m.m[2][3] = tz;

		return m;
	}

	Matrix4x4 Matrix4x4::Inverse(const Matrix4x4& m)
	{

		// こういうの、計算で出すべきなのか、参考書と同様に手打ちしていくべきなのか….

		float det;

		det =
			m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2]

			+ m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2]
			+ m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]

			+ m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1]

			+ m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0]


			- m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2]
			- m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1]

			- m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2]

			- m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0]

			- m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2]
			- m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0]
			- m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1];

		if (det == 0.0f) {
			return m;	// Δ = 0の時は処理できない.
		}
		det = 1.0f / det;	// 逆数にしておく.

		Matrix4x4 r;

		r.m[0][0] = det * (
			m.m[1][1] * m.m[2][2] * m.m[3][3]
			+ m.m[1][2] * m.m[2][3] * m.m[3][1]
			+ m.m[1][3] * m.m[2][1] * m.m[3][2]
			- m.m[1][1] * m.m[2][3] * m.m[3][2]
			- m.m[1][2] * m.m[2][1] * m.m[3][3]
			- m.m[1][3] * m.m[2][2] * m.m[3][1]);
		r.m[0][1] = det * (
			m.m[0][1] * m.m[2][3] * m.m[3][2]
			+ m.m[0][2] * m.m[2][1] * m.m[3][3]
			+ m.m[0][3] * m.m[2][2] * m.m[3][1]
			- m.m[0][1] * m.m[2][2] * m.m[3][3]
			- m.m[0][2] * m.m[2][3] * m.m[3][1]
			- m.m[0][3] * m.m[2][1] * m.m[3][2]);
		r.m[0][2] = det * (
			m.m[0][1] * m.m[1][2] * m.m[3][3]
			+ m.m[0][2] * m.m[1][3] * m.m[3][1]
			+ m.m[0][3] * m.m[1][1] * m.m[3][2]
			- m.m[0][1] * m.m[1][3] * m.m[3][2]
			- m.m[0][2] * m.m[1][1] * m.m[3][3]
			- m.m[0][3] * m.m[1][2] * m.m[3][1]);
		r.m[0][3] = det * (
			m.m[0][1] * m.m[1][3] * m.m[2][2]
			+ m.m[0][2] * m.m[1][1] * m.m[2][3]
			+ m.m[0][3] * m.m[1][2] * m.m[2][1]
			- m.m[0][1] * m.m[1][2] * m.m[2][3]
			- m.m[0][2] * m.m[1][3] * m.m[2][1]
			- m.m[0][3] * m.m[1][1] * m.m[2][2]);

		r.m[1][0] = det * (
			m.m[1][0] * m.m[2][3] * m.m[3][2]
			+ m.m[1][2] * m.m[2][0] * m.m[3][3]
			+ m.m[1][3] * m.m[2][2] * m.m[3][0]
			- m.m[1][0] * m.m[2][2] * m.m[3][3]
			- m.m[1][2] * m.m[2][3] * m.m[3][0]
			- m.m[1][3] * m.m[2][0] * m.m[3][2]);
		r.m[1][1] = det * (
			m.m[0][0] * m.m[2][2] * m.m[3][3]
			+ m.m[0][2] * m.m[2][3] * m.m[3][0]
			+ m.m[0][3] * m.m[2][0] * m.m[3][2]
			- m.m[0][0] * m.m[2][3] * m.m[3][2]
			- m.m[0][2] * m.m[2][0] * m.m[3][3]
			- m.m[0][3] * m.m[2][2] * m.m[3][0]);
		r.m[1][2] = det * (
			m.m[0][0] * m.m[1][3] * m.m[3][2]
			+ m.m[0][2] * m.m[1][0] * m.m[3][3]
			+ m.m[0][3] * m.m[1][2] * m.m[3][0]
			- m.m[0][0] * m.m[1][2] * m.m[3][3]
			- m.m[0][2] * m.m[1][3] * m.m[3][0]
			- m.m[0][3] * m.m[1][0] * m.m[3][2]);
		r.m[1][3] = det * (
			m.m[0][0] * m.m[1][2] * m.m[2][3]
			+ m.m[0][2] * m.m[1][3] * m.m[2][0]
			+ m.m[0][3] * m.m[1][0] * m.m[2][2]
			- m.m[0][0] * m.m[1][3] * m.m[2][2]
			- m.m[0][2] * m.m[1][0] * m.m[2][3]
			- m.m[0][3] * m.m[1][2] * m.m[2][0]);

		r.m[2][0] = det * (
			m.m[1][0] * m.m[2][1] * m.m[3][3]
			+ m.m[1][1] * m.m[2][3] * m.m[3][0]
			+ m.m[1][3] * m.m[2][0] * m.m[3][1]
			- m.m[1][0] * m.m[2][3] * m.m[3][1]
			- m.m[1][1] * m.m[2][0] * m.m[3][3]
			- m.m[1][3] * m.m[2][1] * m.m[3][0]);
		r.m[2][1] = det * (
			m.m[0][0] * m.m[2][3] * m.m[3][1]
			+ m.m[0][1] * m.m[2][0] * m.m[3][3]
			+ m.m[0][3] * m.m[2][1] * m.m[3][0]
			- m.m[0][0] * m.m[2][1] * m.m[3][3]
			- m.m[0][1] * m.m[2][3] * m.m[3][0]
			- m.m[0][3] * m.m[2][0] * m.m[3][1]);
		r.m[2][2] = det * (
			m.m[0][0] * m.m[1][1] * m.m[3][3]
			+ m.m[0][1] * m.m[1][3] * m.m[3][0]
			+ m.m[0][3] * m.m[1][0] * m.m[3][1]
			- m.m[0][0] * m.m[1][3] * m.m[3][1]
			- m.m[0][1] * m.m[1][0] * m.m[3][3]
			- m.m[0][3] * m.m[1][1] * m.m[3][0]);
		r.m[2][3] = det * (
			m.m[0][0] * m.m[1][3] * m.m[2][1]
			+ m.m[0][1] * m.m[1][0] * m.m[2][3]
			+ m.m[0][3] * m.m[1][1] * m.m[2][0]
			- m.m[0][0] * m.m[1][1] * m.m[2][3]
			- m.m[0][1] * m.m[1][3] * m.m[2][0]
			- m.m[0][3] * m.m[1][0] * m.m[2][1]);

		r.m[3][0] = det * (
			m.m[1][0] * m.m[2][2] * m.m[3][1]
			+ m.m[1][1] * m.m[2][0] * m.m[3][2]
			+ m.m[1][2] * m.m[2][1] * m.m[3][0]
			- m.m[1][0] * m.m[2][1] * m.m[3][2]
			- m.m[1][1] * m.m[2][2] * m.m[3][0]
			- m.m[1][2] * m.m[2][0] * m.m[3][1]);
		r.m[3][1] = det * (
			m.m[0][0] * m.m[2][1] * m.m[3][2]
			+ m.m[0][1] * m.m[2][2] * m.m[3][0]
			+ m.m[0][2] * m.m[2][0] * m.m[3][1]
			- m.m[0][0] * m.m[2][2] * m.m[3][1]
			- m.m[0][1] * m.m[2][0] * m.m[3][2]
			- m.m[0][2] * m.m[2][1] * m.m[3][0]);
		r.m[3][2] = det * (
			m.m[0][0] * m.m[1][2] * m.m[3][1]
			+ m.m[0][1] * m.m[1][0] * m.m[3][2]
			+ m.m[0][2] * m.m[1][1] * m.m[3][0]
			- m.m[0][0] * m.m[1][1] * m.m[3][2]
			- m.m[0][1] * m.m[1][2] * m.m[3][0]
			- m.m[0][2] * m.m[1][0] * m.m[3][1]);
		r.m[3][3] = det * (
			m.m[0][0] * m.m[1][1] * m.m[2][2]
			+ m.m[0][1] * m.m[1][2] * m.m[2][0]
			+ m.m[0][2] * m.m[1][0] * m.m[2][1]
			- m.m[0][0] * m.m[1][2] * m.m[2][1]
			- m.m[0][1] * m.m[1][0] * m.m[2][2]
			- m.m[0][2] * m.m[1][1] * m.m[2][0]);

		return r;
	}

	Matrix4x4 Matrix4x4::Transpose(const Matrix4x4& m)
	{
		Matrix4x4 r;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				r.m[i][j] = m.m[j][i];
			}
		}
		return r;
	}

	Matrix4x4 Matrix4x4::transpose() const
	{
		Matrix4x4 r;

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				r.m[i][j] = m[j][i];
			}
		}
		return r;
	}
}