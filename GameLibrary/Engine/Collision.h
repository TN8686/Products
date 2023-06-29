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

	// 前方宣言.
	class HitChecker;
	class GameObject;

	// 各種コリジョンの基底クラス.
	class Collision2D {
		// メンバ変数.
	private:
		CllisionType Type_;
		GameObject* parent_;	// 親オブジェクト.
		bool isActive_;			// アクティブ状態.
		// コンストラクタ.
	protected:
		explicit Collision2D(CllisionType Type = COLTYPE_NON)
			: Type_(Type)
			, parent_(nullptr)
			, isActive_(true)
		{}

		// デストラクタ.
	public:
		virtual ~Collision2D() = default;

		// 操作.
	public:
		virtual bool render(unsigned int color) const = 0;	// 描画.

		// 属性.
	public:
		bool getActive() const;
		bool getActiveSelf() const		{ return isActive_; }
		CllisionType getType() const	{ return Type_; }
		GameObject* getParent() const	{ return parent_; }
		virtual Rect getWorldBoundingRect() const = 0;	// OFT登録用に、オブジェクトを包むAABBの座標情報を取得します.

		void setActive(const bool& active)			{ isActive_ = active; }
		void setParent(GameObject* const& parent)	{ parent_ = parent; }
	};

	// 点.
	class PointCollision2D : public Collision2D {
		// メンバ変数.
	private:
		Vector2 position_;			// 座標.

		// コンストラクタ.
	public:
		explicit PointCollision2D(Vector2 position = Vector2())
			: Collision2D(COLTYPE_POINT)
			, position_(position)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
	public:
		Vector2 getPoint2D() const { return position_; }
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override { 			
			auto pos = getWorldPosition();
			return Rect(pos, pos);
		}

		void setPoint2D(const Vector2& position) { position_ = position; }

	};

	// 軸並行矩形.
	class AABBCollision2D : public Collision2D {
		// メンバ変数.
	private:
		AABB2D aabb_;			// 矩形領域.

		// コンストラクタ.
	public:
		explicit AABBCollision2D(AABB2D aabb = AABB2D())
			: Collision2D(COLTYPE_AABB)
			, aabb_(aabb)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
	public:
		AABB2D getAABB2D() const { return aabb_; }
		AABB2D getWorldAABB2D() const;
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override { return getWorldAABB2D(); };

		void setAABB2D(const AABB2D& aabb) { aabb_ = aabb; }

	};

	// 円.
	class CircleCollision2D : public Collision2D {
		// メンバ変数.
	private:
		Circle2D circle_;			// 円形領域.

		// コンストラクタ.
	public:
		explicit CircleCollision2D(Circle2D circle = Circle2D())
			: Collision2D(COLTYPE_CIRCLE)
			, circle_(circle)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
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


	// カプセル.
	class CapsuleCollision2D : public Collision2D {
		// メンバ変数.
	private:
		Capsule2D capsule_;			// 円形領域.

		// コンストラクタ.
	public:
		explicit CapsuleCollision2D(Capsule2D capsule = Capsule2D())
			: Collision2D(COLTYPE_CAPSULE)
			, capsule_(capsule)
		{}

		// 操作.
	public:
		bool render(unsigned int color) const override;

		// 属性.
	public:
		Capsule2D getCapsule2D() const { return capsule_; }
		Capsule2D getWorldCapsule2D() const;
		Vector2 getWorldPosition() const;
		Rect getWorldBoundingRect() const override {
			auto c = getWorldCapsule2D();
			auto spos = c.segment.position;
			auto epos = c.segment.getEndPoint();

			Rect ret;
			ret.topLeft.x = min(spos.x - c.radius, epos.x - c.radius);		// 左辺.
			ret.topLeft.y = min(spos.y - c.radius, epos.y - c.radius);		// 上辺.
			ret.bottomRight.x = max(spos.x + c.radius, epos.x + c.radius);	// 右辺.
			ret.bottomRight.y = max(spos.y + c.radius, epos.y + c.radius);	// 下辺.

			return ret;
		}

		void setCapsule2D(const Capsule2D& capsule) { capsule_ = capsule; }

	};
}