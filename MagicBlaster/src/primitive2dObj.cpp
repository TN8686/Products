#include "primitive2dObj.h"
#include "Engine/Collision.h"
#include "GameObj.h"

std::vector<AABB2DObj*> AABB2DObj::_objList;
std::vector<std::size_t> AABB2DObj::_freeIDList;
std::vector<Sprite> AABB2DObj::_sprList;
std::vector<Sprite> AABB2DObj::_dummySprList;
bool AABB2DObj::_isDraw = false;

AABB2DObj::AABB2DObj(const GameObj* pParent, const AABB2D& collision)
	: _onActive(true)
	, _pParent(pParent)
	, color({ 0, 1, 0, 0.5f })	// 初期状態では緑の半透明で可視化します.
	, AABB2D(collision)
	, _index(-1)
{
	Register();
}

AABB2DObj::AABB2DObj(const GameObj* pParent, const Point2D& p, const Float2& hl)
	: _onActive(true)
	, _pParent(pParent)
	, color({ 0, 1, 0, 0.5f })	// 初期状態では緑の半透明で可視化します.
	, AABB2D(p, hl)
	, _index(-1)
{
	Register();
}

AABB2DObj::AABB2DObj(const AABB2DObj& obj)
	: _onActive(obj._onActive)
	, _pParent(obj._pParent)
	, color(obj.color)
	, AABB2D(obj.p, obj.hl)
	, _index(-1)
{
	Register();
}

AABB2DObj::~AABB2DObj() {
	if (_index < 0) {
		return;
	}
	if (_index >= _objList.size()) {
		return;
	}

	// リストから自身を削除.
	_objList.at(_index) = nullptr;		// 使用していたインデックスの要素をnullptrに.
	_freeIDList.emplace_back(_index);	// 使用していたインデックスを未使用リストに登録.
}

// 代入が行われた時、リストへの登録はオブジェクト生成時にコンストラクタで行われるため、それ以外の値をコピーする.
AABB2DObj& AABB2DObj::operator=(const AABB2DObj& obj) {
	this->_pParent = obj._pParent;
	this->p = obj.p;
	this->hl = obj.hl;
	this->color = obj.color;
	this->_onActive = obj._onActive;

	return *this;
}

// AABB2Dプリミティブから代入可能にする.
AABB2DObj& AABB2DObj::operator=(const AABB2D& rect)
{
	this->p = rect.p;
	this->hl = rect.hl;

	return *this;
}

bool AABB2DObj::DrawBegin() {
	if (!_isDraw) {
		return false;
	}

	if (_sprList.size() < _objList.size()) {	// スプライトが少ない場合.
		_sprList.resize(_objList.size());		// 同じ数に.
	}

	int i = 0;
	for (; i < _objList.size(); ++i) {	// スプライトに値をセット.
		ToSprite(&_sprList.at(i), _objList.at(i));
	}
	for (; i < _sprList.size(); ++i) {	// 未使用スプライトを非アクティブに.
		_sprList.at(i).onActive = false;
	}

	return true;
}

// オブジェクトの情報を元にスプライトを設定します.
void AABB2DObj::ToSprite(Sprite* sprite, const AABB2DObj* obj) {
	if (obj == nullptr) {			// オブジェクトが存在しない場合.
		sprite->onActive = false;	// 対応するスプライトを非アクティブに.
		return;
	}

	sprite->onActive = obj->GetOnActive();	// オブジェクトが非アクティブならスプライトも非アクティブに

	if (!sprite->onActive) {	// 非アクティブなら処理しない.
		return;
	}

	// 座標設定.
	sprite->pos.x = obj->GetWorldPosition().x;
	sprite->pos.y = obj->GetWorldPosition().y;

	// 矩形コリジョンの幅と高さから頂点情報を設定.
	sprite->rectOffset.left = -obj->hl.x;
	sprite->rectOffset.right = obj->hl.x;

	sprite->rectOffset.top = -obj->hl.y;
	sprite->rectOffset.bottom = obj->hl.y;

	// 可視化時の色を設定.
	sprite->addColor = { obj->color.r, obj->color.g ,obj->color.b ,obj->color.a };
}


bool AABB2DObj::RectOnRect(const AABB2DObj& sa, const AABB2DObj& sb) {
	if (!sa.GetOnActive() || !sb.GetOnActive()) {
		return false;
	}

	auto posa = sa.GetWorldPosition();
	auto posb = sb.GetWorldPosition();

	struct {
		float top, bottom, right, left;
	}rect1, rect2;

	rect1.top = posa.y - sa.hl.y;
	rect1.bottom = posa.y + sa.hl.y;
	rect1.right = posa.x + sa.hl.x;
	rect1.left = posa.x - sa.hl.x;

	rect2.top = posb.y - sb.hl.y;
	rect2.bottom = posb.y + sb.hl.y;
	rect2.right = posb.x + sb.hl.x;
	rect2.left = posb.x - sb.hl.x;

	// 左辺矩形の左辺が右辺矩形の右辺より左.
	// 左辺矩形の右辺が右辺矩形の左辺より右.
	// 左辺矩形の上辺が右辺矩形の下辺より高く.
	// 左辺矩形の下辺が右辺矩形の上辺より低い時、衝突している.
	if (rect1.left < rect2.right &&
		rect1.right > rect2.left &&
		rect1.top < rect2.bottom &&
		rect1.bottom > rect2.top) {
		return true;
	}

	return false;
}


Point2D AABB2DObj::GetWorldPosition() const {
	Point2D pos = p;
	if (_pParent != nullptr) {
		pos = pos + _pParent->GetWorldPosition();	// 親オブジェクトの座標を再帰的に追いかけていく.
	}
	return pos;
}

// 親オブジェクトのアクティブ状態を追っていき、全てアクティブならtrueを返します.
bool AABB2DObj::GetOnActive() const {
	if (_onActive) {						// アクティブだった場合.
		if (_pParent != nullptr) {			// 親オブジェクトが存在すれば.
			return _pParent->GetOnActive();	// 親のアクティブ状態を再帰的に追いかけていく.
		}
		return true;						// 自身がアクティブで親が存在しない場合はtrue;
	}
	return false;							// アクティブじゃなかった場合はfalse.
}

// 自身をオブジェクトリストに登録します.
void AABB2DObj::Register() {
	if (_freeIDList.empty()) {			// 未使用インデックスリストが空の場合.
		_index = _objList.size();		// インデックスを保持（emplace_backの後ならsize() - 1が追加したインデックス）.
		_objList.emplace_back(this);	// リストに自身を登録.
	}
	else {								// 未使用インデックスがある場合.
		_index = _freeIDList.back();	// 未使用のインデックスを一つpop.
		_freeIDList.pop_back();
		_objList.at(_index) = this;		// リストに自身を登録.
	}
}