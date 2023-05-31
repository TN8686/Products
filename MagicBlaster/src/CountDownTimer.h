#pragma once

class CountDownTimer {
private:
	double _time;		// ���ݎ���.
	double _targetTime;	// �ڕW����.
	bool _isLoop;		// ���[�v�t���O.
	bool _isReachedFrame;	// ���B�t���[�����ۂ�.
	bool _isStop;		// ��~�t���O.

public:
	/// <summary>
	/// �o�ߎ��Ԃ��v�����A�K�莞�ԂɒB�������ɒʒm����I�u�W�F�N�g�ł�.
	/// </summary>
	/// <param name="targetTime">�ڕW����</param>
	/// <param name="isLoop">���[�v�t���O true:���[�v���� false:���[�v�Ȃ�(�f�t�H���g)</param>
	/// <param name="startFromReach">����̌��ݎ��Ԃ̏����l�I�� true:targetTime�ŏ����� false:0�ŏ�����(�f�t�H���g)</param>
	CountDownTimer(double targetTime, bool isLoop = false, bool startFromReach = false) 
		: _targetTime(targetTime)
		, _isLoop(isLoop)
		, _isStop(false)
	{
		startFromReach ? _time = targetTime : _time = 0;
		startFromReach ? _isReachedFrame = true: _isReachedFrame = false;
	}

	/// <summary>
	/// �o�ߎ��Ԃ��J�E���g���A�ڕW���ԂɒB���Ă�����true��Ԃ��܂�.
	/// ���[�v�ݒ�ON�̏ꍇ�A���B�����t���[���̂�true��Ԃ��܂�.
	/// </summary>
	/// <param name="delta">�o�ߎ���</param>
	/// <returns>true : ���B�ς� false : �����B</returns>
	bool Update(double delta);

	// �ݒ��ύX�����A�^�C�}�[���������܂�.
	void Reset() { _time = 0; }

	/// <summary>
	/// �ڕW���Ԃ�targetTime�ɐݒ肵�A���ݎ��Ԃ����������܂�.
	/// </summary>
	/// <param name="targetTime">�ڕW����</param>
	void Set(double targetTime) {
		_time = 0;
		_targetTime = targetTime;
	}

	/// <summary>
	/// ���[�v�t���O��ݒ肵�܂�.
	/// </summary>
	/// <param name="isLoop">true:���[�v���� false:���[�v�Ȃ�</param>
	void SetIsLoop(bool isLoop) { _isLoop = isLoop; }

	// �ڕW���Ԃ܂ł̎c�莞�Ԃ�Ԃ��܂�.
	double GetTime() { return (_time >= _targetTime) ? 0 : (_targetTime - _time); }

	/// <summary>
	/// ���ݖڕW���Ԃ܂œ��B�ς݂��ǂ�����Ԃ��܂�.
	/// �Ă�ł���̂�Update�̑O���ォ�A���[�v�ݒ蓙�ɒ��ӂ��Ă�������.
	/// ���[�v�ݒ肪ON�̏ꍇ�AUpdate�ŖڕW���Ԃɓ��B���Ă��������xUpdate���Ă΂��܂ł̊�true��Ԃ��܂�.
	/// </summary>
	/// <returns>true : ���B�ς� false : �����B</returns>
	bool IsReached() { return (_time >= _targetTime) ? true : false; }

	/// <summary>
	/// ���݂̃t���[�����ڕW���Ԃ܂œ��B�����ŏ��̃t���[�����ǂ�����Ԃ��܂�.
	/// �Ă�ł���̂�Update�̑O���ォ�ɒ��ӂ��Ă�������.
	/// </summary>
	/// <returns>true : ���B�t���[�� false : ���B�t���[���łȂ�</returns>
	bool IsReachedFrame() { return _isReachedFrame; }

	// ������0�A�ڕW���Ԃ�1�Ƃ���.
	// �ڕW���Ԃɑ΂��Ă̌��݂̌o�ߎ��Ԃ̔��0�`1�ŕԂ��܂�.
	float GetRatio() { return (_time >= _targetTime || _targetTime == 0) ? 1 : static_cast<float>(_time / _targetTime); }	// ����̐�����͋N���肦�Ȃ��͂������ǔO�̂���_targetTime��0�̏ꍇ���1�ɂ���0���Z���.

	/// <summary>
	/// ���ݎ��Ԃ������I�ɐݒ肵�܂�.
	/// </summary>
	/// <param name="time">���ݎ���</param>
	void SetTime(double time) { _time = time; }

	/// <summary>
	/// �^�C�}�[���~�����AUpdate���Ă΂�Ă����Ԍo�߂��Ȃ����܂�.
	/// </summary>
	void Stop() { _isStop = true; }

	/// <summary>
	/// �^�C�}�[�̒�~��Ԃ��������܂�.
	/// </summary>
	void Start() { _isStop = false; }

};