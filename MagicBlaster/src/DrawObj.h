#pragma once
#include "GameObj.h"
#include "Engine/Sprite.h"
#include "assert.h"

// TODO �X�v���C�g�֘A����蒼����������F�X�Ƒg�݂Ȃ����K�v�����肻��.

// �X�v���C�g�A�A�j���[�V�������X�g�A�Z�����X�g�A�e�N�X�`���ɂ���.
// �O���܂���static�ȃ����o�ϐ��Ƃ���List�����A�X�v���C�g�͌ʂɊ��蓖�āA�A�j���[�V�������X�g�̓X�v���C�g�ɐݒ�.
// �Z�����X�g�A�e�N�X�`����Draw�֐��Ɉ����n���ĉ�����.

class DrawObj : public virtual GameObj {
protected:
	Sprite* _pSprite;		// �g�p����X�v���C�g�����蓖�Ăĉ����� TODO �j���\�� �����SpriteResource��SpriteHandorer���Z�b�g�Ŏg�p���Ă�������.
	Camera* _pCamera;

public:
	DrawObj(Point2D position = {0, 0}) :
		GameObj(position),
		_pSprite(nullptr),
		_pCamera(nullptr)		
	{};
	virtual ~DrawObj() = default;


	virtual void SpriteUpdate(double delta) = 0;

	void SetOnActive(bool onActive) override { 
		_onActive = onActive;
		assert(_pSprite != nullptr);
		if (_pSprite != nullptr) {
			_pSprite->onActive = onActive;
		}
	}
	void SetSprOnActive(bool onActive) {
		assert(_pSprite != nullptr);
		if (_pSprite != nullptr) {
			_pSprite->onActive = onActive;
		}
	}
	void pSetSprite(Sprite* sprite) { _pSprite = sprite; }
	void pSetCamera(Camera* pCamera) { _pCamera = pCamera; }

	bool GetSprOnActive() const { return _pSprite->onActive; }

};