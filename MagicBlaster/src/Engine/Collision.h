#pragma once

#include "primitive2d.h"
#include "Sprite.h"

namespace Collision {

	bool PointOnPoint(Point2D sa, Float2 ra, Point2D sb, Float2 rb);
	bool PointOnSegment(Point2D sa, Float2 ra, Segment2D sb, Float2 rb);
	bool PointOnCircle(Point2D sa, Float2 ra, Circle2D sb, Float2 rb);
	bool PointOnRect(Point2D sa, Float2 ra, AABB2D sb, Float2 rb);

	bool SegmentOnSegment(Segment2D sa, Float2 ra, Segment2D sb, Float2 rb);
	bool SegmentOnCircle(Segment2D sa, Float2 ra, Circle2D sb, Float2 rb);
	bool SegmentOnRect(Segment2D sa, Float2 ra, AABB2D sb, Float2 rb);

	bool CircleOnCircle(Circle2D sa, Float2 ra, Circle2D sb, Float2 rb);
	bool CircleOnRect(Circle2D sa, Float2 ra, AABB2D sb, Float2 rb);

	bool RectOnRect(AABB2D sa, Float2 ra, AABB2D sb, Float2 rb);

	void RectToSprite(Sprite* sprite, AABB2D collision, Float2 position);
}

/* // 時間かかりすぎる　今は無し.
class CollisionFactry {
public:
	// ヒットレイヤー.
	enum Layer{
		PlayerHurt,
		PlayerAttack,
		EnemyHurt,
		EnemyAttack,
		Ground,
		GroundCheck,
		EnumLayerNum
	};

	// コリジョンタイプ　将来的にはサークルなども増やす.
	enum Type {
		Point,
		Rect,
		EnumTypeNum
	};

	static constexpr bool HIT_TABLE[EnumLayerNum][EnumLayerNum] = {
		{0,0,1,1,0,0},	// PlayerHurt.
		{0,0,1,0,0,0},	// PlayerAttack.



	};


private:




};

// ヒットチェッカー.

// コリジョンオブジェクトインターフェース,
*/