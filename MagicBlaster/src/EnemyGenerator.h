#pragma once
#include "DrawObj.h"
#include "EnemyFactory.h"
#include "CountDownTimer.h"

// TODO ジェネレータ毎に敵の上限を設定できるように EnemyFactoryと合わせて.

class EnemyGenerator : public DrawObj {
private:
	static const AnimationSequence ANIM;
	static constexpr double INIT_GENERATE_TIME = 0.5;

	static SpriteResource* _pSpriteResource;		// 描画情報のポインタ.
	SpriteHandle _spriteHandle;	// スプライトを使用するためのハンドル.
	EnemyCreateQueue _queue;						// 敵生成情報.
	bool _isGenerate;								// 生成するか否か.
	Player* _pPlayer;								// プレイヤーのポインタ.
	CountDownTimer _generateTimer;					// 生成間隔.
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