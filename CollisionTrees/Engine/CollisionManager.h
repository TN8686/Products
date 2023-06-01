#pragma once
#include <vector>

#include "../Utility/Reference.h"
#include "Collision.h"
#include "CollisionTrees.h"

namespace Engine {

	// TODO
	// 複数のリストをハンドルで管理できるように.
	// 個別の当たり判定オブジェクトを破棄.
	// ベクトルに対応？.

	// 前方宣言.
	class GameObject;

	class CollisionManager {
		// メンバ変数.
	private:
		// シングルトン.
		static CollisionManager instace_;

		std::vector<AABBCollision2D*> aabbCls2DList_[LAYER_NUM];	// レイヤー数分のAABBコリジョンリスト.

		Liner4TreeManager<AABBCollision2D> l4tree_;	// 仮の四分木空間分割管理.

		/*
			// レイヤー.
	enum CllisionLayer {
		LAYER_DEFAULT,
		LAYER_PLAYER,
		LAYER_PLAYER_ATTACK,
		LAYER_BLOCK,
		LAYER_WALL,
		LAYER_NUM
	};
		*/

		std::vector<bool> combinationList_ = {					// allHitCheckでチェックするレイヤーの組合せのリスト TODO 直接ここで定義すべきではないが、今だけここで.
			false,	false,	false,	false,	false,
					false,	false,	true,	true,
							false,	true,	false,
									false,	false,
											false,
		};

		static constexpr unsigned int COLOR[LAYER_NUM] = {
			0xFF00FF00,	// LAYER_DEFAULT,
			0xFF00FF00,	// LAYER_PLAYER,
			0xFFFFFF00,	// LAYER_PLAYER_ATTACK,
			0xFF00FFFF,	// LAYER_BLOCK,
			0xFF00FFFF,	// LAYER_WALL,
		};


		// コンストラクタ.
	private:
		CollisionManager()
		{};

		// デストラクタ.
	public:
		~CollisionManager();

		// 操作.
	public:
		// シングルトン オブジェクトを取得.
		static CollisionManager* getInstance() { return &instace_; }

		// 初期化 起動時に実行してください.
		bool initialize();
		
		// コリジョンオブジェクトを生成.
		bool createAABBCollision(GameObject* const parent, const CllisionLayer &layer, AABBCollision2D*& ref);

		// 全ての当たり判定のチェックを行います.
		// combinationListを基準にレイヤー同士でヒットチェックを行います.
		// 一回のゲームループで一度だけ実行してください.
		bool allHitCheck();

		// 能動的なヒットチェックを行います.
		// combinationListにおける設定は無視され、指定レイヤーのオブジェクトをチェックします.
		// 処理順に注意してください（オブジェクトAのupdateで利用した際、オブジェクトBのアップデートがまだの場合に想定された位置に無い場合があります）.
		bool activeHitCheckAABB2D(AABBCollision2D* const collision, const CllisionLayer &layer);

		// 全ての当たり判定を描画します.
		bool render();

		// 全ての当たり判定オブジェクトを破棄します
		// 必ず全てのゲームオブジェクトの破棄を行ってから実行してください.
		bool clear();

		// AABB同士の当たり判定を行います.
		static bool AABBOnAABB(AABBCollision2D a, AABBCollision2D b);

		// AABB同士の当たり判定を行います.
		static bool CircleOnCircle(CircleCollision2D a, CircleCollision2D b);


		// 属性.
	public:
		Liner4TreeManager<AABBCollision2D>* getL4Tree() { return &l4tree_; }

	};
}