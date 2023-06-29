#pragma once
#include <vector>

#include "../Utility/Reference.h"
#include "Collision.h"
#include "CollisionTrees.h"

namespace Engine {

	// TODO
	// �����̃q�b�g�̈���n���h���ŊǗ��ł���悤��.

	// �O���錾.
	class GameObject;

	class HitChecker {
	private:
		// �萔.
		enum HitFuncID {
			HITFUNC_NON = -1,
			POINT_POINT,
			POINT_AABB,
			POINT_CIRCLE,
			POINT_CAPSULE,
			AABB_AABB,
			AABB_CIRCLE,
			AABB_CAPSULE,
			CIRCLE_CIRCLE,
			CIRCLE_CAPSULE,
			CAPSULE_CAPSULE,
			HITFUNC_NUM
		};

		static constexpr unsigned int COLOR[OFTLAYER_NUM] = {
			0xFF00FF00,	// LAYER_DEFAULT,
			0xFF00FF00,	// LAYER_PLAYER,
			0xFFFFFF00,	// LAYER_PLAYER_ATTACK,
			0xFF00FFFF,	// LAYER_BLOCK,
			0xFF00FFFF,	// LAYER_WALL,
		};

		// �����o�ϐ�.
	private:
		// �V���O���g��.
		static HitChecker instace_;

		Liner4TreeManager<Collision2D> l4tree_;	// �l���؋�ԕ����Ǘ�.

		int handle_; // �n���h��.

		// �R���X�g���N�^.
	private:
		HitChecker()
			: handle_(-1)
		{};

		// �f�X�g���N�^.
	public:
		~HitChecker();

		// ����.
	public:
		// �V���O���g�� �I�u�W�F�N�g���擾.
		static HitChecker* getInstance() { return &instace_; }

		// ������ �N�����Ɏ��s���Ă�������.
		bool initialize();

		// �S�Ă̓����蔻��̃`�F�b�N���s���܂�.
		// combinationList����Ƀ��C���[���m�Ńq�b�g�`�F�b�N���s���܂�.
		// ���̃Q�[�����[�v�ň�x�������s���Ă�������.
		bool allHitCheck();

		// �S�Ă̓����蔻���`�悵�܂�.
		bool render();
		
		// �_�Ɛ����̍ŒZ�����i2D�j.
		// position : �_.
		// line : ����.
		// �߂�l: �ŒZ����.
		static float pointSegmentDistance2D(const Vector2& position, const Segment2D& line);

		// �_�Ɛ����̍ŒZ�����i2D�j.
		// position : �_.
		// line : ����.
		// h : �_���牺�낵�������̑��i�߂�l�j.
		// t :�x�N�g���W���i�߂�l�j.
		// �߂�l: �ŒZ����.
		static float pointSegmentDistance2D(const Vector2& position, const Segment2D& line, Vector2& h, float& t);

		// �ڐG����v���~�e�B�u��.
		// Point���m�̓����蔻����s���܂�.
		static bool PointOnPoint(Vector2 a, Vector2 b);

		// Point��AABB�̓����蔻����s���܂�.
		static bool PointOnAABB(Vector2 a, AABB2D b);

		// Point��Circle�̓����蔻����s���܂�.
		static bool PointOnCircle(Vector2 a, Circle2D b);
		
		// Circle��Capsule�̓����蔻����s���܂�.
		static bool PointOnCapsule(Vector2 a, Capsule2D b);

		// AABB���m�̓����蔻����s���܂�.
		static bool AABBOnAABB(AABB2D a, AABB2D b);

		// AABB��Circle�̓����蔻����s���܂�.
		static bool AABBOnCircle(AABB2D a, Circle2D b);

		// Circle���m�̓����蔻����s���܂�.
		static bool CircleOnCircle(Circle2D a, Circle2D b);

		// Circle��Capsule�̓����蔻����s���܂�.
		static bool CircleOnCapsule(Circle2D a, Capsule2D b);

		// �ڐG����R���W�����I�u�W�F�N�g��.
		// Point���m�̓����蔻����s���܂�.
		static bool PointOnPoint(PointCollision2D a, PointCollision2D b);

		// Point��AABB�̓����蔻����s���܂�.
		static bool PointOnAABB(PointCollision2D a, AABBCollision2D b);

		// Point��Circle�̓����蔻����s���܂�.
		static bool PointOnCircle(PointCollision2D a, CircleCollision2D b);

		// AABB���m�̓����蔻����s���܂�.
		static bool AABBOnAABB(AABBCollision2D a, AABBCollision2D b);

		// AABB���m�̓����蔻����s���܂�.
		static bool AABBOnCircle(AABBCollision2D a, CircleCollision2D b);

		// Circle���m�̓����蔻����s���܂�.
		static bool CircleOnCircle(CircleCollision2D a, CircleCollision2D b);

		// Circle��Capsule�̓����蔻����s���܂�.
		static bool CircleOnCapsule(CircleCollision2D a, CapsuleCollision2D b);

	private:
		// �n���h�����L���Ȃ��̂��ǂ������`�F�b�N���܂�.
		bool handleCheck(int handle);

		// ����.
	public:
		// ���ݑ��쒆�̃n���h����Ԃ��܂�.
		int getHandle() const { return handle_; }

		// ���삷�郊�X�g�̃n���h����ݒ肵�܂�.
		bool setHandle(const int& handle) {
			if (!handleCheck(handle)) {
				return false;
			}
			handle_ = handle;
			return true;
		}

		Liner4TreeManager<Collision2D>* getL4Tree() { return &l4tree_; }
	};
}