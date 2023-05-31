#pragma once

#include "../Engine/Primitive.h"
#include "../Utility/Reference.h"

namespace Engine{

	// ���C���[.
	enum CllisionLayer {
		LAYER_DEFAULT,
		LAYER_PLAYER,
		LAYER_PLAYER_ATTACK,
		LAYER_BLOCK,
		LAYER_WALL,
		LAYER_NUM
	};

	// �O���錾.
	class CollisionManager;
	class GameObject;

	// �e��R���W�����̊��N���X.
	class AbstractCollitsion2D {
		// �����o�ϐ�.
	private:
		bool isActive_;			// �A�N�e�B�u���.
		CllisionLayer layer_;	// ���C���[.
		GameObject* parent_;	// �e�I�u�W�F�N�g.

		// �R���X�g���N�^.
	protected:
		AbstractCollitsion2D()
			: isActive_(true)
			, layer_(LAYER_DEFAULT)
			, parent_(nullptr)
		{}

		// �f�X�g���N�^.
	public:
		~AbstractCollitsion2D() = default;

		// ����.
	public:
		virtual bool render(unsigned int color) const = 0;

		// ����.
	public:
		bool getActive() const;
		bool getActiveSelf() const		{ return isActive_; }
		CllisionLayer getLayer() const	{ return layer_; }
		GameObject* getParent() const	{ return parent_; }

		void setActive(const bool& active)			{ isActive_ = active; }
		void setLayer(const CllisionLayer& layer)	{ layer_ = layer; }
		void setParent(GameObject* const& parent)	{ parent_ = parent; }
	};


	// �����s��`.
	class AABBCollision2D : public AbstractCollitsion2D {
		// �����o�ϐ�.
	private:
		AABB2D aabb_;			// ��`�̈�.

		// �R���X�g���N�^.
	public:
		AABBCollision2D() = default;
		AABBCollision2D(AABB2D aabb)
			: aabb_(aabb)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		AABB2D getAABB2D() const { return aabb_; }
		AABB2D getWorldAABB2D() const;
		Float2 getWorldPosition() const;

		void setAABB2D(const AABB2D& aabb) { aabb_ = aabb; }

	};

	// ��]���Ȃ���`.
	class CircleCollision2D : public AbstractCollitsion2D {
		// �����o�ϐ�.
	private:
		Circle2D circle_;			// ��`�̈�.

		// �R���X�g���N�^.
	public:
		CircleCollision2D() = default;
		CircleCollision2D(Circle2D circle)
			: circle_(circle)
		{}

		// ����.
	public:
		bool render(unsigned int color) const override;

		// ����.
	public:
		Circle2D getCircle2D() const { return circle_; }
		Circle2D getWorldCircle2D() const;
		Float2 getWorldPosition() const;

		void setAABB2D(const Circle2D& circle) { circle_ = circle; }

	};
}