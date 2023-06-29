#pragma once
#include <windows.h>

#include "Primitive.h"
#include "../Utility/MyArray.h"

// ���[�g�������𗘗p������ԕ����ɂ��Փ˔���

namespace Engine {

	// ���C���[.
	enum OFTLayer {
		OFTLAYER_DEFAULT,
		OFTLAYER_PLAYER,
		OFTLAYER_ITEM,
		OFTLAYER_ENEMY_ATTACK,
		OFTLAYER_WALL,
		OFTLAYER_NUM
	};

	// �O���錾.
	template <class T>
	class Cell;

	//////////////////////////////
	// ���ؓo�^�I�u�W�F�N�g(OFT).
	//////////////////////////////
	template< class T>
	class ObjectForTree
	{
		// �����o�ϐ�.
	public:
		int id_;					// ID.
		Cell<T>* pCell_;			// �o�^���.
		T* pObject_;				// ����ΏۃI�u�W�F�N�g.
		ObjectForTree<T>* pPre_;	// �O��ObjectForTree�\����.
		ObjectForTree<T>* pNext_;	// ����ObjectForTree�\����.
		OFTLayer layer_;

		// �R���X�g���N�^.
	public:
		ObjectForTree(int id = 0)
			: id_(id)
			, pCell_(nullptr)
			, pObject_(nullptr)
			, pPre_(nullptr)
			, pNext_(nullptr)
			, layer_(OFTLAYER_DEFAULT)
		{}

		// �f�X�g���N�^.
		virtual ~ObjectForTree() {
			remove();
		}

		// ����.
	public:
		// ���烊�X�g����O���.
		bool remove() {
			// ���łɈ�E���Ă��鎞�͏����I��.
			if (pCell_ == nullptr) {
				return false;
			}

			// ������o�^���Ă����ԂɎ��g��ʒm.
			if (!pCell_->onRemove(this)) {
				return false;
			}

			// ��E����.
			// �O��̃I�u�W�F�N�g�����т���.
			if (pPre_ != nullptr) {
				pPre_->pNext_ = pNext_;
			}
			if (pNext_ != nullptr) {
				pNext_->pPre_ = pPre_;
			}
			pPre_ = nullptr;
			pNext_ = nullptr;
			pCell_ = nullptr;

			return true;
		}

	};

	//////////////////////////////////
	// �Փ˃��X�g �|�C���^��ێ�����V���v���ȃR���e�i�N���X
	//////////////////////////////////
	template < class T >
	class CollisionList {
		static constexpr int COLLISIONLIST_REALLOCSIZE = 100;
	private:
		T** root_;		// ���X�g���[�g�|�C���^.
		size_t	pos_;
		size_t	allocSize_;

	public:
		CollisionList() 
			: root_(nullptr)
			, pos_(0)
			, allocSize_(0) 
		{
			root_ = (T**)malloc(0);
		}
		~CollisionList() {
			reflesh();
		}

		size_t getSize() {
			return pos_;
		}

		T** getRootPtr() {
			return root_;
		}

		void resetPos() {
			pos_ = 0;
		}

		void wright(T* obj1, T* obj2) {
			if (pos_ >= allocSize_) {
				root_ = (T**)realloc(root_, sizeof(T*) * (allocSize_ + COLLISIONLIST_REALLOCSIZE));
				allocSize_ += COLLISIONLIST_REALLOCSIZE;
			}
			root_[pos_++] = obj1;
			root_[pos_++] = obj2;
		}

		void reflesh() {
			if (root_) {
				free(root_);
			}
		}

	};

	////////////////////////////
	// ���`4���؋�ԊǗ��N���X.
	////////////////////////////

	// �萔.
	namespace {
		constexpr unsigned int LINER4TREEMANAGER_MAXLEVEL = 9;	// ��ԕ������x���̏��.
	}

	template <class T>
	class Liner4TreeManager {

		// �q�b�g���郌�C���[�̑g�����̃��X�g TODO ���ڂ����Œ�`���ׂ��ł͂Ȃ����A������������.
		bool combinationList_[15] = {

			//		de		pl		it		at		wl
			/*de*/	false,	false,	false,	false,	false,
			/*pl*/			false,	true,	true,	true,
			/*it*/					false,	false ,	false,
			/*at*/							false,	true,
			/*wl*/									false,
		};

		// �����o�ϐ�.
	protected:
		Cell<T>** ppCellAry_;								// ���`��ԃ|�C���^�z��.
		unsigned int pow_[LINER4TREEMANAGER_MAXLEVEL + 1];	// �ׂ��搔�l�z��.
		Vector2 widthHight_;								// �̈�̕��ƍ���.
		Vector2 topLeft_;									// �̈�̊�_ ����.
		Vector2 unitWidthHight_;							// �ŏ����x����Ԃ̕��ƍ���.
		DWORD cellNum_;										// ��Ԃ̐�.
		unsigned int level_;								// �ŉ��ʃ��x��.
		CollisionList<T> colList_;							// �Փ˃��X�g
		Utility::MyArray<ObjectForTree<T>*> colStac_;		// �Փ˃I�u�W�F�N�g�X�^�b�N.

		// �R���X�g���N�^.
	public:
		Liner4TreeManager()
			: ppCellAry_(nullptr)
			, widthHight_{0.0f, 0.0f}
			, topLeft_{0.0f, 0.0f}
			, unitWidthHight_{0.0f, 0.0f}
			, cellNum_(0)
			, level_(0)
			, colStac_(128)
		{
			// �e���x���ł̋�Ԑ����Z�o
			int i;
			pow_[0] = 1;	// ���[�g��Ԃ̐��Ȃ̂�1
			for (i = 1; i < LINER4TREEMANAGER_MAXLEVEL + 1; i++) {
				pow_[i] = pow_[i - 1] * 4;		// 1�O��4�{�����Ă����@1, 4, 16, 64...
			}
		}

		// �f�X�g���N�^.
		~Liner4TreeManager() {
			// ��Ԃ�j��.
			for (unsigned int i = 0; i < cellNum_; ++i) {
				if (ppCellAry_[i] != nullptr) {
					delete ppCellAry_[i];
				}
			}
			delete[] ppCellAry_;
		}


		// ����.
	public:
		// ���`4���ؔz����\�z����.
		bool init(unsigned int level, float left, float top, float right, float bottom) {
			// �ݒ�ō����x���ȏ�̋�Ԃ͍��Ȃ�
			if (level >= LINER4TREEMANAGER_MAXLEVEL) {
				return false;
			}

			// ��Ԃ����݂��Ă���ꍇ��x�j��.
			if (ppCellAry_ != nullptr) {
				for (unsigned int i = 0; i < cellNum_; ++i) {
					if (ppCellAry_[i] != nullptr) {
						delete ppCellAry_[i];
					}
				}
				delete[] ppCellAry_;
			}

			// 0��_��level���̔z��쐬.
			cellNum_ = (pow_[level + 1] - 1) / 3;	// ((4 ^ n) - 1) / (4 - 1)
			ppCellAry_ = new Cell<T>*[cellNum_] { nullptr };

			// �̈��o�^.
			topLeft_.x = left;
			topLeft_.y = top;
			widthHight_.x = right - left;
			widthHight_.y = bottom - top;
			unitWidthHight_.x = widthHight_.x / (1 << level);	// ���x�����ɉ������l�Ŋ���.
			unitWidthHight_.y = widthHight_.y / (1 << level);
			level_ = level;

			return true;
		}


		bool regist(Rect rect, ObjectForTree<T>* pOFT) {
			return regist(rect.topLeft.x, rect.topLeft.y, rect.bottomRight.x, rect.bottomRight.y, pOFT);
		}

		// �I�u�W�F�N�g��o�^����.
		bool regist(float left, float top, float right, float bottom, ObjectForTree<T>* pOFT) {
			
			// ����̈�Ɣ�r.
			// ����́A���E���ɋ߂��l��I��.
			left = max(left, topLeft_.x);
			top = max(top, topLeft_.y);

			// �E���́A��荶��ɋ߂��l��I��.
			right = min(right, topLeft_.x + widthHight_.x - 1);
			bottom = min(bottom, topLeft_.y + widthHight_.y - 1);

			// �̈���ɑ��݂��镔����������Γo�^���Ȃ�.
			if (left > right || top > bottom) {
				return false;
			}

			DWORD elem = getMortonNumber(left, top, right, bottom);
			if (elem < cellNum_) {
				// ��Ԃ������ꍇ�͐V�K�쐬.
				if (ppCellAry_[elem] == nullptr) {
					createNewCell(elem);
				}
				return ppCellAry_[elem]->push(pOFT);
			}
			return false;	// �o�^���s.
		}

		// �Փ˔��胊�X�g���쐬����.
		DWORD getAllCollisionList(CollisionList<T>** colList) {

			// ���X�g�i�z��j�͕K�����������܂�.
			colList_.resetPos();

			// ���[�g��Ԃ̑��݂��`�F�b�N.
			if (ppCellAry_[0] == nullptr) {
				return 0;	// ��Ԃ����݂��Ȃ�.
			}
			getCollisionList(0, colStac_);

			*colList = &colList_;

			return static_cast<DWORD>(colList_.getSize());
		}

	protected:
		// ��ԓ��ŏՓ˃��X�g���쐬����.
		DWORD getCollisionList(DWORD elem, Utility::MyArray<ObjectForTree<T>*>& colStac) {

			// ������ԓ��̃I�u�W�F�N�g���m�̏Փ˃��X�g�쐬.
			ObjectForTree<T>* pOFT1 = ppCellAry_[elem]->getFirstObj();

			// ��d���[�v�őS�Ă̑g�ݍ��킹�����X�g�A�b�v���Ă���
			while (pOFT1 != nullptr) {	// ��ڂ̃I�u�W�F�N�g��H�郋�[�v.
				ObjectForTree<T>* pOFT2 = pOFT1->pNext_;

				while (pOFT2 != nullptr) {	// ��ڂ̌�ɑ����I�u�W�F�N�g��H�郋�[�v.
					if (layerCheck(pOFT1->layer_, pOFT2->layer_)) {	// �Փ˂����郌�C���[���ǂ������`�F�b�N.
						colList_.wright(pOFT1->pObject_, pOFT2->pObject_);
					}
					pOFT2 = pOFT2->pNext_;
				}

				// �Փ˃X�^�b�N�Ƃ̏Փ˃��X�g�쐬.
				auto p = colStac.begin();
				auto end = colStac.end();
				for (; p != end; ++p) {
					if (layerCheck(pOFT1->layer_, (*p)->layer_)) {	// �Փ˂����郌�C���[���ǂ������`�F�b�N.
						colList_.wright(pOFT1->pObject_, (*p)->pObject_);
					}
				}
				pOFT1 = pOFT1->pNext_;
			}

			bool childFlag = false;

			// �q��ԂɈړ�.
			DWORD objNum = 0;
			DWORD i, nextElem;

			for (i = 0; i < 4; ++i) {
				nextElem = elem * 4 + 1 + i;	// ���݂̗v�f * 4 + 1�Ŏq��Ԃ̍ŏ��̗v�f���w���A + i�Ŏl�����ɏ���.
				if (nextElem < cellNum_ && ppCellAry_[nextElem] != nullptr) {
					if (!childFlag) {
						// �o�^�I�u�W�F�N�g���X�^�b�N�ɒǉ�.
						pOFT1 = ppCellAry_[elem]->getFirstObj();
						while (pOFT1 != nullptr) {					// ��ڂ̃I�u�W�F�N�g��H�郋�[�v.
							colStac.push(pOFT1);
							++objNum;
							pOFT1 = pOFT1->pNext_;
						}
					}
					childFlag = true;
					getCollisionList(nextElem, colStac);	// �ċA�I�Ɏq��Ԃ�.
				}
			}

			// �X�^�b�N����I�u�W�F�N�g���O��.
			if (childFlag) {
				for (i = 0; i < objNum; ++i) {
					colStac.pop();
				}
			}

			return true;
		}

		// ��Ԃ𐶐�.
		bool createNewCell(DWORD elem)
		{
			while (ppCellAry_[elem] == nullptr)
			{
				ppCellAry_[elem] = new Cell<T>;	// ��Ԃ𐶐�.

				// �e��ԂɃW�����v.
				elem = (elem - 1) >> 2;
				if (elem >= cellNum_) {	// ����0(���[�g���)���������ꂽ�ꍇ�A���̌�elem���I�[�o�[�t���[���Ă��̏����𖞂���.
					break;
				}
			}

			return true;
		}

		// ���W�����Ԕԍ����Z�o.
		DWORD getMortonNumber(float left, float top, float right, float bottom) {
			//�ŏ����x���ɂ�����e���ʒu���Z�o.
			DWORD lt = getPointElem(left, top);
			DWORD rb = getPointElem(right, bottom);

			// ��Ԕԍ��̔r���I�_���a���珊�����x�����Z�o.
			// ���E�}�`�̏������镪�����x���͂��̔r���I�_���a�̌��ʃr�b�g��0����Ȃ��ŏ�ʂ̋�؂�ɕ\�������.
			DWORD def = rb ^ lt;
			unsigned int hiLevel = 0;
			for (unsigned int i = 0; i < level_; ++i) {
				DWORD check = (def >> (i * 2)) & 0b11;
				if (check != 0) {		// �}�X�N�̌���0����Ȃ���������.
					hiLevel = i + 1;	// �����ō����x�����X�V.
				}
			}

			// ��Ԕԍ����Z�o.
			DWORD spaceNum = rb >> (hiLevel * 2);			// �E���̏����ԍ������x�� * 2�̕��V�t�g.
			spaceNum += (pow_[level_ - hiLevel] - 1) / 3;	// ���x���ɉ�������Ԕԍ��̃I�t�Z�b�g�l�����o�����Z ((4 ^ n) - 1) / (4 - 1).

			if (spaceNum > cellNum_) {	// �Z�o���ʂ��p�ӂ��Ă���Z������葽��.
				return 0xffffffff;		// �G���[��Ԃ�.
			}
			return spaceNum;

		}

		// �r�b�g�����֐�.
		DWORD bitSeparate32(DWORD n) {
			// �Y�����Č���or�����A�}�X�N���鑀����J��Ԃ�.
			n = (n | (n << 8)) & 0x00ff00ff;	// 0b00000000111111110000000011111111
			n = (n | (n << 4)) & 0x0f0f0f0f;	// 0b0000111100001111...
			n = (n | (n << 2)) & 0x33333333;	// 0b00110011...
			return (n | (n << 1)) & 0x55555555;	// 0b0101...
		}

		// 2D���[�g����Ԕԍ��Z�o�֐�.
		WORD get2DMortonNumber(WORD x, WORD y) {
			return static_cast<WORD>(bitSeparate32(x) | (bitSeparate32(y) << 1));	// x��y���番��bit�����Ay����1���V�t�g����or�ň��.
		}

		// ���W������`4���ؗv�f�ԍ��ɕϊ�����֐�.
		DWORD getPointElem(float posX, float posY) {
			// x,y���ꂼ����W�̑��΂���̈��̍��W���o���A�����P�ʕ��Ŋ����ē_�̏����̈���o��.
			return get2DMortonNumber(
				static_cast<WORD>((posX - topLeft_.x) / unitWidthHight_.x),
				static_cast<WORD>((posY - topLeft_.y) / unitWidthHight_.y));
		}

		// �`�F�b�N���s�����C���[���m���ǂ�����Ԃ��܂�.
		bool layerCheck(OFTLayer a, OFTLayer b) {

			// �C���f�b�N�X�Q�l
			//		de	pl	pat	bl	wl
			//de*/	0,	1,	2,	3,	4,
			//pl*/		5,	6,	7,	8,
			//pat*/			9,	10,	11,
			//bl*/				12,	13,
			//wl*/					14,

			// a���������Ȓl�ɂȂ�悤�ɂ���.
			if (a > b) {
				auto tmp = a;
				a = b;
				b = tmp;
			}

			if (a < 0 || b >= OFTLAYER_NUM) {	// �͈͊O�`�F�b�N.
				return false;
			}

			int num1 = OFTLAYER_NUM * (OFTLAYER_NUM + 1) / 2;			// OFTLAYER_NUM�̑g�����̐�.
			int num2 = (OFTLAYER_NUM - a) * (OFTLAYER_NUM - a + 1) / 2;	// OFTLAYER_NUM - a�̑g�����̐�.
			return combinationList_[(num1 - num2) + (b - a)];			// num1 - num2 �ŁA�g����aa��ID���o���A������b - a�𑫂����őg����ab�̈ʒu���o���Ă���.

		}
	};


	/////////////////////////
	// �������ꂽ��ԃN���X.
	/////////////////////////
	template <class T>
	class Cell
	{
	protected:
		ObjectForTree<T>* pLatest_;	// �ŐVOFT�̃|�C���^.

	public:
		// �R���X�g���N�^.
		Cell()
			: pLatest_(nullptr)
		{}

		// �f�X�g���N�^.
	public:
		virtual ~Cell()
		{
			if (pLatest_ != nullptr) {
				resetLink(pLatest_);
			}
		}

		// ����.
	public:
		// �����N��S�ă��Z�b�g����.
		void resetLink(ObjectForTree<T>* pOFT)
		{
			if (pOFT->pNext_ != nullptr) {
				resetLink(pOFT->pNext_);
			}
			pOFT = nullptr;
		}

		// OFT���v�b�V��.
		bool push(ObjectForTree<T>* pOFT)
		{
			if (pOFT == nullptr) {			// �����I�u�W�F�N�g�͓o�^���Ȃ�.
				return false;
			}
			if (pOFT->pCell_ == this) {		// ��d�o�^�`�F�b�N.
				return false;
			}

			if (pLatest_ == nullptr) {		// �ŐVOFT������.
				pLatest_ = pOFT;			// ��ԂɐV�K�o�^.
			}
			else {							// �ŐVOFT������.
				pOFT->pNext_ = pLatest_;	// �ŐV�I�u�W�F�N�g���X�V ���܂ł̍ŐV���ڂ�.
				pLatest_->pPre_ = pOFT;		
				pLatest_ = pOFT;			// �ŐV��V��������.
			}
			pOFT->pCell_ = this;			// ��Ԃ�o�^.

			return true;
		}

		// �폜�����I�u�W�F�N�g���`�F�b�N.
		bool onRemove(ObjectForTree<T>* pRemoveObj)
		{
			if (pLatest_ == pRemoveObj) {			// ���̋�Ԃ̍ŐV�I�u�W�F�N�g��������.
				if (pLatest_ != nullptr) {
					pLatest_ = pLatest_->pNext_;	// ���̃I�u�W�F�N�g�ɑ}���ւ�.
				}
			}
			return true;
		}

		// ����.
	public:
		ObjectForTree<T>* getFirstObj()
		{
			return pLatest_;
		}

	};
}