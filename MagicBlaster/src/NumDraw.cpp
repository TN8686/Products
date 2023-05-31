#include "NumDraw.h"
#include "Engine/Graphics.h"

// private

// 桁数取得　0の場合0が返ってくる事に注意.
unsigned NumDraw::NumDraw::GetDigit(unsigned long long num) {
	unsigned digit = 0;
	while (num != 0) {
		num /= 10;
		digit++;
	}
	return digit;
}

SpriteObj& NumDraw::NumDraw::GetNextSprite()
{
	auto size = _spriteList.size();			// 現状のサイズを取得.
	bool isCapacityShortage = false;
	if (_spriteNum < size) {				// これから使用するスプライトが存在しているなら.
		return _spriteList.at(_spriteNum);	// そのままそのインデックスの要素を渡す.
	}
	else {													// サイズが不足していた場合.
		
		if (_spriteNum >= _spriteList.capacity()) {	// さらにリソースのメモリが不足していた場合　できる事なら、ここに入らないように事前リザーブをすること.
			// コピーコンストラクタの自動を使うと新オブジェクトを全て登録→旧オブジェクトを全て登録解除の順になり、メモリを無暗に多く使う可能性があるので一旦全て登録解除.
			for (auto& e : _spriteList) {
				e.Unregister();
			}
			isCapacityShortage = true;
		}

		_spriteList.emplace_back();							// 新たな要素を追加.

		if (isCapacityShortage) {
			for (auto& e : _spriteList) {					// 全て登録	メモリ確保はvectorに任せる.
				_spriteResouce.Register(&e);
			}
		}
		else {
			_spriteResouce.Register(&_spriteList.at(size));	// その要素を登録 末尾は追加前のサイズがそのままインデックスにできる.
		}
		return _spriteList.at(size);						// その要素を返す.
	}
}

void NumDraw::NumDraw::SetParam(SpriteObj& sprite, const Param& param, const Point2D& position)
{
	// アクティブ状態に.
	sprite.SetOnActive(true);

	// スケールを設定.
	sprite.scale.x = param.scale.x;
	sprite.scale.y = param.scale.y;

	// カメラに影響されるどうかを設定.
	sprite.cameraRelative = param.cameraRelative;

	// 座標設定.
	sprite.SetLocalPosition(position);

	sprite.color = { param.color.r, param.color.g, param.color.b, param.color.a };	// 透明度だけ - 1した値にする事で、透明度をそのまま画像に適用する.
}


// public

// ロード.
bool NumDraw::NumDraw::Load(size_t reserveNum)
{
	if (!Graphics::Get().texMap.LoadFromFile(_spriteResouce.GetTexture(), L"Res/num_01.png")) {
		return false;
	}

	// スプライトの領域を確保.
	_spriteResouce.Reserve(reserveNum);
	_spriteList.reserve(reserveNum);

	for (auto& e : _spriteList) {
		_spriteResouce.Register(&e);
	}

	// セルリストの作成.
	if (_spriteResouce.CreateCellList(10, 1, IMAGE_X, IMAGE_Y) != 0) {
		return false;
	};
	return true;
}

// 描画前の初期化処理.
void NumDraw::NumDraw::ReadyToDraw()
{
	for (auto itr = _spriteList.begin() + _spriteNum; itr != _spriteList.end(); ++itr) {	// _spriteNum以降の要素を非アクティブに.
		itr->SetOnActive(false);
	}
	_spriteNum = 0;	// スプライトの使用数を初期化.
}

void NumDraw::NumDraw::SetDrawParam(const Param& param)
{
	// 桁数を取得.
	unsigned digit = GetDigit(param.num);
	unsigned maxDigit = (digit > param.digit ? digit : param.digit);	// 表示桁数と比較して、より大きい方を取得.

	// 最大0の場合、桁数1とする.
	if (maxDigit == 0) {
		maxDigit = 1;
	}

	float length = param.scale.x * IMAGE_X;								// 一文字あたりの幅.
	float allLength = static_cast<float>(maxDigit) * length;			// 全体幅.

	// 始点を選択.
	auto position = param.position;
	switch (param.alignment)
	{
	case ALIGN_LEFT:
		position.x += allLength;		// 幅の分ズラす.
		break;

	case ALIGN_CENTER:
		position.x += allLength * 0.5f;	// 幅の半分の分ズラす.
		break;

	case ALIGN_RIGHT:				// なにもしない.
		break;

	default:
		assert(false);					// 異常な値.
		break;
	}
	position.x -= length * 0.5f;		// 一文字幅の半分ズラす.

	switch (param.verticalAlign)
	{
	case VER_ALIGN_TOP:
		position.y += IMAGE_Y * param.scale.y * 0.5f;	// 高さの半分の分ズラす.
		break;

	case VER_ALIGN_CENTER:			// なにもしない.
		break;

	case VER_ALIGN_BOTTOM:
		position.y -= IMAGE_Y * param.scale.y * 0.5f;	// 高さの半分の分ズラす.
		break;

	default:
		assert(false);					// 異常な値.
		break;
	}

	auto num = param.num;

	unsigned i = 0;
	for (; i < digit; ++i) {
		auto& sprite = GetNextSprite();	// 次のスプライトを取得.

		assert(num != 0);				// 仕様上0は来ないはず….		
		sprite.animeController.SetCellIndex(num % 10);	// 現在の1桁目を取り出し画像を選択.
		num /= 10;										// 桁を一つ減らす.


		SetParam(sprite, param, position);

		position.x -= length;				// 座標を数字一つ分左へ.

		++_spriteNum;						// スプライトの使用数をインクリメント.
	}

	// 残りの部分には0を表示.
	for (; i < maxDigit; ++i) {
		auto& sprite = GetNextSprite();	// 次のスプライトを取得.

		sprite.animeController.SetCellIndex(0);			// 0の画像を選択.

		SetParam(sprite, param, position);

		position.x -= length;				// 座標を数字一つ分左へ.

		++_spriteNum;						// スプライトの使用数をインクリメント.
	}
}