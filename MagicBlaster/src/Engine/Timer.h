#pragma once
#include <stdint.h>

/**
* �o�ߎ��Ԍv���N���X.
*/
class Timer
{
public:
	Timer();
	double GetFrameDelta();
	double GetFPS() const;

private:
	double frequency;
	int64_t lastFrameTime;
	double fpsDelta;
	double fpsFrames;
	double fps;
};
