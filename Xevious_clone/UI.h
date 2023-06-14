#pragma once

#include <string>
#include "Xevious_clone.h"
#include "ScoreManager.h"
#include "Player.h"

enum class ColorName : int {
	RED,
	YELLOW,
	ORANGE,
	STOCK
};

class UI
{
public:
	UI(const int uiScreen, ScoreManager* scoreManagerP, Player* playerP, const int* padBuf);
	void Init();
	int NameEntryInit();
	int Calc(int mode);
	void Draw(int mode, int frameCount, bool isGameOver);
	int SetRankingText();
	void SetIsDrawHiddenMessage(bool isDrawHiddenMessage);

private:
	static constexpr int UI_TEXT_BUF = 32;

	static constexpr int M_FONT_NUM = 256;					// フォント画像配列数
	static constexpr int M_FONT_IMAGE_DIVISION_NUM = 16;	// フォント画像分割数X,Y
	static constexpr int M_COLOR_NUM = 4;					// カラー配列数
	static constexpr int M_CH_SELECT_COUNT_MAX = 8;			// 文字選択カウント最大値

	// 各テキスト配列数
	static constexpr int M_SCORE_SYSTEM_TEXT_NUM = 4;
	static constexpr int M_READY_TEXT_NUM = 3;
	static constexpr int M_COPYRIGHT_NUM = 2;
	static constexpr int M_PROMPT_START_TEXT_NUM = 2;
	static constexpr int M_CONG_TEXT_NUM = 2;
	static constexpr int M_BEST_FIVE_TEXT_NUM = 6;
	static constexpr int M_HIDDEN_MESSAGE_TEXT_NUM = 2;

	static constexpr int M_LOGO_X = CHARACTER_SIZE * 4 - 3;	// ロゴX座標
	static constexpr int M_LOGO_Y = CHARACTER_SIZE * 14;	// ロゴY座標
	static constexpr int M_READY_FLAME = 161;				// READYを表示するフレーム数
	static constexpr int M_SCORE_Y = 5;						// スコア、ハイスコア表示Y座標
	static constexpr int M_SCORE_X = 6;						// スコア表示X座標
	static constexpr int M_HIGH_SCORE_X = 16;				// ハイスコア表示X座標
	static constexpr int M_RANKING_SCORE_X = 13;			// ランキング表示スコアX座標
	static constexpr int M_RANKING_NAME_X = 16;				// ランキング表示名前X座標

	// テキストとキャラクタ単位の座標をセットにしたテキスト構造体
	typedef struct {
		char text[UI_TEXT_BUF];
		int x;
		int y;
	}TEXT_STRUCT;

	// スコア系テキスト
	const TEXT_STRUCT m_scoreSystemText[M_SCORE_SYSTEM_TEXT_NUM] = {
		"1UP", 3, 4,
		"HIGH SCORE", 9, 4,
		"0", M_SCORE_X, 5,			// スコア末尾の0
		"0", M_HIGH_SCORE_X, 5		// ハイスコア末尾の0
	};

	const TEXT_STRUCT m_cloneText = {
		"CLONE" , 17, 19
	};

	// デモ版
	const TEXT_STRUCT m_demoText = {
		"DEMO Ver.",16, 20
	};

	// バージョン表記
	const TEXT_STRUCT m_verText = {
		"Ver.1.1",16, 20
	};

	// 権利表記　勝手に書いていいんかな…？
	const TEXT_STRUCT m_copyright[M_COPYRIGHT_NUM] = {
		"{|} 1982 NAMCO LTD.", 5, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 5,
		"ｱｲｳｴｵｶｷ", 11, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 3				// NAMCOロゴ　"ｱｲｳｴｵｶｷ"のASKIIコードに対応させてある
	};

	// 開始を促すテキスト
	const TEXT_STRUCT m_promptStartText[M_PROMPT_START_TEXT_NUM] = {
		"PUSH START BUTTON", 6, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 13,
		"ONE PLAYER ONLY", 7, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 11
	};

	// クレジット　ひとまず01固定で表示
	const TEXT_STRUCT m_credit = {
		"CREDIT 01", TEMP_SCREEN_WIDTH / CHARACTER_SIZE - 12, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 1
	};

	// READYテキスト
	const TEXT_STRUCT m_readyText[M_READY_TEXT_NUM] = {
		"PLAYER ONE", 9, 27,
		"READY!", 11, 29,
		"SOLVALOU LEFT", 8, 31
	};

	// ストック表示用
	const TEXT_STRUCT m_stock = {
		"^", 0, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 1
	};

	// 隠しメッセージ
	const TEXT_STRUCT m_hiddenMessageText[M_HIDDEN_MESSAGE_TEXT_NUM] = {
		"THANK YOU FOR PLAYING!", 1, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 3,
		"program by N.Takafumi", 6, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 2
	};


	// コングラチュレーション
	const TEXT_STRUCT m_congText[M_CONG_TEXT_NUM] = {
		"CONGRATULATIONS!", 6, 13,
		"ENTER YOUR INITIAL", 5, 16
	};

	// ベストファイブ表示
	const TEXT_STRUCT m_bestFiveText[M_BEST_FIVE_TEXT_NUM] = {
		"1st        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12,
		"2nd        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 10,
		"3rd        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 8,
		"4th        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 6,
		"5th        0", 2, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 4,
		"Best Five WARRIORS", 5, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 15,
	};

	// ゲームオーバー
	const TEXT_STRUCT m_gameOverText = {
		"GAME OVER" , 10, TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12
	};

	const int m_uiScreen;				// UIスクリーンハンドル
	const int* m_pPadBuf;				// パッドバフポインタ

	ScoreManager* m_scoreManagerP;		// スコアマネージャポインタ
	Player* m_playerP;					// プレイヤーポインタ

	TEXT_STRUCT m_readyStock;			// READY時のストック数
	TEXT_STRUCT m_scoreText;			// スコア
	TEXT_STRUCT m_highScoreText;		// 画面上部のハイスコア
	TEXT_STRUCT m_rankingNameText[ScoreManager::RANKING_MAX];	// ランキング
	TEXT_STRUCT m_rankingScoreText[ScoreManager::RANKING_MAX];	// ランキング

	TEXT_STRUCT m_tempName;				// ランキング処理用　一時名前テキスト
	TEXT_STRUCT m_inputCh;				// ランキング処理用　現在の文字表示用
	char m_Ch;							// ランキング処理用　現在の文字の内部的な値
	char* m_pTempNameText;				// ランキング処理用　現在入力しているm_tempName.textのポインタ
	bool m_isInputFirst;				// ランキング処理用　文字入力のボタン初回入力フラグ

	int m_rank;							// ランキング処理用　入ったランク
	int m_chSelectCount;				// ランキング処理用　文字入力で次の文字に行くカウント

	int m_readyFlameCount;				// READY表示フレームカウント
	int m_preScore;						// 前フレームのスコア
	bool m_isDrawHiddenMessage;			// 隠しメッセージ描画フラグ

	int m_titleLogo;					// ロゴ画像ハンドル
	int m_fontImage[M_FONT_NUM];		// フォント画像ハンドル配列
	unsigned int m_color[M_COLOR_NUM];	// カラー配列

	int DrawGraphicString(int x, int y, const char* strings, int* phFont, int transflag = TRUE);
	int DrawTextStruct(TEXT_STRUCT textStruct);
	int Filter(TEXT_STRUCT textStruct, int color);
	int IntToString(int num, unsigned int textBuf, char* text);
	int SetReadyStock();
	int SetScoreText();
};