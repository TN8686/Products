#include "Player.h"
#include "../Utility/Reference.h"
#include "../Engine/ObjectManager.h"
#include "Ball.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	// 参照用ポインタの初期化.
	Player* Player::instance_ = nullptr;

	bool Player::load()
	{
		// ロード時に自身を登録.
		instance_ = this;

		// コリジョンを生成.
		if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_PLAYER, collision_)) {
			return false;
		}
		collision_->setAABB2D({ {}, Float2((float)halfLength_, (float)halfLength_) });
		oft.pObject_ = collision_;
		oft.layer_ = OFTLAYER_PLAYER;

		// ボールを生成.
		Ball* b = nullptr;
		ObjectManager::getInstance()->createObject(Ball(), &b);
		b->addRefObject(this);

		return true;
	}

	bool Player::init()
	{
		GameObject::init();

		// 座標指定.
		auto pos = getTransform().position;
		//pos.x = static_cast<float>(WindowSize::WIDTH) / 2;
		//pos.y = static_cast<float>(WindowSize::HEIGHT) / 2;
		pos.z = 1.0f;
		setPosition(pos);
		
		return true;
	}

	bool Player::update()
	{
		GameObject::update();
		// 座標反映の実験中.
		/*
		{
			static int sx = 10;
			static int sy = 10;
			static int r = 0;

			// 1～6に各種スケールと回転を仮割り当て
			if (CheckHitKey(KEY_INPUT_1)) {
				sx--;
			}
			if (CheckHitKey(KEY_INPUT_2)) {
				sx++;
			}
			if (CheckHitKey(KEY_INPUT_3)) {
				sy--;
			}
			if (CheckHitKey(KEY_INPUT_4)) {
				sy++;
			}
			if (CheckHitKey(KEY_INPUT_5)) {
				r--;
			}
			if (CheckHitKey(KEY_INPUT_6)) {
				r++;
			}

			setScale({ (float)sx * 0.1f, (float)sy * 0.1f, 1.0f });
			setRotation({ 0,0,(float)r });
		}*/

		static constexpr float RAD_8 = 0.785398f;	// 8分割のラジアン値.
		static constexpr float EPSILON = 0.00001f;	// 誤差範囲.

		// ヒット情報初期化.
		for (int i = 0; i < EDGE_NUM; ++i) {
			isHit_[i] = false;
		}


		// キー入力取得
		int pad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		direction_ = DRC_NON;
		Float2 input;

		// 入力を取る.
		// 右.
		if (pad & PAD_INPUT_RIGHT || CheckHitKey(KEY_INPUT_D)) {
			input.x += 1;
		}
		// 左.
		if (pad & PAD_INPUT_LEFT || CheckHitKey(KEY_INPUT_A)) {
			input.x -= 1;
		}
		// 上.
		if (pad & PAD_INPUT_UP || CheckHitKey(KEY_INPUT_W)) {
			input.y += 1;
		}
		// 下.
		if (pad & PAD_INPUT_DOWN || CheckHitKey(KEY_INPUT_S)) {
			input.y -= 1;
		}

		// 方向判定.
		if (input.x > 0) {
			direction_ = DRC_RIGHT;	// 右;
		}
		else if (input.x < 0) {
			direction_ = DRC_LEFT;	// 左;
		}
		if (input.y > 0) {
			if (input.x > 0) {
				direction_ = DRC_TOP_RIGHT;	// 右上.
			}
			else if (input.x < 0) {
				direction_ = DRC_TOP_LEFT;	// 左上.
			}
			else {
				direction_ = DRC_TOP;	// 上.
			}
		}
		else if (input.y < 0){
			if (input.x > 0) {
				direction_ = DRC_BOTTOM_RIGHT;	// 右下;
			}
			else if (input.x < 0) {
				direction_ = DRC_BOTTOM_LEFT;	// 左下;
			}
			else {
				direction_ = DRC_BOTTOM;	// 下;
			}
		}

		if (direction_ != DRC_NON) {	// なんらかの入力がある場合、speed_を{ speed_, 0 }のベクトルと考えて回転させ、x要素を出す.
			vector_.x = speed_ * cosf(RAD_8 * static_cast<int>(direction_));	// speed_のy要素は0なので考えなくてよい.
			if (vector_.x > -EPSILON && vector_.x < EPSILON) {
				vector_.x = 0.0f;	// 誤差を丸める.
			}
		}

		// x軸移動ヒットチェック.
		auto pos = getTransform().position;
		pos.x += vector_.x;
		setPosition(pos);

		//auto clsMgr = CollisionManager::getInstance();	// コリジョンマネージャを取得.
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_WALL);
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		vector_.x = 0.0f;

		if (direction_ != DRC_NON) {	// x要素と同様にy要素を出す.
			vector_.y = speed_ * sinf(RAD_8 * static_cast<int>(direction_));	// speed_のy要素は0なので考えなくてよい.
			if (vector_.y > -EPSILON && vector_.y < EPSILON) {
				vector_.y = 0.0f;	// 誤差を丸める.
			}
		}

		// y軸移動ヒットチェック.
		pos = getTransform().position;
		pos.y += vector_.y;
		setPosition(pos);


		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_WALL);
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		vector_.y = 0.0f;

		// 当たり判定の登録を解除、再登録.
		oft.remove();
		Rect r = collision_->getWorldAABB2D();
		CollisionManager::getInstance()->getL4Tree()->regist(r.topLeft.x, r.topLeft.y, r.bottomRight.x, r.bottomRight.y, &oft);


		return true;
	}

	bool Player::render()
	{
		// 諸々座標反映の実験.
		auto pos = getTransform().getWorldPosition();
		auto s = getTransform().getWorldScale();
		auto r = getTransform().getWorldRotation().z;

		// スケールを反映させた頂点情報を作る.
		Float2 vertex[4]={
			{ -halfLength_ * s.x, -halfLength_ * s.y },
			{ halfLength_ * s.x, -halfLength_ * s.y },
			{ halfLength_ * s.x, halfLength_ * s.y },
			{ -halfLength_ * s.x, halfLength_ * s.y },
		};

		// 回転させる.
		for (int i = 0; i < 4; ++i) {
			vertex[i]= RotationZ(vertex[i], r);
		}

		// ワールド座標を加算して描画.
		DrawQuadrangle(
			(int)(vertex[0].x + pos.x), (int)(vertex[0].y + pos.y),
			(int)(vertex[1].x + pos.x), (int)(vertex[1].y + pos.y),
			(int)(vertex[2].x + pos.x), (int)(vertex[2].y + pos.y),
			(int)(vertex[3].x + pos.x), (int)(vertex[3].y + pos.y),
			0xffffc000, true);

		return true;	// 表示OFF

		// 地形ヒット情報表示.
		static const AABB2D AABB = { {1260,640}, {10,10} };
		int c = GetColor(0xFF, 0xFF, 0xFF);
		Rect edgeRect = AABB;
		if (isHit_[EDGE_TOP]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.topLeft.y, (int)edgeRect.bottomRight.x, (int)edgeRect.topLeft.y, c);	// 上.
		}
		if (isHit_[EDGE_LEFT]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.topLeft.y, (int)edgeRect.topLeft.x, (int)edgeRect.bottomRight.y, c);	// 左.
		}
		if (isHit_[EDGE_RIGHT]) {
			DrawLine((int)edgeRect.bottomRight.x, (int)edgeRect.topLeft.y, (int)edgeRect.bottomRight.x, (int)edgeRect.bottomRight.y + 1, c);	// 右.
		}
		if (isHit_[EDGE_BOTTOM]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.bottomRight.y, (int)edgeRect.bottomRight.x + 1, (int)edgeRect.bottomRight.y, c);	// 下.
		}
		std::string str = "PLAYER HIT EDGE : ";
		auto strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - strWidth - 30, 640 - GetFontSize() / 2, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

		// 入力方向表示.
		str = "PLAYER DIRECTION : " + std::to_string(direction_);
		strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - strWidth, WindowSize::HEIGHT - GetFontSize() * 4, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

		// 座標情報表示.
		auto t = getTransform();
		str = 
			"PLAYER POSITION\n"
			"  x : " + std::to_string(t.getWorldPosition().x) + "\n"
			"  y : " + std::to_string(t.getWorldPosition().y) + "\n\n"
			"PLAYER SCALE\n"
			"  x : " + std::to_string(t.getWorldScale().x)+ "\n"
			"  y : " + std::to_string(t.getWorldScale().y)+ "\n\n"
			"PLAYER ROTATION\n"
			"  z : " + std::to_string(t.getWorldRotation().z);

		strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - 160, 0, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

		return true;
	}

	bool Player::unload()
	{
		oft.remove();
		return true;
	}

	void Player::onAABBStay2D(Engine::AABBCollision2D* const collision)
	{
		return;	// 空間分割のチェックのため何も処理しない.

		if (collision->getLayer() != LAYER_BLOCK && collision->getLayer() != LAYER_WALL) {
			return;
		}

		auto pos = getTransform().position;	// 自身のローカル座標.

		auto thisAABB = collision_->getWorldAABB2D();	// 自身の矩形のワールド基準情報.
		auto hitAABB = collision->getWorldAABB2D();		// 対象の矩形のワールド基準情報.


		// ベクトルを基準にどの方向に当たったか判定し、当たった辺に対しての相対値を出して一時座標に加算.
		if (vector_.x < 0) {
			isHit_[EDGE_LEFT] = true;

			pos.x = pos.x + (hitAABB.bottomRight().x - thisAABB.topLeft().x);
		}
		else if (vector_.x > 0) {
			isHit_[EDGE_RIGHT] = true;

			pos.x = pos.x + (hitAABB.topLeft().x - thisAABB.bottomRight().x);
		}

		if (vector_.y < 0) {
			isHit_[EDGE_TOP] = true;

			pos.y = pos.y + (hitAABB.bottomRight().y - thisAABB.topLeft().y);
		}
		else if (vector_.y > 0) {
			isHit_[EDGE_BOTTOM] = true;

			pos.y = pos.y + (hitAABB.topLeft().y - thisAABB.bottomRight().y);
		}

		// 算出した座標を反映.
		setPosition(pos);

	}

}