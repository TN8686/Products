#include "MagicCircle.h"
#include <cmath>

void MagicCircle::Update(double delta){
	if (_onActive) {
		if (_startUp) {
			_time -= delta;
			if (_time < 0) {
				_time = 0;
			}
		}
		else {
			_time += delta;
			if (_time > TIME_MAX) {
				_time = TIME_MAX;
				_onActive = false;
			}
		}
	}
}

void MagicCircle::SpriteUpdate(double delta) {
	if (!_onActive) {
		_pSprite->onActive = false;
		return;
	}
	_pSprite->animeController.Update(delta);

	if (_pSprite != nullptr) {
		float w = static_cast<float>(_time / TIME_MAX);
		float a = 1.2f - 0.45f * (1.0f - static_cast<float>(_time / TIME_MAX));
		_pSprite->addColor = { w, w, w, 0 };
		_pSprite->color[0] = _pSprite->color[1] = {1, 1, 1, a};

		// À•WXV.
		auto pos = GetWorldPosition();
		_pSprite->pos.x = pos.x;
		_pSprite->pos.y = pos.y;
	}
}