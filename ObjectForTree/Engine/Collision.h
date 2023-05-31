#pragma once

#include "../Engine/Primitive.h"
#include "../Utility/Reference.h"

namespace Engine{

	// レイヤー.
	enum CllisionLayer {
		LAYER_DEFAULT,
		LAYER_PLAYER,
		LAYER_PLAYER_ATTACK,
		LAYER_BLOCK,
		LAYER_WALL,
		LAYER_NUM
	};

	// 前方宣言.
	class CollisionManager;
	class GameObject;

	// 各種コリジョンの基底クラス.
	class AbstractCollitsion2D {
		// メンバ変数.
	private:
		bool isActive_;			// アクティブ状態.
		CllisionLayer layer_;	// レイヤー.
		GameObject* parent_;	// 親オブジェクト.

		// コンストラクタ.
	protected:
		AbstractCollitsion2D()
			: isActive_(true)
			, layer_(LAYER_DEFAULT)
			, parent_(nullptr)
		{}

		// デストラクタ.
	public:
		~AbstractCollitsion2D() = default;

		// 操作.
	public:
		virtual bool render(unsigned int color) const = 0;

		// 属性.
	public:
		bool getActive() const;
		bool getActiveSelf() const		{ return isActive_; }
		CllisionLayer getLayer() const	{ return layer_; }
		GameObject* getParent() const	{ return parent_; }

		void setActive(const bool& active)			{ isActive_ = active; }
		void setLayer(const CllisionLayer& layer)	{ layer_ = layer; }
		void setParent(GameObject* const& parent)	{ parent_ = parent; }
	};


	// 軸並行矩形.
	class AABBCollision2D : public AbstractCollitsion2D {
		// メンバ変数.
	private:
		AABB2D aabb_;			// 矩形領域.

		// コンストラクタ.
	public:
		AABBCollision2D() = default;
		AABBCollision2D(AABB2D aabb)
			: aabb_(aabb)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
	public:
		AABB2D getAABB2D() const { return aabb_; }
		AABB2D getWorldAABB2D() const;
		Float2 getWorldPosition() const;

		void setAABB2D(const AABB2D& aabb) { aabb_ = aabb; }

	};

	// 回転しない矩形.
	class CircleCollision2D : public AbstractCollitsion2D {
		// メンバ変数.
	private:
		Circle2D circle_;			// 矩形領域.

		// コンストラクタ.
	public:
		CircleCollision2D() = default;
		CircleCollision2D(Circle2D circle)
			: circle_(circle)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
	public:
		Circle2D getCircle2D() const { return circle_; }
		Circle2D getWorldCircle2D() const;
		Float2 getWorldPosition() const;

		void setAABB2D(const Circle2D& circle) { circle_ = circle; }

	};
}