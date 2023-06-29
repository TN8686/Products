#pragma once

#include <Windows.h>

#include "../Engine/Primitive.h"
#include "../Utility/Reference.h"

namespace Engine{

	enum CllisionType {
		COLTYPE_NON = -1,
		COLTYPE_POINT,
		COLTYPE_AABB,
		COLTYPE_CIRCLE,
		COLTYPE_CAPSULE,
		COLTYPE_NUM
	};

	// �O���錾.
	class HitChecker;
	class GameObject;

	// �e��R���W�����̊��N���X.
	class Collision2D {
		// �����o�ϐ�.
	private:
		CllisionType Type_;
		GameObject* parent_;	// �e�I�u�W�F�N�g.
		bool isActive_;			// �A�N�e�B�u���.
		// �R���X�g���N�^.
	protected:
		explicit Collision2D(CllisionType Type = COLTYPE_NON)
			: Type_(Type)
			, parent_(nullptr)
			, isActive_(true)
		{}

		// �f�X�g���N�^.
	public:
		virtual ~Collision2D() = default;

		// ����.
	public:
		virtual bool render(unsigned int color) const = 0;	// �`��.

		// ����.
	public:
		bool getActive() const;
		bool getActiveSelf() const		{ return isActive_; }
		CllisionType getType() const	{ return Type_; }
		GameObject* getParent() const	{ return parent_; }
		virtual Rect getWorldBoundingRect() const = 0;	// OFT�o�^�p�ɁA�I�u�W�F�N�g����AABB�̍��W�����擾���܂�.

		void setActive(const bool& active)			{ isActive_ = active; }
		void setParent(GameObject* const& parent)	{ parent_ = parent; }
	};

	// �_.
	class PointCollision2D : public Collision2D {
		// �����o�ϐ�.
	private:
		Vector2 position_;			// ���W.

		// �R���X�g���N�^.
	public:
		explicit PointCollision2D(Vector2 position = Vector2())
			: Collision2D(COLTYPE_POINT)
			, position_(position)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		Vector2 getPoint2D() const { return position_; }
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override { 			
			auto pos = getWorldPosition();
			return Rect(pos, pos);
		}

		void setPoint2D(const Vector2& position) { position_ = position; }

	};

	// �����s��`.
	class AABBCollision2D : public Collision2D {
		// �����o�ϐ�.
	private:
		AABB2D aabb_;			// ��`�̈�.

		// �R���X�g���N�^.
	public:
		explicit AABBCollision2D(AABB2D aabb = AABB2D())
			: Collision2D(COLTYPE_AABB)
			, aabb_(aabb)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		AABB2D getAABB2D() const { return aabb_; }
		AABB2D getWorldAABB2D() const;
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override { return getWorldAABB2D(); };

		void setAABB2D(const AABB2D& aabb) { aabb_ = aabb; }

	};

	// �~.
	class CircleCollision2D : public Collision2D {
		// �����o�ϐ�.
	private:
		Circle2D circle_;			// �~�`�̈�.

		// �R���X�g���N�^.
	public:
		explicit CircleCollision2D(Circle2D circle = Circle2D())
			: Collision2D(COLTYPE_CIRCLE)
			, circle_(circle)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		Circle2D getCircle2D() const { return circle_; }
		Circle2D getWorldCircle2D() const;
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override {
			auto c = getWorldCircle2D();
			return Rect(
				{ c.position.x - c.radius, c.position.y - c.radius },
				{ c.position.x + c.radius, c.position.y + c.radius });
		}

		void setCircle2D(const Circle2D& circle) { circle_ = circle; }

	};


	// �J�v�Z��.
	class CapsuleCollision2D : public Collision2D {
		// �����o�ϐ�.
	private:
		Capsule2D capsule_;			// �~�`�̈�.

		// �R���X�g���N�^.
	public:
		explicit CapsuleCollision2D(Capsule2D capsule = Capsule2D())
			: Collision2D(COLTYPE_CAPSULE)
			, capsule_(capsule)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		Capsule2D getCapsule2D() const { return capsule_; }
		Capsule2D getWorldCapsule2D() const;
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override {
			auto c = getWorldCapsule2D();
			auto spos = c.segment.position;
			auto epos = c.segment.getEndPoint();

			Rect ret;
			ret.topLeft.x = min(spos.x - c.radius, epos.x - c.radius);		// ����.
			ret.topLeft.y = min(spos.y - c.radius, epos.y - c.radius);		// ���.
			ret.bottomRight.x = max(spos.x + c.radius, epos.x + c.radius);	// �E��.
			ret.bottomRight.y = max(spos.y + c.radius, epos.y + c.radius);	// ����.

			return ret;
		}

		void setCapsule2D(const Capsule2D& capsule) { capsule_ = capsule; }

	};
}