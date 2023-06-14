#pragma once

#include "Player.h"
#include "Debug.h"
#include "EnemyManager.h"
#include "Enemy.h"
#include "BulletManager.h"
#include "SoundManager.h"
#include "Cursor.h"
#include "HitChecker.h"
#include "ScoreManager.h"
#include "BackGround.h"
#include "AreaManager.h"
#include "UI.h"

class GameManager
{
public:
	GameManager(bool isDebug);
	~GameManager();

	// メインループ　実行すると脱出するまで内部でループし続けます
	int MainLoop();

	// 初期化関数
	int TitleInit();
	int MainGameInit();
	int NameEntryInit();

	// 計算関数
	int TitleCalc();
	int MainGameCalc();
	int NameEntryCalc();

	// 描画関数
	int TitleDraw();
	int MainGameDraw();
	int NameEntryDraw();

private:
	static constexpr int TEMP_SCREEN_NUM = 3;				// 仮画面の数
	static constexpr int MICROSECONDS_PER_FRAME = 16500;	// 1フレームあたりのマイクロ秒
	static constexpr int MISS_FRAME = 88;					// ミスウェイトフレーム
	static constexpr int RESTART_FRAME = 74;				// リスタートウェイトフレーム
	static constexpr int GAME_OVER_FRAME = 128;				// 「GAME OVER」表示のフレーム

	bool m_isDebug;						// デバッグモードフラグ
	bool m_isResetFirst;				// リセットキー初回フラグ

	int tempScreen[TEMP_SCREEN_NUM];	// 仮画面のハンドル用
	int uiScreen;						// UI画面ハンドル用
	long long int firstTime;			// 時間計測用
	unsigned int frameCount;			// 全体のフレームカウント
	int mode;							// 現在のモード
	bool isStartBgmAfter;				// 開始時BGM終了後フラグ

	int missFrameCount;					// ミス時に使用するフレームカウント
	bool isRestart;						// リスタート中フラグ
	int restartFrameCount;				// リスタート時に使用するフレームカウント
	bool isGameOver;					// ゲームオーバー表示フラグ
	int gameOverCount;					// ゲームオーバー表示フレームカウント


	char keyBuf[256];	// キー入力受け取り用
	int padBuf;			// パッド入力受け取り用

	Debug* debuger;					// デバッガー
	SoundManager* soundManager;		// サウンドマネージャ
	AreaManager* areaManager;		// エリアマネージャ
	Player* player;					// プレイヤー
	BackGround* backGround;			// バックグラウンド
	ScoreManager* scoreManager;		// スコアマネージャ
	BulletManager* bulletManager;	// バレットマネージャ
	EnemyManager* enemyManager;		// エネミーマネージャ
	HitChecker* hitChecker;			// ヒットチェッカー
	UI* ui;
};