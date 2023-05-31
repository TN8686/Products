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
		if (_onActive) {	// �A�N�e�B�u�������ꍇ.
			if (_pParent != nullptr) {	// �e�I�u�W�F�N�g�����݂����.
				return _pParent->GetOnActive();	// �e�̃A�N�e�B�u��Ԃ��`�F�b�N���ĕԂ�.
			}
			return true;	// �e�����݂��Ȃ��ꍇ��true.
		}	
		return false;// �A�N�e�B�u����Ȃ������ꍇ��false.
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