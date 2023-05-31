#include "CollisionManager.h"
#include "GameObject.h"

#include "CollisionTrees.h"

namespace Engine {

	using namespace Utility;

	CollisionManager CollisionManager::instace_;

	CollisionManager::~CollisionManager() {
		clear();		// �ň��N���A�֐��͎��s�����悤�ɂ��Ă���.
	}

	bool CollisionManager::initialize()
	{
		static constexpr int INIT_CAPASITY = 1024;	// ���̏����m�ې��̈搔.

		// �S�Ẵ��C���[���ɗ̈���m��.
		for (int i = 0; i < LAYER_NUM; ++i) {
			aabbCls2DList_[i].reserve(INIT_CAPASITY);
		}


		// ���͕s�v.
		{	// �����͊O�����烍�[�h����悤�ɂ��ׂ��ȋC������

			// �g�����̐����Z�o.
			combinationList_.resize(LAYER_NUM * (LAYER_NUM + 1) / 2);

			// TODO �����Ń`�F�b�N���X�g���쐬.
		}

		if (!l4tree_.init(4, -32, -32, (float)WindowSize::WIDTH + 32, (float)WindowSize::HEIGHT + 32)) {
			return false;	// �l���ؐ������s.
		}

		return true;
	}


	bool CollisionManager::createAABBCollision(GameObject* const parent, const CllisionLayer& layer, AABBCollision2D*& ref) {

		if (layer >= LAYER_NUM || layer < 0 || parent == nullptr) {	// ���C���[�Ɛe�I�u�W�F�N�g�̐���l�`�F�b�N.
			return false;
		}

		
		auto* p = new AABBCollision2D();		// ����.
		p->setParent(parent);					// �e�I�u�W�F�N�g���֘A�t��.
		p->setLayer(layer);						// ���C���[���Z�b�g.
		ref = p;								// �ԋp�p�����Ƀ|�C���^��n��.

		aabbCls2DList_[layer].emplace_back(p);	// ���X�g�ɓo�^�@TODO �󂫂�T������d�g�݂�ǉ�.
		

		return true;
	}

	// �S�Ă̓����蔻��̃`�F�b�N���s���܂�.
	// combinationList����Ƀ��C���[���m�Ńq�b�g�`�F�b�N���s���܂�.
	// ���̃Q�[�����[�v�ň�x�������s���Ă�������.
	bool CollisionManager::allHitCheck()
	{
		static CollisionList<AABBCollision2D>* pList;
		auto colNum = l4tree_.getAllCollisionList(&pList);
		colNum /= 2;

		AABBCollision2D** pRoot = pList->getRootPtr();
		for (unsigned int i = 0; i < colNum; ++i) {
			if (AABBOnAABB(*pRoot[i * 2], *pRoot[i * 2 + 1])) {
				// �q�b�g�֐����s.
				pRoot[i * 2]->getParent()->onAABBStay2D(pRoot[i * 2 + 1]);
				pRoot[i * 2 + 1]->getParent()->onAABBStay2D(pRoot[i * 2]);
			}
		}

		return true;
	}

	// �\���I�Ȍʂ�AABB2D�q�b�g�`�F�b�N���s���܂�.
	// combinationList�ɂ�����ݒ�͖�������A�w�背�C���[�̃I�u�W�F�N�g���`�F�b�N���܂�.
	// �������ɒ��ӂ��Ă��������i�I�u�W�F�N�gA��update�ŗ��p�����ہA�I�u�W�F�N�gB�̃A�b�v�f�[�g���܂��̏ꍇ�ɑz�肳�ꂽ�ʒu�ɖ����ꍇ������܂��j.
	bool CollisionManager::activeHitCheckAABB2D(AABBCollision2D* const collision, const CllisionLayer& layer)
	{
		if (layer < 0 || layer >= LAYER_NUM) {	// ���C���[�s���`�F�b�N.
			return false;
		}

		for (auto& e : aabbCls2DList_[layer]) {
			if (!e->getActive() || collision == e) {	// �A�N�e�B�u��Ԃ��`�F�b�N�A�����I�u�W�F�N�g�����`�F�b�N.
				continue;
			}
			if (AABBOnAABB(*collision, *e)) {
				// ���ꂼ��̃q�b�g���֐����s�@TODO Enter��Exit������������.
				collision->getParent()->onAABBStay2D(e);
				e->getParent()->onAABBStay2D(collision);
			}
		}

		return false;
	}

	// �S�Ă̓����蔻���`�悵�܂�.
	bool CollisionManager::render()
	{
		unsigned int c = 0;
		for (int i = 0; i < LAYER_NUM; ++i) {

			// ���C���[�ŐF��ς���.
			c = COLOR[i];

			for (auto& e : aabbCls2DList_[i]) {
				if (!e->getActive()) {
					continue;
				}
				e->render(c);
			}
		}
		return true;
	}

	// �S�Ă̓����蔻��I�u�W�F�N�g��j�����܂�
	// �K���S�ẴQ�[���I�u�W�F�N�g�̔j�����s���Ă�����s���Ă�������.
	bool CollisionManager::clear()
	{
		
		// �S�Ă̗v�f���폜.
		for (int i = 0; i < LAYER_NUM; ++i) {
			for (auto& e : aabbCls2DList_[i]) {
				delete e;						// �ʂ̓����蔻��I�u�W�F�N�g��delete.
			}
			aabbCls2DList_[i].clear();			// vector�z����N���A.
			aabbCls2DList_[i].shrink_to_fit();	// �����������.
		}

		return true;
	}

	bool CollisionManager::AABBOnAABB(AABBCollision2D a, AABBCollision2D b)
	{

		// �X�P�[���𔽉f����`�ɕϊ�.
		Rect rect1, rect2;
		rect1 = a.getWorldAABB2D();
		rect2 = b.getWorldAABB2D();

		return	rect1.topLeft.y < rect2.bottomRight.y &&
				rect1.bottomRight.y > rect2.topLeft.y &&
				rect1.topLeft.x < rect2.bottomRight.x &&
				rect1.bottomRight.x > rect2.topLeft.x;
	}

	bool CollisionManager::CircleOnCircle(CircleCollision2D a, CircleCollision2D b)
	{
		auto ca = a.getWorldCircle2D();
		auto cb = b.getWorldCircle2D();

		// �~1�̌��_����~2�̌��_�܂ł̋������o���̔��a�̍��v������������.
		return	(ca.position.x - cb.position.x) * (ca.position.x - cb.position.x) +
				(ca.position.y - cb.position.y) * (ca.position.y - cb.position.y) <
				(ca.radius + cb.radius) * (ca.radius + cb.radius);
	}

}