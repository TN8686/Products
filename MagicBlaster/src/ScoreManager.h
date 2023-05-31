#pragma once

#include<memory>
#include "CountDownTimer.h"

namespace {
	constexpr int SCORE_MAX = 99999999;				// スコア上限.
	constexpr float BONUS_MAX = 8.0f;				// ボーナス上限.

	constexpr double COMBO_TIME = 5.0;				// コンボ受付時間.
	constexpr double SIMULTANEOUS_TIME = 8.0 / 60;	// 同時撃破受付時間.
}

class ScoreManager {
private:
	static ScoreManager _instance;
	int _score;
	int _highScore;
	int _combo;
	float _bonus;
	int _stockScore;					// 適用前の蓄積スコア.
	CountDownTimer _comboTimer;
	CountDownTimer _simultaneousTimer;

	// シングルトン.
	ScoreManager()
		: _score(0)
		, _highScore(0)
		, _combo(0)
		, _bonus(1.0f)
		, _stockScore(0)
		, _comboTimer(COMBO_TIME, false, true)
		, _simultaneousTimer(SIMULTANEOUS_TIME, false, true)
	{};	
public:
	static ScoreManager& GetIncetance() { return _instance; }

	void Init();

	// ゲッター.
	int GetScore() const { return _score; }
	int GetHighScore() const { return _highScore; }
	int GetCombo() const { return _combo; }
	float GetBonus() const { return _bonus; }

	void AddScore(int num);
	void AddCombo(int num);
	void Update(double delta);
	void ComboGaugeReset() { _comboTimer.Reset(); }
	void ComboBreak() { _combo = 0; }
	bool HighScoreSet();

};