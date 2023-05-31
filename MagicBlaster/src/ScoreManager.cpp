#include "ScoreManager.h"
//#include "TimeManager.h"

ScoreManager ScoreManager::_instance;

void ScoreManager::AddScore(int num) {
	_stockScore += num;					// スコアを蓄積.
	_simultaneousTimer.Reset();			// 同時撃破受付タイマーを開始.
}

void ScoreManager::AddCombo(int num) {
	_combo += num;
}

void ScoreManager::Update(double delta)
{
	// タイマーアップデート.
	_comboTimer.Update(delta);
	_simultaneousTimer.Update(delta);

	if (_comboTimer.IsReachedFrame()) {			// コンボタイマーが到達していたら.
		_combo = 0;	// コンボを初期化.
	};


	_bonus = 1.0f + (static_cast<float>(_combo * _combo) * 0.0007f);	// 仮のコンボボーナス算出.

	if (_bonus > BONUS_MAX) {	// ボーナス上限.
		_bonus = BONUS_MAX;
	}

	if (_simultaneousTimer.IsReachedFrame()) {	// 同時撃破受付が終わったら.
		
		_score += (_stockScore * _bonus);		// ボーナス倍率を掛け、スコア加算(端数切捨て)
		_stockScore = 0;						// 蓄積スコアを初期化.

		
		if (_score > SCORE_MAX) {				// 上限チェック.
			_score = SCORE_MAX;
		}
		else if (_score < 0) {					// 下限チェック.
			_score = 0;
		}
	}
}

void ScoreManager::Init()
{
	_score = 0;
	_combo = 0;
	_bonus = 1.0f;
	_stockScore = 0;
	_comboTimer.SetTime(COMBO_TIME);
	_simultaneousTimer.SetTime(SIMULTANEOUS_TIME);
}

bool ScoreManager::HighScoreSet()
{
	if (_score > _highScore) {
		_highScore = _score;
		return true;
	}

	return false;
}
