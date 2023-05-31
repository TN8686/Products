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
		float r, g, b, a;	// 透明度だけ初期値1.
	};

	class Param
	{
	public:
		unsigned long long int num;							// 描画したい値.
		Point2D position;									// 描画基準点.
		Alignment alignment = ALIGN_LEFT;				// アラインメント.
		VerticalAlign verticalAlign = VER_ALIGN_TOP;	// 上下アラインメント.
		Float2 scale = { 1.0f, 1.0f };						// スケール.
		unsigned digit = 0;									// 描画したい桁数　値に対して不足していたら無視されます。多ければ上に0を描画.
		bool cameraRelative = true;							// カメラの影響の有無.
		Color color = { 1.0f, 1.0f, 1.0f, 1.0f };			// 色.
	};

	class NumDraw {
	private:
		SpriteResource2 _spriteResouce;		// リソース　このまま使うんだと仕様上無駄に回りくどくなってる部分があるけどどうしようかな.
		std::vector<SpriteObj> _spriteList;	// 使用するスプライトリスト.
		unsigned int _spriteNum;			// 使用しているスプライトの数.

		unsigned GetDigit(unsigned long long num);	// 桁数の取得.
		SpriteObj& GetNextSprite();					// 次に使用するスプライトの取得.
		void SetParam(SpriteObj& sprite, const Param& param, const Point2D& position);	// パラメータのセット.

	public:
		NumDraw()
			: _spriteNum(0)
		{}
		bool Load(size_t reserveNum = RESERVE_DEFAULT);				// ロード.
		void ReadyToDraw();			// 描画前の初期化処理.
		void SetDrawParam(const Param& param);	// 描画情報をセット.

		// ゲッター.
		const SpriteResource2* pGetSpriteResource() const { return &_spriteResouce; }
	};
}