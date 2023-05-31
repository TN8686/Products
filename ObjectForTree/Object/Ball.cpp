#include "Ball.h"
#include "../Utility/Reference.h"
#include "Player.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	
	bool Ball::load()
	{

		// SE�����[�h.
		hitSeHandle_ = LoadSoundMem("Res/hit.wav");
		if (hitSeHandle_ < 0) {
			return false;
		}
		ChangeVolumeSoundMem(100, hitSeHandle_);
		
		// �R���W�����𐶐�.
		if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_PLAYER_ATTACK, collision_)) {
			return false;
		}
		collision_->setAABB2D({ Float2(), Float2((float)r_, (float)r_) });
		oft.pObject_ = collision_;
		oft.layer_ = OFTLAYER_PLAYER_ATTACK;

		// �v���C���[�p�̈���m��.
		getRefObjectList()->reserve(1);

		return true;
	}

	bool Ball::init()
	{
		GameObject::init();

		// ���W�w��.
		Engine::Float3 pos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			pos = getRefObjectList()->at(0)->getTransform().position;
		}
		pos.z = 1.5f;
		setPosition(pos);

		velocity_.x = 0.0f;
		velocity_.y = 0.0f;

		// �v���C���[��o�^ �O���i�v���C���[�j����o�^����悤��.
		//getRefObjectList()->at(0) = Player::getInstance();

		return true;
	}

	bool Ball::update()
	{
		GameObject::update();

		static constexpr float RELATIVE_VECTOR_SCALE = 0.04f;   // ���΃x�N�g���̏k���䗦.
		static constexpr float ATTENUATION = 0.98f;             // �����x�̌�����.

		if (wait_ > 0) {	// �q�b�g�X�g�b�v.
			--wait_;
			return true;
		}

		// ���W���擾.
		Engine::Float3 playerPos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			playerPos = getRefObjectList()->at(0)->getTransform().position;
		}
		auto pos = getTransform().position;

		// �v���C���[�Ƃ̑��΃x�N�g�����擾.
		Engine::Float3 vec = playerPos - pos;

		// �������q���̒����ȏゾ������.
		auto vecLen = sqrtf(vec.x * vec.x + vec.y * vec.y);
		if (vecLen > length_) {

			// �����p�x�Ńq���̒����̃x�N�g���ɕϊ�����.

			auto ropeLenVec = vec;
			if (vecLen != 0) {						// ���肦�Ȃ��͂������ǈꉞ0���Z���.
				ropeLenVec = ropeLenVec / vecLen;	// �x�N�g���𐳋K��.
			}
			ropeLenVec = ropeLenVec * length_;		// �ϊ�

			// �q���̒����̕��x�N�g�������Z.
			vec = vec - ropeLenVec;

			// �x�N�g�����k��.
			vec = vec * RELATIVE_VECTOR_SCALE;

			// �����x��ݐω��Z.
			velocity_ = velocity_ + vec;

		}

		velocity_.y += 1.0f;	// ���̏d��.

		// �������x�̓����Z�o.
		speedSquare_ = velocity_.x * velocity_.x + velocity_.y * velocity_.y;


		// �ړ��̔��f�ƃq�b�g�`�F�b�N �Ώۂ̏㉺���E�ǂ̕ӂɑ΂��ē������������肷�邽�߁Axy�v�f�𕪗����Ă��ꂼ��Ŕ�����s��.

		tempVelocity_.x = velocity_.x;	// �����x���ꎞ�I�ɕێ�.
		pos.x += tempVelocity_.x;		// ���W�ɔ��f.
		setPosition(pos);

		// x���ړ��q�b�g�`�F�b�N �q�b�g���̏�����onAABBStay2D()�֐��ōs����.
		//CollisionManager::getInstance()->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		velocity_.x = tempVelocity_.x;	// �������x�ɔ��f.
		tempVelocity_.x = 0.0f;			// ����̓����蔻�菈���̓s���㏉����.

		pos = getTransform().position;	// ���W���擾������.
		tempVelocity_.y = velocity_.y;	// �����x���ꎞ�I�ɕێ�.
		pos.y += tempVelocity_.y;		// ���W�ɔ��f.
		setPosition(pos);

		// y���ړ��q�b�g�`�F�b�N �q�b�g���̏�����onAABBStay2D()�֐��ōs����.
		//CollisionManager::getInstance()->activeHitCheckAABB2D(collision_, LAYER_BLOCK);

		velocity_.y = tempVelocity_.y;
		tempVelocity_.y = 0.0f;

		// �����x������.
		velocity_ = velocity_ * ATTENUATION;

		// �����蔻��̓o�^�������A�ēo�^.
		oft.remove();
		Rect r = collision_->getWorldAABB2D();
		CollisionManager::getInstance()->getL4Tree()->regist(r.topLeft.x, r.topLeft.y, r.bottomRight.x, r.bottomRight.y, &oft);


		return true;
	}

	bool Ball::render()
	{
		// ���W���擾.
		auto pos = getTransform().position;
		Float3 playerPos;
		if (getRefObjectList()->size() >= 1 && getRefObjectList()->at(0) != nullptr) {
			playerPos = getRefObjectList()->at(0)->getTransform().position;
		}

		// ����`��.
		DrawLine(static_cast<int>(playerPos.x), static_cast<int>(playerPos.y), static_cast<int>(pos.x), static_cast<int>(pos.y), GetColor(0xFF, 0xFF, 0xFF), 1);
		
		// �ʂ�`��.
		DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), r_, GetColor(0xFF, 0x00, 0x00), true);
		/*
		// �Ƃ肠�����̏��\��.
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
		return;	// ��ԕ����̃`�F�b�N�̂��߉������Ȃ�.

		// �u���b�N�ȊO�`�F�b�N���Ȃ�.
		if (collision->getLayer() != Engine::LAYER_BLOCK) {
			return;
		}

		// ���x�����ȏ�Ȃ�I��.

		if (speedSquare_ > BREAK_SPEED) {
			return;
		}

		// ���W���擾.
		auto pos = getTransform().position;

		// ���[���h���W��ŋ�`���擾.
		AABB2D aabb = collision->getAABB2D();
		aabb.position = collision->getWorldPosition();

		// ����.
		// �����x�łǂ̕����̕ǂɓ����������𔻒�.
		// �ʒu���킹���s������A�����x�𔽓]�����Ĕ��ˋ�����.
		// TODO �Ƃ肠�����̏�ԂȂ̂Ŗ��ʂ����� Collision�֘A�̎d�l�ƍ��킹�ėv���P.
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