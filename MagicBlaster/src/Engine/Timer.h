#pragma once
#include <stdint.h>

/**
* 経過時間計測クラス.
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
