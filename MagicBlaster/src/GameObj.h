#pragma once

#include "Engine/primitive2d.h"

class GameObj {
protected:
	bool _onActive;
	Point2D _position;
	const GameObj* _pParent;

public:
	GameObj(Point2D position = {0, 0}) : _onActive(true), _position(position) , _pParent(nullptr)	{};
	virtual ~GameObj() = default;

	virtual void SetOnActive(bool onActive) { _onActive = onActive; }
	void SetLocalPosition(Point2D position) { _position = position; }
	void SetWorldPosition(Point2D position) {
		if (_pParent != nullptr) {
			_position = position - _pParent->GetWorldPosition();
		}
		else {
			_position = position;
		}
	}
	void SetParent(const GameObj* parent){ _pParent = parent; }

	virtual bool GetOnActive() const { 
		if (_onActive) {	// アクティブだった場合.
			if (_pParent != nullptr) {	// 親オブジェクトが存在すれば.
				return _pParent->GetOnActive();	// 親のアクティブ状態をチェックして返す.
			}
			return true;	// 親が存在しない場合はtrue.
		}	
		return false;// アクティブじゃなかった場合はfalse.
	}

	Point2D GetLocalPosition() const { return _position; }
	Point2D GetWorldPosition() const {
		Point2D pos = _position;
		if (_pParent != nullptr) {
			pos = pos + _pParent->GetWorldPosition();
		}
		return pos;
	}

	virtual void Update(double delta) {};
};