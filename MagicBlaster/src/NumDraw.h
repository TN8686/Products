#pragma once

#include "Engine/Sprite.h"
#include "Engine/primitive2d.h"
//#include "PtrVectorMgr.h"
namespace NumDraw {
	namespace {
		constexpr int IMAGE_X = 16;
		constexpr int IMAGE_Y = 24;

		constexpr int RESERVE_DEFAULT = 32;
	}

	enum Alignment {
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT,
		ALIGNMENT_MAX
	};

	enum VerticalAlign {
		VER_ALIGN_TOP,
		VER_ALIGN_CENTER,
		VER_ALIGN_BOTTOM,
		VERTICAL_ALIGN_MAX
	};

	struct Color {
		float r, g, b, a;	// �����x���������l1.
	};

	class Param
	{
	public:
		unsigned long long int num;							// �`�悵�����l.
		Point2D position;									// �`���_.
		Alignment alignment = ALIGN_LEFT;				// �A���C�������g.
		VerticalAlign verticalAlign = VER_ALIGN_TOP;	// �㉺�A���C�������g.
		Float2 scale = { 1.0f, 1.0f };						// �X�P�[��.
		unsigned digit = 0;									// �`�悵���������@�l�ɑ΂��ĕs�����Ă����疳������܂��B������Ώ��0��`��.
		bool cameraRelative = true;							// �J�����̉e���̗L��.
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };			// �F.
	};

	class NumDraw {
	private:
		SpriteResource2 _spriteResouce;		// ���\�[�X�@���̂܂܎g���񂾂Ǝd�l�㖳�ʂɉ�肭�ǂ��Ȃ��Ă镔�������邯�ǂǂ����悤����.
		std::vector<SpriteObj> _spriteList;	// �g�p����X�v���C�g���X�g.
		unsigned int _spriteNum;			// �g�p���Ă���X�v���C�g�̐�.

		unsigned GetDigit(unsigned long long num);	// �����̎擾.
		SpriteObj& GetNextSprite();					// ���Ɏg�p����X�v���C�g�̎擾.
		void SetParam(SpriteObj& sprite, const Param& param, const Point2D& position);	// �p�����[�^�̃Z�b�g.

	public:
		NumDraw()
			: _spriteNum(0)
		{}
		bool Load(size_t reserveNum = RESERVE_DEFAULT);				// ���[�h.
		void ReadyToDraw();			// �`��O�̏���������.
		void SetDrawParam(const Param& param);	// �`������Z�b�g.

		// �Q�b�^�[.
		const SpriteResource2* pGetSpriteResource() const { return &_spriteResouce; }
	};
}