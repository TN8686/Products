#include "Collision.h"

namespace Collision {
/*
*　2つの点の衝突判定.
*
* @param sa 左辺の点オブジェクト.
* @param pa 左辺の点オブジェクトの座標.
* @param sb 右辺の点オブジェクト.
* @param pb 右辺の点オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool PointOnPoint(Point2D sa, Float2 ra, Point2D sb, Float2 rb) {
		Float2 num = (sa + ra) - (sb + rb);
		if (fabsf(num.x) < _OX_EPSILON_ && fabsf(num.y) < _OX_EPSILON_) {
			return true;
		}
		return false;
	}

/*
*　点と線分の衝突判定.
*
* @param sa 左辺の点オブジェクト.
* @param pa 左辺の点オブジェクトの座標.
* @param sb 右辺の線分オブジェクト.
* @param pb 右辺の線分オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool PointOnSegment(Point2D sa, Float2 ra, Segment2D sb, Float2 rb) {

		sa = sa + ra;
		sb.p = sb.p + rb;
		Point2D segEnd = sb.getEndPoint();

		float l1 = sqrtf((segEnd.x - sb.p.x) * (segEnd.x - sb.p.x) + (segEnd.y - sb.p.y) * (segEnd.y - sb.p.y));	// 線分の長さ.
		float l2 = sqrtf((sa.x - sb.p.x) * (sa.x - sb.p.x) + (sa.y - sb.p.y) * (sa.y - sb.p.y));					// 線分の始点から点までの長さ

		float aa = fabsf((segEnd.x - sb.p.x) * (sa.x - sb.p.x) + (segEnd.y - sb.p.y) * (sa.y - sb.p.y) - (l1 * l2));
		// l2がl1よりも短いか同じで、かつl1とl2が並行であれば点が線分上に存在する事になる.
		if (l1 >= l2 && fabsf((segEnd.x - sb.p.x) * (sa.x - sb.p.x) + (segEnd.y - sb.p.y) * (sa.y - sb.p.y) - (l1 * l2)) < _OX_EPSILON_) {
			return true;
		}
		return false;
	}

/*
*　点と円の衝突判定.
*
* @param sa 左辺の点オブジェクト.
* @param pa 左辺の点オブジェクトの座標.
* @param sb 右辺の円オブジェクト.
* @param pb 右辺の円オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool PointOnCircle(Point2D sa, Float2 ra, Circle2D sb, Float2 rb) {
		sa = sa + ra;
		sb.p = sb.p + rb;

		// 円の原点から点までの距離が半径以下だったら.
		if ((sa.x - sb.p.x) * (sa.x - sb.p.x) + (sa.y - sb.p.y) * (sa.y - sb.p.y) < (sb.r * sb.r)) {
			return true;
		}
		return false;
	}

/*
*　点と矩形の衝突判定.
*
* @param sa 左辺の点オブジェクト.
* @param pa 左辺の点オブジェクトの座標.
* @param sb 右辺の矩形オブジェクト.
* @param pb 右辺の矩形オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool PointOnRect(Point2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa = sa + ra;
		sb.p = sb.p + rb;

		// 点が、右辺矩形の左右の間、かつ、右辺矩形の上下の間にある場合、衝突している
		if (sa.x > sb.p.x - sb.hl.x && sa.x <= sb.p.x + sb.hl.x && sa.y > sb.p.y - sb.hl.y && sa.y <= sb.p.y + sb.hl.y) {
			return true;
		}
		return false;
	
	}

	bool SegmentOnSegment(Segment2D sa, Float2 ra, Segment2D sb, Float2 rb);
	bool SegmentOnCircle(Segment2D sa, Float2 ra, Circle2D sb, Float2 rb);
	bool SegmentOnRect(Segment2D sa, Float2 ra, AABB2D sb, Float2 rb);

/*
*　2つの円の衝突判定.
*
* @param sa 左辺の円オブジェクト.
* @param pa 左辺の円オブジェクトの座標.
* @param sb 右辺の円オブジェクト.
* @param pb 右辺の円オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool CircleOnCircle(Circle2D sa, Float2 ra, Circle2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;

		// 円1の原点から円2の原点までの距離が双方の半径の合計以下だったら.
		if ((sa.p.x - sb.p.x) * (sa.p.x - sb.p.x) + (sa.p.y - sb.p.y) * (sa.p.y - sb.p.y) <= (sa.r + sb.r) * (sa.r + sb.r)) {
			return true;
		}
		return false;
	}

	bool CircleOnRect(Circle2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;
		// まずは円を一辺の長さが2rの矩形として当たり判定を行う　ここで衝突していれば、実際に衝突している可能性がある.
		if (RectOnRect({ sa.p, { sa.r, sa.r } }, { 0, 0 }, sb, { 0, 0 })) {

			// 円の半径の分X方向に長い矩形と、Y方向に長い矩形を作る.
			AABB2D rect1, rect2;
			rect1 = rect2 = sb;

			// TODO 中心座標の分、1減らして加算…でいいはず　正しく判定できてはいるのでちゃんと検証.
			rect1.hl.x += sa.r - 1;
			rect2.hl.y += sa.r - 1;

			// その矩形と円の中心座標の判定を取り、入っていれば少なくとも衝突している.
			if (PointOnRect(sa.p, { 0, 0 }, rect1, { 0, 0 }) || PointOnRect(sa.p, { 0, 0 }, rect2, { 0, 0 })) {
				return true;
			}
			else {
				// 衝突していなかった場合、矩形の四隅の座標と円の判定を取り、入っていれば衝突している　それでも入っていなければ衝突していない.
				if (sa.p.x <= sb.p.x) {		// 左.
					if (sa.p.y <= sb.p.y) {	// 左上.
						return PointOnCircle({ sb.p.x - sb.hl.x + 1, sb.p.y - sb.hl.y + 1 }, { 0, 0 }, sa, { 0, 0 });
					}
					else {					// 左下.
						return PointOnCircle({ sb.p.x - sb.hl.x + 1, sb.p.y + sb.hl.y }, { 0, 0 }, sa, { 0, 0 });
					}
				}
				else {						// 右.
					if (sa.p.y <= sb.p.y) {	// 右上.
						return PointOnCircle({ sb.p.x + sb.hl.x, sb.p.y - sb.hl.y + 1 }, { 0, 0 }, sa, { 0, 0 });
					}
					else {					// 右下.
						return PointOnCircle({ sb.p.x + sb.hl.x, sb.p.y + sb.hl.y }, { 0, 0 }, sa, { 0, 0 });
					}
				}
			}
		}
		return false;
	}

/*
*　2つの矩形の衝突判定.
*
* @param sa 左辺の矩形オブジェクト.
* @param pa 左辺の矩形オブジェクトの座標.
* @param sb 右辺の矩形オブジェクト.
* @param pb 右辺の矩形オブジェクトの座標.
*
* @retval true  衝突している.
* @retval false 衝突していない.
*/
	bool RectOnRect(AABB2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;

		struct {
			float top, bottom, right, left;
		}rect1, rect2;

		rect1.top = sa.p.y - sa.hl.y;
		rect1.bottom = sa.p.y + sa.hl.y;
		rect1.right = sa.p.x + sa.hl.x;
		rect1.left = sa.p.x - sa.hl.x;

		rect2.top = sb.p.y - sb.hl.y;
		rect2.bottom = sb.p.y + sb.hl.y;
		rect2.right = sb.p.x + sb.hl.x;
		rect2.left = sb.p.x - sb.hl.x;

		// 左辺矩形の上辺が右辺矩形の下辺より高く.
		// 左辺矩形の下辺が右辺矩形の上辺より低く.
		// 左辺矩形の左辺が右辺矩形の右辺より左.
		// 左辺矩形の右辺が右辺矩形の左辺より右の時、衝突している.
		if (rect1.top < rect2.bottom &&
			rect1.bottom > rect2.top &&
			rect1.left < rect2.right &&
			rect1.right > rect2.left) {
			return true;
		}

		return false;
	}

	/// <summary>
	/// 判定可視化関数.
	/// 矩形コリジョン情報を元に、スプライトの値を設定します.
	/// </summary>
	/// <param name="sprite">i 値を設定するスプライト</param>
	/// <param name="collision">o 矩形コリジョン</param>
	/// <param name="position">o 中心座標</param>
	void RectToSprite(Sprite *sprite, AABB2D collision, Float2 position) {
		sprite->pos.x = position.x + collision.p.x;
		sprite->pos.y = position.y + collision.p.y;

		sprite->scale = { 1.0f ,1.0f };

		sprite->rectOffset.left = -collision.hl.x;
		sprite->rectOffset.right = collision.hl.x;

		sprite->rectOffset.top = -collision.hl.y;
		sprite->rectOffset.bottom = collision.hl.y;
	}
}