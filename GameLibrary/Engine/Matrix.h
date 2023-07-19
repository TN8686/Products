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

		// Vector4�Ƃ̏�Z.
		Engine::Vector4 operator *(const Engine::Vector4& v) const;

		Engine::Vector3 operator *(const Engine::Vector3& v) const;

		// �}�g���N�X4x4���m�̏�Z.
		Matrix4x4 operator *(const Matrix4x4& mat) const;
		Matrix4x4& operator *=(const Matrix4x4& mat);


		// �P�ʃ}�g���N�X�𐶐����܂�.
		static Matrix4x4 Identity();

		// �X�P�[���}�g���N�X�𐶐����܂�.
		static Matrix4x4 Scale(const float& x, const float& y, const float& z);

		// �X�P�[���}�g���N�X�𐶐����܂�.
		static Matrix4x4 Scale(const Engine::Vector3& v) {
			return Scale(v.x, v.y, v.x);
		}

		// X����]�}�g���N�X�𐶐����܂�.
		static Matrix4x4 RotationX(const float& radX);

		// Y����]�}�g���N�X�𐶐����܂�.
		static Matrix4x4 RotationY(const float& radY);

		// X����]�}�g���N�X�𐶐����܂�.
		static Matrix4x4 RotationZ(const float& radZ);

		// ���s�ړ��}�g���N�X�𐶐����܂�.
		static Matrix4x4 Translate(const float& tx, const float& ty, const float& tz);

		// ���s�ړ��}�g���N�X�𐶐����܂�.
		static Matrix4x4 Translate(const Engine::Vector3& v) {
			return Translate(v.x, v.y, v.x);
		}

		// �t�}�g���N�X�𐶐����܂��B
		static Matrix4x4 Inverse(const Matrix4x4& m);

		// �]�u�}�g���N�X�𐶐����܂�.
		static Matrix4x4 Transpose(const Matrix4x4& m);


		// �t�}�g���N�X�𐶐����܂��B
		Matrix4x4 inverse() const {
			return Inverse(*this);
		}

		// �]�u�}�g���N�X�𐶐����܂�.
		Matrix4x4 transpose() const;
	};
}