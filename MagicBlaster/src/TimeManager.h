#pragma once
/// <summary>
/// �o�ߎ��Ԃ��󂯎��A�␳�������Ԃ�Ԃ��I�u�W�F�N�g�ł�.
/// </summary>
class TimeManager {
private:
	static TimeManager _timeManager;

	float _magnification;	// ���݂̎��Ԕ{��.
	float _nextMagnification;	// �\�񂳂�Ă��鎞�Ԕ{�� Update�ŏ��߂ēK�p�����.

	double _delta;	// �o�ߎ���.

	TimeManager() :
		_nextMagnification(1.0),
		_magnification(1.0),
		_delta(0)
	{};

public:
	static TimeManager& GetInstance() { return _timeManager; }

	void Update(double delta) {
		_delta = delta;
		_magnification = _nextMagnification;
	}
	TimeManager(const TimeManager&) = delete;
	const TimeManager& operator=(const TimeManager&) = delete;

	void SetNextMagnification(float magnification) { _nextMagnification = magnification; }
	double GetUncorrectedDelta() const { return _delta; }
	double GetCorrectedDelta() const { return _delta * _magnification; }
	void Reset() {
		_nextMagnification = 1.0;
		_magnification = 1.0;
		_delta = 0;
	};
};