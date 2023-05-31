#pragma once
#include "DrawObj.h"

class MagicCircle : public DrawObj {
private:
	static constexpr double TIME_MAX = 8.0 / 60;

	bool _startUp;
	double _time;
public:
	MagicCircle() : _startUp(false), _time(TIME_MAX) {};
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
	void SetStartUp(bool startUp) { _startUp = startUp; }
};