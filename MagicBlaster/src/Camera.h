#pragma once
#include "Engine/primitive2D.h"
#include "GameObj.h"
#include "Reference.h"

struct CameraLimit4 {
	struct CameraLimit {
		float limit = 0.0f;
		bool apply = false;
	} left, right, top, bottom;
};

class Camera : public GameObj {

private:
	GameObj* _pTarget;		// �J�����^�[�Q�b�g�̃|�C���^,
	GameObj* _pSecondTarget;
	float _zoomFactor;		// �g�嗦.
	float _rotate;			// ��].
	CameraLimit4 _cameraLimit;	// �J�������.
	double _targetZoomTime;
	float _targetZoomFactor;
	float _preZoomFactor;
	double _zoomTime;
	double _targetShakeTime;
	double _shakeTime;
	float _blend;
	float _preBlend;
	float _targetBlend;
	double _blendTime;
	double _targetBlendTime;

public:
	Camera();
	~Camera() = default;

	void Update(double delta);

	void SetTarget(GameObj* target);
	void SetSecondTarget(GameObj* target) { _pSecondTarget = target; } // TODO �g��ƃ^�[�Q�b�g�𕪂��Đ��`�⊮����.
	void SetZoomFactor(float zoomFactor);
	void SetRotate(float rotate);
	void SetCameraLimit4(CameraLimit4 limit);

	float GetZoomFactor() const;
	float GetRotate() const;
	CameraLimit4 GetCameraLimit4() const;
	Point2D GetTopLeft();
	Point2D GetBottomRight();

	void Zoom(float targetZoomFactor, double targetZoomTime);
	void BlendLerp(float targetBlend, double targetblendTime);
};