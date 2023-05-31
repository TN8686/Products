#include "CountDownTimer.h"

bool CountDownTimer::Update(double delta) {
	// 分かり辛かったので、現在時間が目標時間に向かっていく形に変更（CountUpだけど…）.

	_isReachedFrame = false;	// Updateの度falseにし続ける.

	if (_isStop) {
		return IsReached();
	}

	// ループ設定ONの場合、目標時間に到達してからもう一度Updateが呼ばれて時間経過した時に初めてループさせるためにここで処理.
	if (_isLoop && _time >= _targetTime)
	{
		_time -= _targetTime;
	}
	

	// その後経過時間を加算.
	if (_time < _targetTime) {
		_time += delta;
	}
	else {
		return true;	// 加算以前から到達している場合、ここでtrueを返す.
	}

	// 最後に目標時間に到達しているかを判定して返す.
	if (_time >= _targetTime)
	{
		_isReachedFrame = true;	// 加算後に到達しているので、初めて到達したフレームという事になる.
		return true;
	}
	return false;
}