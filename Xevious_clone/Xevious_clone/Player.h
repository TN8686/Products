#pragma once

#include "Xevious_clone.h"
#include "SoundManager.h"

// とりあえずプレイヤークラスを作ってみる
class Player {
public:
	Player(SoundManager* pSoundManager);			// コンストラクタ宣言

	// 初期化
	void Init();
	void Restart();

	// Get関数
	float GetX();		// X座標
	float GetY();		// Y座標
	int GetStock();		// 残機
	bool GetOnActive();

	// Set関数
	void SetIsHit(bool isHit);

	void Extend();		// エクステンド

	void Calc(char keyBuf[], int padBuf, int frameCount);	// 計算
	void Draw();		// 描画

	// 定数
	static constexpr int PLAYER_IMG_DIVSION_NUM = 6;	// 画像分割数
	static constexpr int PLAYER_SIZE = 16;		// 自機の縦横サイズ
private:
	static constexpr int BROKEN_IMG_DIVSION_NUM = 7;	// 画像分割数
	static constexpr int BROKEN_IMG_SIZE = 32;		// 自機の縦横サイズ

	static constexpr float CURSOR_RANGE = 96;
	static constexpr float Y_MAX = CHARACTER_SIZE + CHARACTER_SIZE + CURSOR_RANGE + TEMP_OFFSCREEN_HEIGHT;

	static constexpr int STOCK_INIT = 2;

	int m_width;		// 幅
	int m_height;		// 高さ
	float m_x;			// X座標
	float m_y;			// Y座標
	float m_speed;		// Y方向、斜め方向への移動速度
	float m_speed_x;	// X方向のみへの移動速度
	int m_image[PLAYER_IMG_DIVSION_NUM];		// 画像ハンドル
	int m_brokenImage[BROKEN_IMG_DIVSION_NUM];	// 破壊画像ハンドル
	int m_stock;		// 残機
	bool m_onActive;	// アクティブ状態
	bool m_isHit;		// ヒット情報
	bool m_isInvincible; // 主にデバッグ用無敵フラグ
	bool m_isDraw;		// 主にデバッグ用描画フラグ
	bool m_isFlip;		// 破壊描画時の反転フラグ
	int m_brokenAnimID;	// 破壊アニメーションID
	int m_frameCount;	// 破壊アニメーション用フレームカウンタ

	SoundManager* m_pSoundManager;	// サウンドマネージャポインタ

	void BrokenCalc(int frameCount);	// 破壊計算
	void BrokenDraw();	// 破壊描画
};