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
	// �Y�[���A��]�e�X�g.
	/*
	static int a = 0;
	a += 1;
	if (a >= 360) {
		a -= 360;
	}*/
	//_zoomFactor = cosf(a * (3.141592 / 180)) + 2;
	//_rotate = (cosf(a * (3.141592 / 180)) + 1) * 3.141592;

	if (_pTarget != nullptr) {
		auto pos = _pTarget->GetWorldPosition();	// �^�[�Q�b�g�̍��W���擾.
		 pos.y += 88;	// �J�������������ɃY�����i�^�[�Q�b�g���W����Ɂj.

		if (_blend < 0) {
			_blend = 0;
		}else if(_blend > 1.0f) {
			_blend = 1.0f;
		}
		// �A���O�����`�⊮.
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

		// ��ʒ[�܂ł̋������擾����.
		float halfWidth;
		float halfHeight;

		// ���`�⊮�Y�[��.
		if (_zoomTime < _targetZoomTime) {
			_zoomTime += delta;
			
			if (_zoomTime >= _targetZoomTime) {
				_zoomFactor = _preZoomFactor = _targetZoomFactor;
			}
			else {
				_zoomFactor = _preZoomFactor + (_targetZoomFactor - _preZoomFactor) * (_zoomTime / _targetZoomTime);
			}
		}

		if (_zoomFactor == 0) {	// �[�����Z���.
			halfWidth = 0;
			halfHeight = 0;
		}
		else {
			halfWidth = (clientWidth / 2) / _zoomFactor;
			halfHeight = (clientHeight / 2) / _zoomFactor;
		}

		// �E�ɃX�N���[�������獶�ɖ߂�Ȃ�.
		//if (pos.x > _position.x) {
		//	_cameraLimit.left.limit = pos.x - clientWidth / 2;
		//}

		// �X�N���[��������ݒ肳��Ă���ꍇ�A����ɉ����ăJ�������W�̕␳�l�����.
		if (_cameraLimit.bottom.apply && (pos.y + halfHeight) - _cameraLimit.bottom.limit > 0) {	// ��.
			pos.y -= pos.y + halfHeight - _cameraLimit.bottom.limit;
		}
		if (_cameraLimit.top.apply && (pos.y - halfHeight) - _cameraLimit.top.limit < 0) {			// ��.
			pos.y -= pos.y - halfHeight - _cameraLimit.top.limit;
		}
		if (_cameraLimit.right.apply && (pos.x + halfWidth) - _cameraLimit.right.limit > 0) {		// ��.
			pos.x -= pos.x + halfWidth - _cameraLimit.right.limit;
		}
		if (_cameraLimit.left.apply && (pos.x - halfWidth) - _cameraLimit.left.limit < 0) {			// �E.
			pos.x -= pos.x - halfWidth - _cameraLimit.left.limit;
		}

		// �J�������W�̍X�V.
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
* ��ʍ���̍��W��Ԃ��܂�.
* 
* @retval ��ʍ�����W.
*/
Point2D Camera::GetTopLeft() {
	// ��ʒ[�܂ł̋������擾����.
	float halfWidth;
	float halfHeight;

	if (_zoomFactor == 0) {	// �[�����Z���.
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
* ��ʉE���̍��W��Ԃ��܂�.
*
* @retval ��ʉE�����W.
*/
Point2D Camera::GetBottomRight() {
	// ��ʒ[�܂ł̋������擾����.
	float halfWidth;
	float halfHeight;

	if (_zoomFactor == 0) {	// �[�����Z���.
		halfWidth = 0;
		halfHeight = 0;
	}
	else {
		halfWidth = (clientWidth / 2) / _zoomFactor;
		halfHeight = (clientHeight / 2) / _zoomFactor;
	}

	return Point2D{ _position.x + halfWidth, _position.y + halfHeight };
}