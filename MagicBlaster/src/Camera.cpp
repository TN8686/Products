#include "Camera.h"
#include "TimeManager.h"
#include <cmath>

Camera::Camera() :
	_pTarget(nullptr),
	_zoomFactor(0.8f),
	_rotate(0.0f),
	_cameraLimit(),
	_targetZoomTime(0),
	_zoomTime(0),
	_preZoomFactor(1),
	_targetZoomFactor(1),
	_targetShakeTime(0),
	_shakeTime(0),
	_blend(0),
	_preBlend(0),
	_blendTime(1),
	_targetBlendTime(1)
{
	_cameraLimit.left.apply = true;
	_cameraLimit.left.limit = 0;
	_cameraLimit.right.apply = true;
	_cameraLimit.right.limit = clientWidth * 2;
	_cameraLimit.top.apply = true;
	_cameraLimit.top.limit = -clientHeight * 1.5f;
	_cameraLimit.bottom.apply = true;
	_cameraLimit.bottom.limit = clientHeight;

}

void Camera::BlendLerp(float targetBlend, double targetblendTime) {
	_preBlend = _blend;
	_blendTime = 0;
	_targetBlend = targetBlend;
	_targetBlendTime = targetblendTime;
};

void Camera::Zoom(float targetZoomFactor, double targetZoomTime) {
	_preZoomFactor = _zoomFactor;
	_zoomTime = 0;
	_targetZoomFactor = targetZoomFactor;
	_targetZoomTime = targetZoomTime;
};

void Camera::Update(double delta) {
	delta = TimeManager::GetInstance().GetUncorrectedDelta();
	// ズーム、回転テスト.
	/*
	static int a = 0;
	a += 1;
	if (a >= 360) {
		a -= 360;
	}*/
	//_zoomFactor = cosf(a * (3.141592 / 180)) + 2;
	//_rotate = (cosf(a * (3.141592 / 180)) + 1) * 3.141592;

	if (_pTarget != nullptr) {
		auto pos = _pTarget->GetWorldPosition();	// ターゲットの座標を取得.
		 pos.y += 88;	// カメラを少し下にズラす（ターゲット座標を上に）.

		if (_blend < 0) {
			_blend = 0;
		}else if(_blend > 1.0f) {
			_blend = 1.0f;
		}
		// アングル線形補完.
		if (_blendTime < _targetBlendTime) {
			_blendTime += delta;

			if (_blendTime >= _targetBlendTime) {
				_blend = _preBlend = _targetBlend;
			}
			else {
				_blend = _preBlend + (_targetBlend - _preBlend) * (_blendTime / _targetBlendTime);
			}
		}
		if (_pSecondTarget) {
			auto pos2 = _pSecondTarget->GetWorldPosition();
			pos = pos + (pos2 - pos) * _blend;
		}

		// 画面端までの距離を取得する.
		float halfWidth;
		float halfHeight;

		// 線形補完ズーム.
		if (_zoomTime < _targetZoomTime) {
			_zoomTime += delta;
			
			if (_zoomTime >= _targetZoomTime) {
				_zoomFactor = _preZoomFactor = _targetZoomFactor;
			}
			else {
				_zoomFactor = _preZoomFactor + (_targetZoomFactor - _preZoomFactor) * (_zoomTime / _targetZoomTime);
			}
		}

		if (_zoomFactor == 0) {	// ゼロ除算回避.
			halfWidth = 0;
			halfHeight = 0;
		}
		else {
			halfWidth = (clientWidth / 2) / _zoomFactor;
			halfHeight = (clientHeight / 2) / _zoomFactor;
		}

		// 右にスクロールしたら左に戻らない.
		//if (pos.x > _position.x) {
		//	_cameraLimit.left.limit = pos.x - clientWidth / 2;
		//}

		// スクロール上限が設定されている場合、それに応じてカメラ座標の補正値を作る.
		if (_cameraLimit.bottom.apply && (pos.y + halfHeight) - _cameraLimit.bottom.limit > 0) {	// 下.
			pos.y -= pos.y + halfHeight - _cameraLimit.bottom.limit;
		}
		if (_cameraLimit.top.apply && (pos.y - halfHeight) - _cameraLimit.top.limit < 0) {			// 上.
			pos.y -= pos.y - halfHeight - _cameraLimit.top.limit;
		}
		if (_cameraLimit.right.apply && (pos.x + halfWidth) - _cameraLimit.right.limit > 0) {		// 左.
			pos.x -= pos.x + halfWidth - _cameraLimit.right.limit;
		}
		if (_cameraLimit.left.apply && (pos.x - halfWidth) - _cameraLimit.left.limit < 0) {			// 右.
			pos.x -= pos.x - halfWidth - _cameraLimit.left.limit;
		}

		// カメラ座標の更新.
		_position.x = pos.x;
		_position.y = pos.y;

	}
}

void Camera::SetTarget(GameObj* target) {
	_pTarget = target;
}

void Camera::SetZoomFactor(float zoomFactor) {
	_zoomFactor = zoomFactor;
}

void Camera::SetRotate(float rotate) {
	_rotate = rotate;
}

void Camera::SetCameraLimit4(CameraLimit4 limit) {
	_cameraLimit = limit;
}

float Camera::GetZoomFactor() const {
	return _zoomFactor;
}

float Camera::GetRotate() const {
	return _rotate;
}

CameraLimit4 Camera::GetCameraLimit4() const {
	return _cameraLimit;
}

/*
* 画面左上の座標を返します.
* 
* @retval 画面左上座標.
*/
Point2D Camera::GetTopLeft() {
	// 画面端までの距離を取得する.
	float halfWidth;
	float halfHeight;

	if (_zoomFactor == 0) {	// ゼロ除算回避.
		halfWidth = 0;
		halfHeight = 0;
	}
	else {
		halfWidth = (clientWidth / 2) / _zoomFactor;
		halfHeight = (clientHeight / 2) / _zoomFactor;
	}

	return Point2D{ _position.x - halfWidth, _position.y - halfHeight };
}

/*
* 画面右下の座標を返します.
*
* @retval 画面右下座標.
*/
Point2D Camera::GetBottomRight() {
	// 画面端までの距離を取得する.
	float halfWidth;
	float halfHeight;

	if (_zoomFactor == 0) {	// ゼロ除算回避.
		halfWidth = 0;
		halfHeight = 0;
	}
	else {
		halfWidth = (clientWidth / 2) / _zoomFactor;
		halfHeight = (clientHeight / 2) / _zoomFactor;
	}

	return Point2D{ _position.x + halfWidth, _position.y + halfHeight };
}