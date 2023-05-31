#include "Ball.h"
#include "../Utility/Reference.h"
#include "Player.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	
	bool Ball::load()
	{

		// SEをロード.
		hitSeHandle_ = LoadSoundMem("Res/hit.wav");
		if (hitSeHandle_ < 0) {
			return false;
		}
		ChangeVolumeSoundMem(100, hitSeHandle_);
		
		// コリジョンを生成.
		if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_PLAYER_ATTACK, collision_)) {
			return false;
		}
		collision_->setAABB2D({ Float2(), Float2((float)r_, (float)r_) });
		oft.pObject_ = collision_;
		oft.layer_ = OFTLAYER_PLAYER_ATTACK;

		// プレイヤー用領域を確保.
		getRefObjectList()->reserve(1);

		return true;
	}

	bool Ball::init()
	{
		GameObject::init();

		// 座標指定.
		Engine::Float3 pos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			pos = getRefObjectList()->at(0)->getTransform().position;
		}
		pos.z = 1.5f;
		setPosition(pos);

		velocity_.x = 0.0f;
		velocity_.y = 0.0f;

		// プレイヤーを登録 外部（プレイヤー）から登録するように.
		//getRefObjectList()->at(0) = Player::getInstance();

		return true;
	}

	bool Ball::update()
	{
		GameObject::update();

		static constexpr float RELATIVE_VECTOR_SCALE = 0.04f;   // 相対ベクトルの縮小比率.
		static constexpr float ATTENUATION = 0.98f;             // 加速度の減衰率.

		if (wait_ > 0) {	// ヒットストップ.
			--wait_;
			return true;
		}

		// 座標を取得.
		Engine::Float3 playerPos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			playerPos = getRefObjectList()->at(0)->getTransform().position;
		}
		auto pos = getTransform().position;

		// プレイヤーとの相対ベクトルを取得.
		Engine::Float3 vec = playerPos - pos;

		// 距離がヒモの長さ以上だったら.
		auto vecLen = sqrtf(vec.x * vec.x + vec.y * vec.y);
		if (vecLen > length_) {

			// 同じ角度でヒモの長さのベクトルに変換する.

			auto ropeLenVec = vec;
			if (vecLen != 0) {						// ありえないはずだけど一応0除算回避.
				ropeLenVec = ropeLenVec / vecLen;	// ベクトルを正規化.
			}
			ropeLenVec = ropeLenVec * length_;		// 変換

			// ヒモの長さの分ベクトルを減算.
			vec = vec - ropeLenVec;

			// ベクトルを縮小.
			vec = vec * RELATIVE_VECTOR_SCALE;

			// 加速度を累積加算.
			velocity_ = velocity_ + vec;

		}

		velocity_.y += 1.0f;	// 仮の重力.

		// 直線速度の二乗を算出.
		speedSquare_ = velocity_.x * velocity_.x + velocity_.y * velocity_.y;


		// 移動の反映とヒットチェック 対象の上下左右どの辺に対して当たったか判定するため、xy要素を分離してそれぞれで判定を行う.

		tempVelocity_.x = velocity_.x;	// 加速度を一時的に保持.
		pos.x += tempVelocity_.x;		// 座標に反映.
		setPosition(pos);

		// x軸移動ヒットチェック ヒット時の処理はonAABBStay2D()関数で行われる.
		//CollisionManager::getInstance()->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		velocity_.x = tempVelocity_.x;	// 実加速度に反映.
		tempVelocity_.x = 0.0f;			// 現状の当たり判定処理の都合上初期化.

		pos = getTransform().position;	// 座標を取得し直す.
		tempVelocity_.y = velocity_.y;	// 加速度を一時的に保持.
		pos.y += tempVelocity_.y;		// 座標に反映.
		setPosition(pos);

		// y軸移動ヒットチェック ヒット時の処理はonAABBStay2D()関数で行われる.
		//CollisionManager::getInstance()->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		velocity_.y = tempVelocity_.y;
		tempVelocity_.y = 0.0f;

		// 加速度を減衰.
		velocity_ = velocity_ * ATTENUATION;

		// 当たり判定の登録を解除、再登録.
		oft.remove();
		Rect r = collision_->getWorldAABB2D();
		CollisionManager::getInstance()->getL4Tree()->regist(r.topLeft.x, r.topLeft.y, r.bottomRight.x, r.bottomRight.y, &oft);


		return true;
	}

	bool Ball::render()
	{
		// 座標を取得.
		auto pos = getTransform().position;
		Float3 playerPos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			playerPos = getRefObjectList()->at(0)->getTransform().position;
		}

		// 線を描画.
		DrawLine(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), static_cast<int>(pos.x), static_cast<int>(pos.y), GetColor(0xFF, 0xFF, 0xFF), 1);
		
		// 玉を描画.
		DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), r_, GetColor(0xFF, 0x00, 0x00), true);
		/*
		// とりあえずの情報表示.
		std::string str = "BALL SPEED SQUARE : " + std::to_string(speedSquare_);
		auto strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - strWidth, WindowSize::HEIGHT - GetFontSize() * 3, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));
		*/return true;
	}

	bool Ball::unload()
	{
		DeleteSoundMem(hitSeHandle_);
		oft.remove();
		return true;
	}

	void Ball::onAABBStay2D(Engine::AABBCollision2D* const collision)
	{
		return;	// 空間分割のチェックのため何もしない.

		// ブロック以外チェックしない.
		if (collision->getLayer() != Engine::LAYER_BLOCK) {
			return;
		}

		// 速度が一定以上なら終了.

		if (speedSquare_ > BREAK_SPEED) {
			return;
		}

		// 座標を取得.
		auto pos = getTransform().position;

		// ワールド座標基準で矩形を取得.
		AABB2D aabb = collision->getAABB2D();
		aabb.position = collision->getWorldPosition();

		// 処理.
		// 加速度でどの方向の壁に当たったかを判定.
		// 位置合わせを行った後、加速度を反転させて反射挙動に.
		// TODO とりあえずの状態なので無駄が多い Collision関連の仕様と合わせて要改善.
		if (tempVelocity_.x < 0) {
			pos.x = aabb.bottomRight().x + collision_->getAABB2D().halfLength.x;

			if (tempVelocity_.x < -0.5f) {
				PlaySoundMem(hitSeHandle_, DX_PLAYTYPE_BACK);
			}
			tempVelocity_.x = -tempVelocity_.x;
		}
		else if (tempVelocity_.x > 0) {
			pos.x = aabb.topLeft().x - collision_->getAABB2D().halfLength.x;
			if (tempVelocity_.x > 0.5f) {
				PlaySoundMem(hitSeHandle_, DX_PLAYTYPE_BACK);
			}
			tempVelocity_.x = -tempVelocity_.x;
		}
		if (tempVelocity_.y < 0) {
			pos.y = aabb.bottomRight().y + collision_->getAABB2D().halfLength.y;
			if (tempVelocity_.y < -0.5f) {
				PlaySoundMem(hitSeHandle_, DX_PLAYTYPE_BACK);
			}
			tempVelocity_.y = -tempVelocity_.y;
		}
		else if (tempVelocity_.y > 0) {
			pos.y = aabb.topLeft().y - collision_->getAABB2D().halfLength.y;
			if (tempVelocity_.y > 0.5f) {
				PlaySoundMem(hitSeHandle_, DX_PLAYTYPE_BACK);
			}
			tempVelocity_.y = -tempVelocity_.y;
		}

		setPosition(pos);

	}

}