#include "EnemyFactory.h"
#include "Enemy.h"
#include "Mummy.h"
#include "Bat.h"
#include "UniquePtrTemplate.h"

EnemyFactory EnemyFactory::_instace;

bool EnemyFactory::Load() {
	assert(_pSpriteResourceList != nullptr);
	if (_pSpriteResourceList == nullptr) {
		return false;
	}

	// TODO 将来的に、先に用意した使用する敵の種類のリストに応じてLoadするように.
	Mummy::pSetSpriteResource(pCreateObject(*_pSpriteResourceList));
	if (!Mummy::Load()) {
		return false;
	}
	if (!Bat::Load()) {
		return false;
	}
	return true;

}

bool EnemyFactory::Create(){

	if (_queue.empty()) {	// キューが空なら終了.
		return true;
	}
	// キューを走査.

	bool isInstanced = false;
	for (const auto& q : _queue) {

		//不正なIDだったら何もしない.
		if (q.id >= EnemyID::EnumEnemyID_Num) {
			continue;
		}

		isInstanced = false;

		// 敵リストを走査.
		for (auto& e : _enemyList) {
			if (!e->GetOnActive()) {	// アクティブでない物があったら.
				// idの敵を生成.
				switch (q.id)
				{
				case EnemyID::Mummy:
					e.reset(new Mummy(q.point));
					break;

				case EnemyID::Bat:
					e.reset(new Bat(q.point));
					break;

				case EnemyID::EnumEnemyID_Num:
					break;
				default:
					break;
				}

				assert(e != nullptr);
				if (e != nullptr) {			// nullチェック.
					EnemyInit(e.get(), q);	// 生成した敵を初期化.
					isInstanced = true;		// 空きに対して生成した.
					break;					// 敵リストの走査から脱出.
				}
				else {
					_queue.clear();
					return false;	// 生成した箇所がnullptrだったらエラーで終了.
				}
			}
		}

		// 領域が敵出現上限数に達していたら新規生成範囲には行かずにcontinue.
		if (_enemyList.size() >= ENEMY_MAX) {
			continue;			
		}

		if (isInstanced) {	// ここまでで生成できている.
			continue;		// 次のキューへ.
		}
		else {				// ここまでで生成できていない.
			// 新たな要素を追加して生成する.
			switch (q.id)
			{
			case EnemyID::Mummy:
				_enemyList.emplace_back(new Mummy(q.point));
				break;

			case EnemyID::Bat:
				_enemyList.emplace_back(new Bat(q.point));
				break;

			case EnemyID::EnumEnemyID_Num:
				break;
			default:
				break;
			}
			auto index = _enemyList.size() - 1;
			assert(&_enemyList.at(index) != nullptr);
			if (&_enemyList.at(index) != nullptr) {			// nullチェック.
				EnemyInit(_enemyList.at(index).get(), q);
			}
			else {
				_queue.clear();
				return false;	// 生成した箇所がnullptrだったらエラーで終了.
			}
		}
	}
	_queue.clear();
	return true;
}

bool EnemyFactory::Enqueue(EnemyCreateQueue queue) {
	if (_queue.size() < ENEMY_MAX) {	// キューの追加は最大でも敵出現上限まで.
		_queue.push_back(queue);
		return true;
	}
	return false;
}

bool EnemyFactory::Enqueue(EnemyID id, Point2D point, Camera* pCamera, Player* pPlayer, bool isRight) {
	return Enqueue({ id, point, pCamera, pPlayer, isRight });
}

/// <summary>
/// Enemyオブジェクトの初期設定を行います.
/// </summary>
/// <param name="pEnemyObj">設定するオブジェクトのポインタ</param>
/// <param name="queue">生成情報キュー</param>
void EnemyFactory::EnemyInit(Enemy* pEnemyObj, EnemyCreateQueue queue) {
	pEnemyObj->Init();
	pEnemyObj->pSetCamera(queue.pCamera);
	pEnemyObj->SetIsLeft(!queue.isRight);
	pEnemyObj->pSetPlayer(queue.pPlayer);

	assert(_pBlockList != nullptr);
	if (_pBlockList != nullptr) {
		pEnemyObj->pSetBlockList(_pBlockList);
	}

	assert(_pThroughBlockList != nullptr);
	if (_pThroughBlockList != nullptr) {
		pEnemyObj->pSetThroughBlockList(_pThroughBlockList);
	}
}

/// <summary>
/// 利用終了時に必ず実行してください.
/// </summary>
void EnemyFactory::Finalize() {
	_pBlockList = nullptr;
	_pThroughBlockList = nullptr;
	_pSpriteResourceList = nullptr;
	_queue.clear();
	_enemyList.clear();
}