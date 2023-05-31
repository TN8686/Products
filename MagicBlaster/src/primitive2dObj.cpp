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
	, color({ 0, 1, 0, 0.5f })	// ������Ԃł͗΂̔������ŉ������܂�.
	, AABB2D(collision)
	, _index(-1)
{
	Register();
}

AABB2DObj::AABB2DObj(const GameObj* pParent, const Point2D& p, const Float2& hl)
	: _onActive(true)
	, _pParent(pParent)
	, color({ 0, 1, 0, 0.5f })	// ������Ԃł͗΂̔������ŉ������܂�.
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

	// ���X�g���玩�g���폜.
	_objList.at(_index) = nullptr;		// �g�p���Ă����C���f�b�N�X�̗v�f��nullptr��.
	_freeIDList.emplace_back(_index);	// �g�p���Ă����C���f�b�N�X�𖢎g�p���X�g�ɓo�^.
}

// ������s��ꂽ���A���X�g�ւ̓o�^�̓I�u�W�F�N�g�������ɃR���X�g���N�^�ōs���邽�߁A����ȊO�̒l���R�s�[����.
AABB2DObj& AABB2DObj::operator=(const AABB2DObj& obj) {
	this->_pParent = obj._pParent;
	this->p = obj.p;
	this->hl = obj.hl;
	this->color = obj.color;
	this->_onActive = obj._onActive;

	return *this;
}

// AABB2D�v���~�e�B�u�������\�ɂ���.
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

	if (_sprList.size() < _objList.size()) {	// �X�v���C�g�����Ȃ��ꍇ.
		_sprList.resize(_objList.size());		// ��������.
	}

	int i = 0;
	for (; i < _objList.size(); ++i) {	// �X�v���C�g�ɒl���Z�b�g.
		ToSprite(&_sprList.at(i), _objList.at(i));
	}
	for (; i < _sprList.size(); ++i) {	// ���g�p�X�v���C�g���A�N�e�B�u��.
		_sprList.at(i).onActive = false;
	}

	return true;
}

// �I�u�W�F�N�g�̏������ɃX�v���C�g��ݒ肵�܂�.
void AABB2DObj::ToSprite(Sprite* sprite, const AABB2DObj* obj) {
	if (obj == nullptr) {			// �I�u�W�F�N�g�����݂��Ȃ��ꍇ.
		sprite->onActive = false;	// �Ή�����X�v���C�g���A�N�e�B�u��.
		return;
	}

	sprite->onActive = obj->GetOnActive();	// �I�u�W�F�N�g����A�N�e�B�u�Ȃ�X�v���C�g����A�N�e�B�u��

	if (!sprite->onActive) {	// ��A�N�e�B�u�Ȃ珈�����Ȃ�.
		return;
	}

	// ���W�ݒ�.
	sprite->pos.x = obj->GetWorldPosition().x;
	sprite->pos.y = obj->GetWorldPosition().y;

	// ��`�R���W�����̕��ƍ������璸�_����ݒ�.
	sprite->rectOffset.left = -obj->hl.x;
	sprite->rectOffset.right = obj->hl.x;

	sprite->rectOffset.top = -obj->hl.y;
	sprite->rectOffset.bottom = obj->hl.y;

	// �������̐F��ݒ�.
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

	// ���Ӌ�`�̍��ӂ��E�Ӌ�`�̉E�ӂ�荶.
	// ���Ӌ�`�̉E�ӂ��E�Ӌ�`�̍��ӂ��E.
	// ���Ӌ�`�̏�ӂ��E�Ӌ�`�̉��ӂ�荂��.
	// ���Ӌ�`�̉��ӂ��E�Ӌ�`�̏�ӂ��Ⴂ���A�Փ˂��Ă���.
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
		pos = pos + _pParent->GetWorldPosition();	// �e�I�u�W�F�N�g�̍��W���ċA�I�ɒǂ������Ă���.
	}
	return pos;
}

// �e�I�u�W�F�N�g�̃A�N�e�B�u��Ԃ�ǂ��Ă����A�S�ăA�N�e�B�u�Ȃ�true��Ԃ��܂�.
bool AABB2DObj::GetOnActive() const {
	if (_onActive) {						// �A�N�e�B�u�������ꍇ.
		if (_pParent != nullptr) {			// �e�I�u�W�F�N�g�����݂����.
			return _pParent->GetOnActive();	// �e�̃A�N�e�B�u��Ԃ��ċA�I�ɒǂ������Ă���.
		}
		return true;						// ���g���A�N�e�B�u�Őe�����݂��Ȃ��ꍇ��true;
	}
	return false;							// �A�N�e�B�u����Ȃ������ꍇ��false.
}

// ���g���I�u�W�F�N�g���X�g�ɓo�^���܂�.
void AABB2DObj::Register() {
	if (_freeIDList.empty()) {			// ���g�p�C���f�b�N�X���X�g����̏ꍇ.
		_index = _objList.size();		// �C���f�b�N�X��ێ��iemplace_back�̌�Ȃ�size() - 1���ǉ������C���f�b�N�X�j.
		_objList.emplace_back(this);	// ���X�g�Ɏ��g��o�^.
	}
	else {								// ���g�p�C���f�b�N�X������ꍇ.
		_index = _freeIDList.back();	// ���g�p�̃C���f�b�N�X�����pop.
		_freeIDList.pop_back();
		_objList.at(_index) = this;		// ���X�g�Ɏ��g��o�^.
	}
}