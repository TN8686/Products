#include "ScoreManager.h"
//#include "TimeManager.h"

ScoreManager ScoreManager::_instance;

void ScoreManager::AddScore(int num) {
	_stockScore += num;					// �X�R�A��~��.
	_simultaneousTimer.Reset();			// �������j��t�^�C�}�[���J�n.
}

void ScoreManager::AddCombo(int num) {
	_combo += num;
}

void ScoreManager::Update(double delta)
{
	// �^�C�}�[�A�b�v�f�[�g.
	_comboTimer.Update(delta);
	_simultaneousTimer.Update(delta);

	if (_comboTimer.IsReachedFrame()) {			// �R���{�^�C�}�[�����B���Ă�����.
		_combo = 0;	// �R���{��������.
	};


	_bonus = 1.0f + (static_cast<float>(_combo * _combo) * 0.0007f);	// ���̃R���{�{�[�i�X�Z�o.

	if (_bonus > BONUS_MAX) {	// �{�[�i�X���.
		_bonus = BONUS_MAX;
	}

	if (_simultaneousTimer.IsReachedFrame()) {	// �������j��t���I�������.
		
		_score += (_stockScore * _bonus);		// �{�[�i�X�{�����|���A�X�R�A���Z(�[���؎̂�)
		_stockScore = 0;						// �~�σX�R�A��������.

		
		if (_score > SCORE_MAX) {				// ����`�F�b�N.
			_score = SCORE_MAX;
		}
		else if (_score < 0) {					// �����`�F�b�N.
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
