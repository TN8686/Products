#pragma once
#include <vector>

#include "../Utility/Reference.h"
#include "Collision.h"
#include "CollisionTrees.h"

namespace Engine {

	// TODO
	// 複数のヒット領域をハンドルで管理できるように.

	// 前方宣言.
	class GameObject;

	class HitChecker {
	private:
		// 定数.
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

		// メンバ変数.
	private:
		// シングルトン.
		static HitChecker instace_;

		Liner4TreeManager<Collision2D> l4tree_;	// 四分木空間分割管理.

		int handle_; // ハンドル.

		// コンストラクタ.
	private:
		HitChecker()
			: handle_(-1)
		{};

		// デストラクタ.
	public:
		~HitChecker();

		// 操作.
	public:
		// シングルトン オブジェクトを取得.
		static HitChecker* getInstance() { return &instace_; }

		// 初期化 起動時に実行してください.
		bool initialize();

		// 全ての当たり判定のチェックを行います.
		// combinationListを基準にレイヤー同士でヒットチェックを行います.
		// 一回のゲームループで一度だけ実行してください.
		bool allHitCheck();

		// 全ての当たり判定を描画します.
		bool render();
		
		// 点と線分の最短距離（2D）.
		// position : 点.
		// line : 直線.
		// 戻り値: 最短距離.
		static float pointSegmentDistance2D(const Vector2& position, const Segment2D& line);

		// 点と線分の最短距離（2D）.
		// position : 点.
		// line : 直線.
		// h : 点から下ろした垂線の足（戻り値）.
		// t :ベクトル係数（戻り値）.
		// 戻り値: 最短距離.
		static float pointSegmentDistance2D(const Vector2& position, const Segment2D& line, Vector2& h, float& t);

		// 接触判定プリミティブ版.
		// Point同士の当たり判定を行います.
		static bool PointOnPoint(Vector2 a, Vector2 b);

		// PointとAABBの当たり判定を行います.
		static bool PointOnAABB(Vector2 a, AABB2D b);

		// PointとCircleの当たり判定を行います.
		static bool PointOnCircle(Vector2 a, Circle2D b);
		
		// CircleとCapsuleの当たり判定を行います.
		static bool PointOnCapsule(Vector2 a, Capsule2D b);

		// AABB同士の当たり判定を行います.
		static bool AABBOnAABB(AABB2D a, AABB2D b);

		// AABBとCircleの当たり判定を行います.
		static bool AABBOnCircle(AABB2D a, Circle2D b);

		// Circle同士の当たり判定を行います.
		static bool CircleOnCircle(Circle2D a, Circle2D b);

		// CircleとCapsuleの当たり判定を行います.
		static bool CircleOnCapsule(Circle2D a, Capsule2D b);

		// 接触判定コリジョンオブジェクト版.
		// Point同士の当たり判定を行います.
		static bool PointOnPoint(PointCollision2D a, PointCollision2D b);

		// PointとAABBの当たり判定を行います.
		static bool PointOnAABB(PointCollision2D a, AABBCollision2D b);

		// PointとCircleの当たり判定を行います.
		static bool PointOnCircle(PointCollision2D a, CircleCollision2D b);

		// AABB同士の当たり判定を行います.
		static bool AABBOnAABB(AABBCollision2D a, AABBCollision2D b);

		// AABB同士の当たり判定を行います.
		static bool AABBOnCircle(AABBCollision2D a, CircleCollision2D b);

		// Circle同士の当たり判定を行います.
		static bool CircleOnCircle(CircleCollision2D a, CircleCollision2D b);

		// CircleとCapsuleの当たり判定を行います.
		static bool CircleOnCapsule(CircleCollision2D a, CapsuleCollision2D b);

	private:
		// ハンドルが有効なものかどうかをチェックします.
		bool handleCheck(int handle);

		// 属性.
	public:
		// 現在操作中のハンドルを返します.
		int getHandle() const { return handle_; }

		// 操作するリストのハンドルを設定します.
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