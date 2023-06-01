#include "Block.h"
#include "BlockPiece.h"
#include "Ball.h"
#include "../Utility/Reference.h"
#include "../Engine/ObjectManager.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	int Block::breakSeHandle_ = 0;
	int Block::blockObjNum_ = 0;

	bool Block::load()	{
		// �J�E���^���C���N�������g.
		++blockObjNum_;

		// ��ڂ̎�����SE�̑�{�����[�h.
		if (blockObjNum_ == 1) {
			breakSeHandle_ = LoadSoundMem("Res/break.wav");
			if (breakSeHandle_ < 0) {
				return false;
			}
		}

		// �ʂ�SE�����[�h.
		localBreakSeHandle_ = DuplicateSoundMem(breakSeHandle_);
		if (breakSeHandle_ < 0) {
			return false;
		}
		ChangeVolumeSoundMem(100, localBreakSeHandle_);	// ���ʐݒ�.

		
		// �R���W�����𐶐�.
		if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_BLOCK, collision_)) {
			return false;
		}
		collision_->setAABB2D({ Float2(), Float2((float)halfLength_, (float)halfLength_) });
		oft.pObject_ = collision_;
		oft.layer_ = OFTLAYER_BLOCK;

		// �j�А���.
		auto om = ObjectManager::getInstance();
		BlockPiece obj;
		for (int i = 0; i < PIECE_NUM; ++i) {
			om->createObject(obj, &piece[i]);
		}

		om->deleteObject(piece[3]);

		om->createObject(obj, &piece[3]);
		return true;
	}

	bool Block::init()
	{
		GameObject::init();

		auto pos = getTransform().getWorldPosition();
		pos.z = 2;
		piece[0]->setPosition(pos);
		piece[1]->setPosition(pos);
		piece[2]->setPosition(pos);
		piece[3]->setPosition(pos);

		return true;
	}

	bool Block::update()
	{
		oft.remove();
		Rect r = collision_->getWorldAABB2D();
		CollisionManager::getInstance()->getL4Tree()->regist(r.topLeft.x, r.topLeft.y, r.bottomRight.x, r.bottomRight.y, &oft);

		color = GetColor(0x80, 0x80, 0x80);

		return true;
	}

	bool Block::render() {
		auto pos = getTransform().getWorldPosition();

		DrawBox(static_cast<int>(pos.x) - halfLength_, static_cast<int>(pos.y) - halfLength_,
				static_cast<int>(pos.x) + halfLength_, static_cast<int>(pos.y) + halfLength_, color, true);

		DrawBox(static_cast<int>(pos.x) - halfLength_, static_cast<int>(pos.y) - halfLength_,
			static_cast<int>(pos.x) + halfLength_, static_cast<int>(pos.y) + halfLength_, GetColor(0x40, 0x40, 0x40), false);
		return true;
	}

	bool Block::unload()
	{
		--blockObjNum_;

		DeleteSoundMem(localBreakSeHandle_);

		// �Ō�̈���������{��SE���폜.
		if (blockObjNum_ == 0) {
			DeleteSoundMem(breakSeHandle_);
		}

		oft.remove();	// oft���O��.
		return true;
	}


	void Block::onAABBStay2D(Engine::AABBCollision2D* const collision)
	{
		
		if (collision->getLayer() != Engine::LAYER_PLAYER_ATTACK && collision->getLayer() != Engine::LAYER_PLAYER) {
			return;
		}

		// ��ԕ����̃`�F�b�N�̂��ߐF�ς�.
		if (collision->getLayer() == Engine::LAYER_PLAYER_ATTACK) {

			color = color | GetColor(0x00, 0xFF, 0x00);
		}
		if (collision->getLayer() == Engine::LAYER_PLAYER) {
			color = color | GetColor(0x00, 0x00, 0xFF);
		}

		// �����ɗ���̂̓{�[�������Ȃ��͂��Ȃ̂Ń_�E���L���X�g.
		//if (dynamic_cast<Ball*>(collision->getParent())->getSpeedSquare() > Ball::BREAK_SPEED) {
		/*
			for (int i = 0; i < PIECE_NUM; ++i) {
				piece[i]->setActive(true);
			}
			PlaySoundMem(localBreakSeHandle_, DX_PLAYTYPE_BACK);
			setActive(false);
			oft.remove();	// TODO ��U�A�A�N�e�B�u��Ԃ�false�ɂȂ�����o�^���� �v������.
			*/
		//}
	}
}