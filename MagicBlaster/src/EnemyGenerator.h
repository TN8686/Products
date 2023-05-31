#pragma once
#include "DrawObj.h"
#include "EnemyFactory.h"
#include "CountDownTimer.h"

// TODO �W�F�l���[�^���ɓG�̏����ݒ�ł���悤�� EnemyFactory�ƍ��킹��.

class EnemyGenerator : public DrawObj {
private:
	static const AnimationSequence ANIM;
	static constexpr double INIT_GENERATE_TIME = 0.5;

	static SpriteResource* _pSpriteResource;		// �`����̃|�C���^.
	SpriteHandle _spriteHandle;	// �X�v���C�g���g�p���邽�߂̃n���h��.
	EnemyCreateQueue _queue;						// �G�������.
	bool _isGenerate;								// �������邩�ۂ�.
	Player* _pPlayer;								// �v���C���[�̃|�C���^.
	CountDownTimer _generateTimer;					// �����Ԋu.
	double _generateTime;

public:
	EnemyGenerator()
		: _generateTimer(INIT_GENERATE_TIME, true)
		, _isGenerate(true)
		, _pPlayer(nullptr)
		, _generateTime(INIT_GENERATE_TIME)
	{}
	~EnemyGenerator() = default;

	static bool Load();
	static void pSetSpriteResource(SpriteResource* pSprRes) { _pSpriteResource = pSprRes; }
	static SpriteResource* pGetSpriteResource() { return _pSpriteResource; }

	bool Init();
	void pSetPlayer(Player* player) { _pPlayer = player; }
	void Update(double delta) override;
	void SpriteUpdate(double delta) override;
};