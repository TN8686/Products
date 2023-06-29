#include <algorithm>
#include <cmath>
#include <cassert>

#include "HitChecker.h"
#include "GameObject.h"

#include "CollisionTrees.h"


namespace Engine {

	using namespace Utility;

	HitChecker HitChecker::instace_;

	HitChecker::~HitChecker() {
	}

	bool HitChecker::initialize()
	{
		if (!l4tree_.init(4, -128, -128, (float)WindowSize::WIDTH + 128, (float)WindowSize::HEIGHT + 128)) {
			return false;	// 四分木生成失敗.
		}

		return true;
	}

	// 全ての当たり判定のチェックを行います.
	// combinationListを基準にレイヤー同士でヒットチェックを行います.
	// 一回のゲームループで一度だけ実行してください.
	bool HitChecker::allHitCheck()
	{
		static CollisionList<Collision2D>* pList = nullptr;
		auto colNum = l4tree_.getAllCollisionList(&pList);
		colNum /= 2;	// リストは二つ一組なのでチェック回数は半分.

		if (pList == nullptr) {
			return false;
		}

		Collision2D** pRoot = pList->getRootPtr();
		
		for (unsigned int i = 0; i < colNum; ++i) {

			auto a = pRoot[i * 2];
			auto b = pRoot[i * 2 + 1];

			if (!a->getActive() || !b->getActive()) {
				continue;
			}

			// a側のtypeが小さな値になるようにする.
			if (a->getType() > b->getType()) {
				auto tmp = a;
				a = b;
				b = tmp;
			}

			auto aType = a->getType();
			int num1 = COLTYPE_NUM * (COLTYPE_NUM + 1) / 2;									// COLTYPE_NUMの組合せの数.
			int num2 = (COLTYPE_NUM - aType) * (COLTYPE_NUM - aType + 1) / 2;				// COLTYPE_NUM - aの組合せの数.
			HitFuncID id = static_cast<HitFuncID>((num1 - num2) + (b->getType() - aType));	// num1 - num2 で、組合せaaのIDを出し、そこにb - aを足す事で組合せabの位置を出している.

			// 種類に応じてコリジョンをダウンキャストしヒットチェック処理を振り分け.
			switch (id)
			{
			case POINT_POINT:
				if (PointOnPoint(dynamic_cast<PointCollision2D*>(a)->getPoint2D(), dynamic_cast<PointCollision2D*>(b)->getWorldPosition())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_AABB:
				if (PointOnAABB(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<AABBCollision2D*>(b)->getWorldAABB2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_CIRCLE:
				if (PointOnCircle(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_CAPSULE:
				if (PointOnCapsule(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<CapsuleCollision2D*>(b)->getWorldCapsule2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_AABB:
				if (AABBOnAABB(dynamic_cast<AABBCollision2D*>(a)->getWorldAABB2D(), dynamic_cast<AABBCollision2D*>(b)->getWorldAABB2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_CIRCLE:
				if (AABBOnCircle(dynamic_cast<AABBCollision2D*>(a)->getWorldAABB2D(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_CAPSULE:
				// TODO
				//MessageBox(NULL, "AABB On Capsule is not implemented.", NULL, MB_OK);
				break;

			case CIRCLE_CIRCLE:
				if (CircleOnCircle(dynamic_cast<CircleCollision2D*>(a)->getWorldCircle2D(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case CIRCLE_CAPSULE:
				if (CircleOnCapsule(dynamic_cast<CircleCollision2D*>(a)->getWorldCircle2D(), dynamic_cast<CapsuleCollision2D*>(b)->getWorldCapsule2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case CAPSULE_CAPSULE:
				// TODO
				//MessageBox(NULL, "Capsule On Capsule is not implemented.", NULL, MB_OK);
				break;

			default:
				break;
			}

		}
		return true;
	}


	// TODO 全ての当たり判定を描画します.
	bool HitChecker::render()
	{
		unsigned int c = 0;
		for (int i = 0; i < OFTLAYER_NUM; ++i) {

			// レイヤーで色を変える.
			c = COLOR[i];

			// TODO 登録されたリストから描画.
			/*
			for (auto& e : aabbCls2DList_[i]) {
				if (!e->getActive()) {
					continue;
				}
				e->render(c);
			}*/
		}
		return true;
	}
	

	// 点と線分の最短距離（2D）.
	// position : 点.
	// line : 直線.
	// 戻り値: 最短距離.
	float HitChecker::pointSegmentDistance2D(const Vector2& position, const Segment2D& line)
	{
		float t;
		Vector2 h;
		return pointSegmentDistance2D(position, line, h, t);
	}

	// 点と線分の最短距離（2D）.
	// position : 点.
	// line : 直線.
	// mp : 点に対して最も近い直線上の座標（戻り値）.
	// m : ベクトルの比（戻り値）.
	// 戻り値: 最短距離.
	float HitChecker::pointSegmentDistance2D(const Vector2& position, const Segment2D& line, Vector2& mp, float& m)
	{
		/*
		* line.vectorをベクトルを a とする.
		* line.positionを始点とした 点positionまでのベクトルを b とする.
		* ベクトルaの大きさを1とした時、始点からmpまでの大きさの比をmとする.
		* positionに対して最も近い直線上の座標をmpとする（mp = a * m）.
		*
		* dot(a, b - mp) = 0
		* dot(a, b - a * m) = 0
		* 分配法則から
		* dot(a, b) + dot(a, -a * m) = 0
		* 定数倍から
		* dot(a, b) + m * -dot(a, a) = 0
		* m = -dot(a, b) / -dot(a, a)
		* m = dot(a, b) / dot(a, a)
		* m = dot(a, b) / |a||a|
		*/

		float lenSqV = line.vector.lengthSquare();	// 距離の二乗（自分自身との内積と同じ）を出す.
		m = 0.0f;

		if (lenSqV > 0.0f) {
			m = line.vector.dot(position - line.position) / lenSqV;	// 線分のベクトルと、線分始点から点までのベクトルの内積を距離二乗で割る m = dot(a, b) / |a||a|.
		}

		mp = line.position + m * line.vector;

		return (mp - position).length();	// 最短距離.
	}

	bool HitChecker::PointOnPoint(Vector2 a, Vector2 b)
	{
		return (fabs(a.x - b.x) < EPSILON) && (fabs(a.y - b.y) < EPSILON);		// 差を許容誤差と比較.
	}

	bool HitChecker::PointOnAABB(Vector2 a, AABB2D b)
	{
		Rect rect = b;

		return  a.y < rect.bottomRight.y&&
				a.y > rect.topLeft.y &&
				a.x < rect.bottomRight.x&&
				a.x > rect.topLeft.x;
	}

	bool HitChecker::PointOnCircle(Vector2 a, Circle2D b)
	{

		// 座標と円の原点の距離が半径の未満だったら.
		return	(a.x - b.position.x) * (a.x - b.position.x) +
				(a.y - b.position.y) * (a.y - b.position.y) <
				(b.radius * b.radius);
	}

	bool HitChecker::PointOnCapsule(Vector2 a, Capsule2D b)
	{
		
		auto v1 = a - b.segment.position;							// 始点から点へのベクトル.
		if (v1.isSharpAngle(b.segment.vector)) {					// 始点からの二つのベクトルが鋭角か.
			Vector2 v2 = -b.segment.vector;							// 終点から始点へのベクトル.
			Vector2 v3 = a - b.segment.getEndPoint();				// 終点から点へのベクトル.
			if (v2.isSharpAngle(v3)) {								// 終点からの二つのベクトルが鋭角か.
				auto ref = pointSegmentDistance2D(a, b.segment);	// 両方鋭角なので線分との距離を出す.
				return (ref < b.radius);							// 距離が半径未満だったら衝突.
			}
			else {	// 終点側の鈍角.
				return PointOnCircle(a, Circle2D(b.segment.getEndPoint(), b.radius));	// 終点を中心とした円との衝突判定.
			}
		}
		else {	// 始点側の鈍角.
			return PointOnCircle(a, Circle2D(b.segment.position, b.radius));	// 始点を中心とした円との衝突判定.
		}
	}

	bool HitChecker::AABBOnAABB(AABB2D a, AABB2D b)
	{

		// 矩形に変換.
		Rect rect1, rect2;
		rect1 = a;
		rect2 = b;

		return	rect1.topLeft.y < rect2.bottomRight.y&&
				rect1.bottomRight.y > rect2.topLeft.y &&
				rect1.topLeft.x < rect2.bottomRight.x&&
				rect1.bottomRight.x > rect2.topLeft.x;
	}

	bool HitChecker::AABBOnCircle(AABB2D a, Circle2D b)
	{
		Rect rect = a;

		// 円の原点に最も近い矩形の点を探す.
		Vector2 pos;
		pos.x = std::min<float>(std::max<float>(rect.topLeft.x, b.position.x), rect.bottomRight.x);	// 矩形の左のxと円の原点でより右にある方を取り、さらにそれと矩形の右のxでより左にある方を取る事で、最も近いx座標が出る.
		pos.y = std::min<float>(std::max<float>(rect.topLeft.y, b.position.y), rect.bottomRight.y);	// 同じ理屈で最も近いyも取る.

		// その座標と円の半径で接触判定を行う.
		return	(pos.x - b.position.x) * (pos.x - b.position.x) +
				(pos.y - b.position.y) * (pos.y - b.position.y) <
				(b.radius * b.radius);
	}

	bool HitChecker::CircleOnCircle(Circle2D a, Circle2D b)
	{
		// 円1の原点から円2の原点までの距離が双方の半径の合計未満だったら.
		return	(a.position.x - b.position.x) * (a.position.x - b.position.x) +
				(a.position.y - b.position.y) * (a.position.y - b.position.y) <
				(a.radius + b.radius) * (a.radius + b.radius);
	}

	bool HitChecker::CircleOnCapsule(Circle2D a, Capsule2D b)
	{
		auto v1 = a.position - b.segment.position;							// 始点から点へのベクトル.
		if (v1.isSharpAngle(b.segment.vector)) {							// 始点からの二つのベクトルが鋭角か.
			Vector2 v2 = -b.segment.vector;									// 終点から始点へのベクトル.
			Vector2 v3 = a.position - b.segment.getEndPoint();				// 終点から点へのベクトル.
			if (v2.isSharpAngle(v3)) {										// 終点からの二つのベクトルが鋭角か.
				auto ref = pointSegmentDistance2D(a.position, b.segment);	// 両方鋭角なので線分との距離を出す.
				return (ref < b.radius + a.radius);							// 距離が半径の合計未満だったら衝突.
			}
			else {	// 終点側の鈍角.
				return CircleOnCircle(a, Circle2D(b.segment.getEndPoint(), b.radius));	// 終点を中心とした円との衝突判定.
			}
		}
		else {	// 始点側の鈍角.
			return CircleOnCircle(a, Circle2D(b.segment.position, b.radius));	// 始点を中心とした円との衝突判定.
		}
	}

	bool HitChecker::PointOnPoint(PointCollision2D a, PointCollision2D b)
	{
		return PointOnPoint(a.getWorldPosition(), b.getWorldPosition());
	}

	bool HitChecker::PointOnAABB(PointCollision2D a, AABBCollision2D b)
	{
		return  PointOnAABB(a.getWorldPosition(), b.getWorldAABB2D());
	}

	bool HitChecker::PointOnCircle(PointCollision2D a, CircleCollision2D b)
	{
		return	PointOnCircle(a.getWorldPosition(), b.getWorldCircle2D());
	}

	bool HitChecker::AABBOnAABB(AABBCollision2D a, AABBCollision2D b)
	{
		return	AABBOnAABB(a.getWorldAABB2D(), b.getWorldAABB2D());
	}

	bool HitChecker::AABBOnCircle(AABBCollision2D a, CircleCollision2D b)
	{
		return	AABBOnCircle(a.getWorldAABB2D(), b.getWorldCircle2D());
	}

	bool HitChecker::CircleOnCircle(CircleCollision2D a, CircleCollision2D b)
	{
		return	CircleOnCircle(a.getWorldCircle2D(), b.getWorldCircle2D());
	}

	bool HitChecker::CircleOnCapsule(CircleCollision2D a, CapsuleCollision2D b) {
		return CircleOnCapsule(a.getWorldCircle2D(), b.getWorldCapsule2D());
	}

	// TODO ハンドルが有効なものかどうかをチェックします.
	bool HitChecker::handleCheck(int handle)
	{
		if (handle == -1) {	// -1だけ無効な値として扱う.
			return false;
		}

		/* TODO L4Treeリストのサイズチェック.
		if (handle < 0 || handle >= ) {
			return false;
		}*/

		return true;
	}
}