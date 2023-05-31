#pragma once
/// <summary>
/// 経過時間を受け取り、補正した時間を返すオブジェクトです.
/// </summary>
class TimeManager {
private:
	static TimeManager _timeManager;

	float _magnification;	// 現在の時間倍率.
	float _nextMagnification;	// 予約されている時間倍率 Updateで初めて適用される.

	double _delta;	// 経過時間.

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