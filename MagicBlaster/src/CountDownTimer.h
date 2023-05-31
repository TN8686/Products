#pragma once

class CountDownTimer {
private:
	double _time;		// 現在時間.
	double _targetTime;	// 目標時間.
	bool _isLoop;		// ループフラグ.
	bool _isReachedFrame;	// 到達フレームか否か.
	bool _isStop;		// 停止フラグ.

public:
	/// <summary>
	/// 経過時間を計測し、規定時間に達した時に通知するオブジェクトです.
	/// </summary>
	/// <param name="targetTime">目標時間</param>
	/// <param name="isLoop">ループフラグ true:ループあり false:ループなし(デフォルト)</param>
	/// <param name="startFromReach">初回の現在時間の初期値選択 true:targetTimeで初期化 false:0で初期化(デフォルト)</param>
	CountDownTimer(double targetTime, bool isLoop = false, bool startFromReach = false) 
		: _targetTime(targetTime)
		, _isLoop(isLoop)
		, _isStop(false)
	{
		startFromReach ? _time = targetTime : _time = 0;
		startFromReach ? _isReachedFrame = true: _isReachedFrame = false;
	}

	/// <summary>
	/// 経過時間をカウントし、目標時間に達していたらtrueを返します.
	/// ループ設定ONの場合、到達したフレームのみtrueを返します.
	/// </summary>
	/// <param name="delta">経過時間</param>
	/// <returns>true : 到達済み false : 未到達</returns>
	bool Update(double delta);

	// 設定を変更せず、タイマー初期化します.
	void Reset() { _time = 0; }

	/// <summary>
	/// 目標時間をtargetTimeに設定し、現在時間を初期化します.
	/// </summary>
	/// <param name="targetTime">目標時間</param>
	void Set(double targetTime) {
		_time = 0;
		_targetTime = targetTime;
	}

	/// <summary>
	/// ループフラグを設定します.
	/// </summary>
	/// <param name="isLoop">true:ループあり false:ループなし</param>
	void SetIsLoop(bool isLoop) { _isLoop = isLoop; }

	// 目標時間までの残り時間を返します.
	double GetTime() { return (_time >= _targetTime) ? 0 : (_targetTime - _time); }

	/// <summary>
	/// 現在目標時間まで到達済みかどうかを返します.
	/// 呼んでいるのがUpdateの前か後か、ループ設定等に注意してください.
	/// ループ設定がONの場合、Updateで目標時間に到達してからもう一度Updateが呼ばれるまでの間trueを返します.
	/// </summary>
	/// <returns>true : 到達済み false : 未到達</returns>
	bool IsReached() { return (_time >= _targetTime) ? true : false; }

	/// <summary>
	/// 現在のフレームが目標時間まで到達した最初のフレームかどうかを返します.
	/// 呼んでいるのがUpdateの前か後かに注意してください.
	/// </summary>
	/// <returns>true : 到達フレーム false : 到達フレームでない</returns>
	bool IsReachedFrame() { return _isReachedFrame; }

	// 初期を0、目標時間を1として.
	// 目標時間に対しての現在の経過時間の比を0～1で返します.
	float GetRatio() { return (_time >= _targetTime || _targetTime == 0) ? 1 : static_cast<float>(_time / _targetTime); }	// 現状の性質上は起こりえないはずだけど念のため_targetTimeが0の場合常に1にして0除算回避.

	/// <summary>
	/// 現在時間を強制的に設定します.
	/// </summary>
	/// <param name="time">現在時間</param>
	void SetTime(double time) { _time = time; }

	/// <summary>
	/// タイマーを停止させ、Updateが呼ばれても時間経過しなくします.
	/// </summary>
	void Stop() { _isStop = true; }

	/// <summary>
	/// タイマーの停止状態を解除します.
	/// </summary>
	void Start() { _isStop = false; }

};