#pragma once
#include <vector>

#include "../Utility/Reference.h"
#include "Collision.h"
#include "CollisionTrees.h"

namespace Engine {

	// TODO
	// �����̃��X�g���n���h���ŊǗ��ł���悤��.
	// �ʂ̓����蔻��I�u�W�F�N�g��j��.
	// �x�N�g���ɑΉ��H.

	// �O���錾.
	class GameObject;

	class CollisionManager {
		// �����o�ϐ�.
	private:
		// �V���O���g��.
		static CollisionManager instace_;

		std::vector<AABBCollision2D*> aabbCls2DList_[LAYER_NUM];	// ���C���[������AABB�R���W�������X�g.

		Liner4TreeManager<AABBCollision2D> l4tree_;	// ���̎l���؋�ԕ����Ǘ�.

		/*
			// ���C���[.
	enum CllisionLayer {
		LAYER_DEFAULT,
		LAYER_PLAYER,
		LAYER_PLAYER_ATTACK,
		LAYER_BLOCK,
		LAYER_WALL,
		LAYER_NUM
	};
		*/

		std::vector<bool> combinationList_ = {					// allHitCheck�Ń`�F�b�N���郌�C���[�̑g�����̃��X�g TODO ���ڂ����Œ�`���ׂ��ł͂Ȃ����A������������.
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


		// �R���X�g���N�^.
	private:
		CollisionManager()
		{};

		// �f�X�g���N�^.
	public:
		~CollisionManager();

		// ����.
	public:
		// �V���O���g�� �I�u�W�F�N�g���擾.
		static CollisionManager* getInstance() { return &instace_; }

		// ������ �N�����Ɏ��s���Ă�������.
		bool initialize();
		
		// �R���W�����I�u�W�F�N�g�𐶐�.
		bool createAABBCollision(GameObject* const parent, const CllisionLayer &layer, AABBCollision2D*& ref);

		// �S�Ă̓����蔻��̃`�F�b�N���s���܂�.
		// combinationList����Ƀ��C���[���m�Ńq�b�g�`�F�b�N���s���܂�.
		// ���̃Q�[�����[�v�ň�x�������s���Ă�������.
		bool allHitCheck();

		// �\���I�ȃq�b�g�`�F�b�N���s���܂�.
		// combinationList�ɂ�����ݒ�͖�������A�w�背�C���[�̃I�u�W�F�N�g���`�F�b�N���܂�.
		// �������ɒ��ӂ��Ă��������i�I�u�W�F�N�gA��update�ŗ��p�����ہA�I�u�W�F�N�gB�̃A�b�v�f�[�g���܂��̏ꍇ�ɑz�肳�ꂽ�ʒu�ɖ����ꍇ������܂��j.
		bool activeHitCheckAABB2D(AABBCollision2D* const collision, const CllisionLayer &layer);

		// �S�Ă̓����蔻���`�悵�܂�.
		bool render();

		// �S�Ă̓����蔻��I�u�W�F�N�g��j�����܂�
		// �K���S�ẴQ�[���I�u�W�F�N�g�̔j�����s���Ă�����s���Ă�������.
		bool clear();

		// AABB���m�̓����蔻����s���܂�.
		static bool AABBOnAABB(AABBCollision2D a, AABBCollision2D b);

		// AABB���m�̓����蔻����s���܂�.
		static bool CircleOnCircle(CircleCollision2D a, CircleCollision2D b);


		// ����.
	public:
		Liner4TreeManager<AABBCollision2D>* getL4Tree() { return &l4tree_; }

	};
}