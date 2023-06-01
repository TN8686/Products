#include "Player.h"
#include "../Utility/Reference.h"
#include "../Engine/ObjectManager.h"
#include "Ball.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	// �Q�Ɨp�|�C���^�̏�����.
	Player* Player::instance_ = nullptr;

	bool Player::load()
	{
		// ���[�h���Ɏ��g��o�^.
		instance_ = this;

		// �R���W�����𐶐�.
		if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_PLAYER, collision_)) {
			return false;
		}
		collision_->setAABB2D({ {}, Float2((float)halfLength_, (float)halfLength_) });
		oft.pObject_ = collision_;
		oft.layer_ = OFTLAYER_PLAYER;

		// �{�[���𐶐�.
		Ball* b = nullptr;
		ObjectManager::getInstance()->createObject(Ball(), &b);
		b->addRefObject(this);

		return true;
	}

	bool Player::init()
	{
		GameObject::init();

		// ���W�w��.
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
		// ���W���f�̎�����.
		/*
		{
			static int sx = 10;
			static int sy = 10;
			static int r = 0;

			// 1�`6�Ɋe��X�P�[���Ɖ�]�������蓖��
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

		static constexpr float RAD_8 = 0.785398f;	// 8�����̃��W�A���l.
		static constexpr float EPSILON = 0.00001f;	// �덷�͈�.

		// �q�b�g��񏉊���.
		for (int i = 0; i < EDGE_NUM; ++i) {
			isHit_[i] = false;
		}


		// �L�[���͎擾
		int pad = GetJoypadInputState(DX_INPUT_KEY_PAD1);
		direction_ = DRC_NON;
		Float2 input;

		// ���͂����.
		// �E.
		if (pad & PAD_INPUT_RIGHT || CheckHitKey(KEY_INPUT_D)) {
			input.x += 1;
		}
		// ��.
		if (pad & PAD_INPUT_LEFT || CheckHitKey(KEY_INPUT_A)) {
			input.x -= 1;
		}
		// ��.
		if (pad & PAD_INPUT_UP || CheckHitKey(KEY_INPUT_W)) {
			input.y += 1;
		}
		// ��.
		if (pad & PAD_INPUT_DOWN || CheckHitKey(KEY_INPUT_S)) {
			input.y -= 1;
		}

		// ��������.
		if (input.x > 0) {
			direction_ = DRC_RIGHT;	// �E;
		}
		else if (input.x < 0) {
			direction_ = DRC_LEFT;	// ��;
		}
		if (input.y > 0) {
			if (input.x > 0) {
				direction_ = DRC_TOP_RIGHT;	// �E��.
			}
			else if (input.x < 0) {
				direction_ = DRC_TOP_LEFT;	// ����.
			}
			else {
				direction_ = DRC_TOP;	// ��.
			}
		}
		else if (input.y < 0){
			if (input.x > 0) {
				direction_ = DRC_BOTTOM_RIGHT;	// �E��;
			}
			else if (input.x < 0) {
				direction_ = DRC_BOTTOM_LEFT;	// ����;
			}
			else {
				direction_ = DRC_BOTTOM;	// ��;
			}
		}

		if (direction_ != DRC_NON) {	// �Ȃ�炩�̓��͂�����ꍇ�Aspeed_��{ speed_, 0 }�̃x�N�g���ƍl���ĉ�]�����Ax�v�f���o��.
			vector_.x = speed_ * cosf(RAD_8 * static_cast<int>(direction_));	// speed_��y�v�f��0�Ȃ̂ōl���Ȃ��Ă悢.
			if (vector_.x > -EPSILON && vector_.x < EPSILON) {
				vector_.x = 0.0f;	// �덷���ۂ߂�.
			}
		}

		// x���ړ��q�b�g�`�F�b�N.
		auto pos = getTransform().position;
		pos.x += vector_.x;
		setPosition(pos);

		//auto clsMgr = CollisionManager::getInstance();	// �R���W�����}�l�[�W�����擾.
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_WALL);
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		vector_.x = 0.0f;

		if (direction_ != DRC_NON) {	// x�v�f�Ɠ��l��y�v�f���o��.
			vector_.y = speed_ * sinf(RAD_8 * static_cast<int>(direction_));	// speed_��y�v�f��0�Ȃ̂ōl���Ȃ��Ă悢.
			if (vector_.y > -EPSILON && vector_.y < EPSILON) {
				vector_.y = 0.0f;	// �덷���ۂ߂�.
			}
		}

		// y���ړ��q�b�g�`�F�b�N.
		pos = getTransform().position;
		pos.y += vector_.y;
		setPosition(pos);


		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_WALL);
		//clsMgr->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		vector_.y = 0.0f;

		// �����蔻��̓o�^�������A�ēo�^.
		oft.remove();
		Rect r = collision_->getWorldAABB2D();
		CollisionManager::getInstance()->getL4Tree()->regist(r.topLeft.x, r.topLeft.y, r.bottomRight.x, r.bottomRight.y, &oft);


		return true;
	}

	bool Player::render()
	{
		// ���X���W���f�̎���.
		auto pos = getTransform().getWorldPosition();
		auto s = getTransform().getWorldScale();
		auto r = getTransform().getWorldRotation().z;

		// �X�P�[���𔽉f���������_�������.
		Float2 vertex[4]={
			{ -halfLength_ * s.x, -halfLength_ * s.y },
			{ halfLength_ * s.x, -halfLength_ * s.y },
			{ halfLength_ * s.x, halfLength_ * s.y },
			{ -halfLength_ * s.x, halfLength_ * s.y },
		};

		// ��]������.
		for (int i = 0; i < 4; ++i) {
			vertex[i]= RotationZ(vertex[i], r);
		}

		// ���[���h���W�����Z���ĕ`��.
		DrawQuadrangle(
			(int)(vertex[0].x + pos.x), (int)(vertex[0].y + pos.y),
			(int)(vertex[1].x + pos.x), (int)(vertex[1].y + pos.y),
			(int)(vertex[2].x + pos.x), (int)(vertex[2].y + pos.y),
			(int)(vertex[3].x + pos.x), (int)(vertex[3].y + pos.y),
			0xffffc000, true);

		return true;	// �\��OFF

		// �n�`�q�b�g���\��.
		static const AABB2D AABB = { {1260,640}, {10,10} };
		int c = GetColor(0xFF, 0xFF, 0xFF);
		Rect edgeRect = AABB;
		if (isHit_[EDGE_TOP]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.topLeft.y, (int)edgeRect.bottomRight.x, (int)edgeRect.topLeft.y, c);	// ��.
		}
		if (isHit_[EDGE_LEFT]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.topLeft.y, (int)edgeRect.topLeft.x, (int)edgeRect.bottomRight.y, c);	// ��.
		}
		if (isHit_[EDGE_RIGHT]) {
			DrawLine((int)edgeRect.bottomRight.x, (int)edgeRect.topLeft.y, (int)edgeRect.bottomRight.x, (int)edgeRect.bottomRight.y + 1, c);	// �E.
		}
		if (isHit_[EDGE_BOTTOM]) {
			DrawLine((int)edgeRect.topLeft.x, (int)edgeRect.bottomRight.y, (int)edgeRect.bottomRight.x + 1, (int)edgeRect.bottomRight.y, c);	// ��.
		}
		std::string str = "PLAYER HIT EDGE : ";
		auto strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - strWidth - 30, 640 - GetFontSize() / 2, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

		// ���͕����\��.
		str = "PLAYER DIRECTION : " + std::to_string(direction_);
		strWidth = GetDrawStringWidth(str.c_str(), static_cast<int>(str.length()));
		DrawString(WindowSize::WIDTH - strWidth, WindowSize::HEIGHT - GetFontSize() * 4, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

		// ���W���\��.
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
		return;	// ��ԕ����̃`�F�b�N�̂��߉����������Ȃ�.

		if (collision->getLayer() != LAYER_BLOCK && collision->getLayer() != LAYER_WALL) {
			return;
		}

		auto pos = getTransform().position;	// ���g�̃��[�J�����W.

		auto thisAABB = collision_->getWorldAABB2D();	// ���g�̋�`�̃��[���h����.
		auto hitAABB = collision->getWorldAABB2D();		// �Ώۂ̋�`�̃��[���h����.


		// �x�N�g������ɂǂ̕����ɓ������������肵�A���������ӂɑ΂��Ă̑��Βl���o���Ĉꎞ���W�ɉ��Z.
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

		// �Z�o�������W�𔽉f.
		setPosition(pos);

	}

}