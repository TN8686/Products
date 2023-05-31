#include "CountDownTimer.h"

bool CountDownTimer::Update(double delta) {
	// ������h�������̂ŁA���ݎ��Ԃ��ڕW���ԂɌ������Ă����`�ɕύX�iCountUp�����ǁc�j.

	_isReachedFrame = false;	// Update�̓xfalse�ɂ�������.

	if (_isStop) {
		return IsReached();
	}

	// ���[�v�ݒ�ON�̏ꍇ�A�ڕW���Ԃɓ��B���Ă��������xUpdate���Ă΂�Ď��Ԍo�߂������ɏ��߂ă��[�v�����邽�߂ɂ����ŏ���.
	if (_isLoop && _time >= _targetTime)
	{
		_time -= _targetTime;
	}
	

	// ���̌�o�ߎ��Ԃ����Z.
	if (_time < _targetTime) {
		_time += delta;
	}
	else {
		return true;	// ���Z�ȑO���瓞�B���Ă���ꍇ�A������true��Ԃ�.
	}

	// �Ō�ɖڕW���Ԃɓ��B���Ă��邩�𔻒肵�ĕԂ�.
	if (_time >= _targetTime)
	{
		_isReachedFrame = true;	// ���Z��ɓ��B���Ă���̂ŁA���߂ē��B�����t���[���Ƃ������ɂȂ�.
		return true;
	}
	return false;
}