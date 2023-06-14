#include "UI.h"

UI::UI(const int uiScreen, ScoreManager* scoreManagerP, Player* playerP, const int* padBuf) : m_uiScreen(uiScreen), m_pPadBuf(padBuf) {

	m_color[static_cast<int>(ColorName::RED)] = GetColor(0xFF, 0x00, 0x00);
	m_color[static_cast<int>(ColorName::YELLOW)] = GetColor(0xFF, 0xFF, 0x00);
	m_color[static_cast<int>(ColorName::ORANGE)] = GetColor(0xFF, 0xAE, 0x00);
	m_color[static_cast<int>(ColorName::STOCK)] = GetColor(0xE0, 0xBC, 0x62);

	m_titleLogo = LoadGraph("img/title.png");
	LoadDivGraph("img/Xevious_font.png", M_FONT_NUM, M_FONT_IMAGE_DIVISION_NUM, M_FONT_IMAGE_DIVISION_NUM, CHARACTER_SIZE, CHARACTER_SIZE, m_fontImage);

	m_scoreManagerP = scoreManagerP;	// スコアマネージャポインタ
	m_playerP = playerP;				// プレイヤーポインタ

	memset(&m_tempName, 0x00, sizeof(m_tempName));			// ランキングネームテキスト初期化
	memset(&m_inputCh, 0x00, sizeof(m_inputCh));			// ランキングスコアテキスト初期化
	m_inputCh.text[0] = m_Ch = 32;							// 初期入力文字
	m_tempName.x = M_RANKING_NAME_X;						// 一時名前テキストX座標
	m_inputCh.x = M_RANKING_NAME_X;							// 入力文字表示用X座標
	
	m_rank = -1;						// ランキング処理用　入ったランク
	m_chSelectCount = 0;				// ランキング処理用　文字選択カウント
	m_isInputFirst = true;				// 文字入力ボタン初回フラグ
	m_isDrawHiddenMessage = false;		// 隠しメッセージ出力フラグ

	m_preScore = 0;						// 前回スコア

	memset(m_rankingNameText, 0x00, sizeof(m_rankingNameText));			// ランキングネームテキスト初期化
	memset(m_rankingScoreText, 0x00, sizeof(m_rankingScoreText));		// ランキングスコアテキスト初期化

	for (int i = 0; i < ScoreManager::RANKING_MAX; i++) {								// ランキング関連座標設定
		m_rankingNameText[i].x = M_RANKING_NAME_X;
		m_rankingNameText[i].y = TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12 + (i * 2);
		m_rankingScoreText[i].y = TEMP_SCREEN_HEIGHT / CHARACTER_SIZE - 12 + (i * 2);
	}


	// テキストY座標設定
	m_readyStock.y = m_readyText[2].y;	// READYストック数
	m_scoreText.y = M_SCORE_Y;			// スコア
	m_highScoreText.y = M_SCORE_Y;		// ハイスコア

	Init();								// 初期化処理
}

// 初期化関数
void UI::Init() {
	SetScoreText();						// スコアテキスト
	SetReadyStock();					// READYストック
	m_readyFlameCount = M_READY_FLAME;	// READY表示フレームカウント
	m_isDrawHiddenMessage = false;		// 隠しメッセージ出力フラグ
}

// ネームエントリー初期化
int UI::NameEntryInit() {
	m_rank = m_scoreManagerP->RankingUpdate();					// ランキング更新、入ったランクを取得

	if (m_rank != -1) {											// ランクインしていたら
		SetRankingText();										// ランキングテキストを構成
		memset(m_tempName.text, 0x00, sizeof(m_tempName.text));	// 初期化
		memset(m_inputCh.text, 0x00, sizeof(m_inputCh.text));	// 初期化
		m_tempName.y = m_rankingNameText[m_rank].y;				// 文字入力関連テキストのY座標設定
		m_inputCh.y = m_rankingNameText[m_rank].y;
		m_pTempNameText = m_tempName.text;						// 現在入力先のポインタを設定
		m_isInputFirst = false;									// ボタン入力フラグ初期化
		m_inputCh.text[0] = m_Ch = 32;							// 初期入力文字 
		m_inputCh.x = M_RANKING_NAME_X;							// 初期入力文字表示X座標
		m_chSelectCount = 0;									// 文字入力選択カウント
	}

	return m_rank;
}

// 文字入力が終わってゲームオーバーになった時のみ1を返し、それ以外では0を返します
int UI::Calc(int mode) {
	int tmpScore;

	switch (mode)
	{
	case static_cast<int>(ModeId::TITLE):
		break;

	case static_cast<int>(ModeId::MAIN_GAME):

		if (m_readyFlameCount != 0) {				// READY表示用フレームカウントデクリメント
			m_readyFlameCount--;
		}

		tmpScore = m_scoreManagerP->GetScore();		// スコア取得
		if (m_preScore != tmpScore) {				// 変化があったら
			m_preScore = tmpScore;					// スコア保持
			SetScoreText();							// 文字列化
		}
		break;

	case static_cast<int>(ModeId::NAME_ENTRY):

		if (*m_pPadBuf & PAD_INPUT_LEFT) {
			m_chSelectCount--;
			if (m_chSelectCount < 0) {
				m_chSelectCount = M_CH_SELECT_COUNT_MAX;
				m_Ch--;
			}
		}
		if (*m_pPadBuf & PAD_INPUT_RIGHT) {
			m_chSelectCount++;
			if (m_chSelectCount >= M_CH_SELECT_COUNT_MAX) {
				m_chSelectCount = 0;
				m_Ch++;
			}
		}

		
		if (m_Ch == 33) {	// スペースの後だったら
			m_Ch = 65;		// Aに
		}
		if (m_Ch == 64) {	// Aの前だったら
			m_Ch = 32;		// スペースに
		}

		if (m_Ch == 31) {	// スペースの前だったら
			m_Ch = 90;		// Zに
		}
		if (m_Ch == 91) {	// Zの後だったら
			m_Ch = 32;		// スペースに
		}

		if (*m_pPadBuf & PAD_INPUT_2 + PAD_INPUT_4) {
			if (m_Ch == 32) {
				m_inputCh.text[0] = 46;
			}
			else {
				m_inputCh.text[0] = m_Ch + 32;
			}
		}
		else {
			m_inputCh.text[0] = m_Ch;
		}

		if (*m_pPadBuf & PAD_INPUT_1 + PAD_INPUT_3) {
			if (m_isInputFirst) {
				m_isInputFirst = false;
				*m_pTempNameText = m_inputCh.text[0];		// 文字を代入
				m_pTempNameText++;							// ポインタをインクリメントで1個ズラす
				m_inputCh.x++;								// 入力文字のX座標をインクリメント

				if (strlen(m_tempName.text) >= 10) {
					m_scoreManagerP->SetUserName(m_rank, m_tempName.text);
					return 1;
				}
			}
		}
		else {
			m_isInputFirst = true;
		}


	default:
		break;
	}

	return 0;
}

void UI::Draw(int mode, int frameCount, bool isGameOver) {
	int i = 0;	//カウンタ

	if (mode == static_cast<int>(ModeId::TITLE) || frameCount % 32 < 16) {	// タイトル以外では点滅処理
		DrawTextStruct(m_scoreSystemText[0]);	// 1UP
	}
	for (i = 1; i < M_SCORE_SYSTEM_TEXT_NUM; i++) {
		DrawTextStruct(m_scoreSystemText[i]);	// HIGH SCORE　スコア末尾の0　ハイスコア末尾の0
	}
	DrawTextStruct(m_scoreText);				// スコア
	DrawTextStruct(m_highScoreText);			// ハイスコア

	// フィルター	画面上のハイスコア表示
	Filter(m_scoreSystemText[1], static_cast<int>(ColorName::ORANGE));


	switch (mode)
	{
	case static_cast<int>(ModeId::TITLE):
		DrawGraph(M_LOGO_X, M_LOGO_Y, m_titleLogo, TRUE);	// ロゴ

		DrawTextStruct(m_cloneText);						// CLONE
		DrawTextStruct(m_verText);							// バージョン表記
		//DrawTextStruct(m_demoText);							// デモ版

		for (i = 0; i < M_COPYRIGHT_NUM; i++) {
			DrawTextStruct(m_copyright[i]);					// 権利表記
		}
		// フィルター	namco
		Filter(m_copyright[1], static_cast<int>(ColorName::RED));

		for (i = 0; i < M_PROMPT_START_TEXT_NUM; i++) {
			DrawTextStruct(m_promptStartText[i]);			// 開始を促すテキスト
		}
		DrawTextStruct(m_credit);							// クレジット


		break;

	case static_cast<int>(ModeId::MAIN_GAME):
		// 文字列出力
		if (m_readyFlameCount != 0) {
			for (i = 0; i < M_READY_TEXT_NUM; i++) {
				DrawTextStruct(m_readyText[i]);			// READYテキスト
			}
			DrawTextStruct(m_readyStock);				// READYストック数表示
		}


		// ストック表示
		for (i = 0; i < m_playerP->GetStock(); i++) {
			DrawGraphicString((m_stock.x + i) * CHARACTER_SIZE, m_stock.y * CHARACTER_SIZE, m_stock.text, m_fontImage);
		}

		// ストック表示フィルター　Filter関数だと一文字ずつ掛ける形になってしまって処理が重いのでまとめてかける
		GraphFilterRectBlt(m_uiScreen, m_uiScreen, CHARACTER_SIZE * m_stock.x, CHARACTER_SIZE * m_stock.y,
			TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT,
			CHARACTER_SIZE * m_stock.x, CHARACTER_SIZE * m_stock.y,
			DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER, 128, TRUE, m_color[static_cast<int>(ColorName::STOCK)], 255);

		// 隠しメッセージ
		if (m_isDrawHiddenMessage) {
			for (int i = 0; i < M_HIDDEN_MESSAGE_TEXT_NUM; i++) {
				DrawTextStruct(m_hiddenMessageText[i]);
			}
		}

		break;

	case static_cast<int>(ModeId::NAME_ENTRY):
		if (isGameOver) {
			DrawTextStruct(m_gameOverText);
		}
		else {
			for (i = 0; i < M_CONG_TEXT_NUM; i++) {
				DrawTextStruct(m_congText[i]);											// コングラチュレーションテキスト
				Filter(m_congText[i], static_cast<int>(ColorName::RED));				// フィルター　赤
			}

			for (i = 0; i < ScoreManager::RANKING_MAX; i++) {
				DrawTextStruct(m_rankingScoreText[i]);									// ランキングスコア
				DrawTextStruct(m_rankingNameText[i]);									// ランキングネーム

			}

			for (i = 0; i < M_BEST_FIVE_TEXT_NUM; i++) {
				DrawTextStruct(m_bestFiveText[i]);										// ベストファイブテキスト

				if (i == m_rank) {														// ランクインした部分のみ 
					Filter(m_bestFiveText[i], static_cast<int>(ColorName::YELLOW));		// フィルター　黄色
				}
			}

			DrawTextStruct(m_tempName);
			DrawTextStruct(m_inputCh);

			Filter(m_tempName, static_cast<int>(ColorName::YELLOW));
			Filter(m_inputCh, static_cast<int>(ColorName::YELLOW));

		}
		break;

	default:
		break;
	}

}

/*
* 名前：DrawGraphicString
*
* 機能：画像のフォントデータで文字列を出力します
*
* 引数：
* (i) int x X座標
* (i) int y Y座標
* (i) const char* text 文字列アドレス
* (i)int* pFontImage フォント画像ハンドルポインタ
*
* 戻り値：
* 成功：0
* 失敗：-1
*/
int UI::DrawGraphicString(int x, int y, const char* text, int* pFontImage, int transflag)
{
	int cx = x;

	for (int i = 0; text[i] != NULL; i++)
	{
		if (DrawGraph(cx, y, pFontImage[static_cast<unsigned char>(text[i])], transflag) != 0) {
			return -1;
		}

		cx += CHARACTER_SIZE;
	}

	return 0;
}


/*
* 名前：DrawGraphicTextStruct
*
* 機能：TEXT_STRUCT型のデータを元に、文字列を出力します
*
* 引数：(i) TEXT_STRUCT textStruct テキスト情報
*
* 戻り値：
* 成功：0
* 失敗：-1
*/
int UI::DrawTextStruct(TEXT_STRUCT textStruct) {
	return DrawGraphicString(textStruct.x * CHARACTER_SIZE, textStruct.y * CHARACTER_SIZE, textStruct.text, m_fontImage);
}

/*
* 名前：Filter
*
* 機能：TEXT_STRUCT型のデータを元に、カラーフィルターを掛けます
*
* 引数：
* (i) TEXT_STRUCT textStruct テキスト情報
* (i) int colorId カラーID
*
* 戻り値：
* 成功：0
* 失敗：-1
*
* 注意：colorIdは static_cast<int>(ColorName::**) で指定してください
*/
int UI::Filter(TEXT_STRUCT textStruct, int colorId) {
	return 	GraphFilterRectBlt(m_uiScreen, m_uiScreen, CHARACTER_SIZE * textStruct.x, CHARACTER_SIZE * textStruct.y,
		CHARACTER_SIZE * (textStruct.x + strlen(textStruct.text)), CHARACTER_SIZE * (textStruct.y + 1),
		CHARACTER_SIZE * textStruct.x, CHARACTER_SIZE * textStruct.y,
		DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_GREATER, 128, TRUE, m_color[colorId], 255);
}

/*
* 名前：IntToString
*
* 機能：第一引数の値を文字列にして第三引数のアドレスに代入します
*
* 引数：
* (i) int number 文字列に変換する値
* (i) unsigned int textBuf 代入先のバッファ
* (o) char* text 文字列代入先のアドレス
*
* 戻り値：
* 成功：0
* 失敗：-1
*/
int UI::IntToString(int number, unsigned int textBuf, char* text) {
	std::string tmp = std::to_string(number);
	if (strcpy_s(text, textBuf, tmp.c_str()) == 0) {
		return 0;
	}
	return -1;
}


/*
* 名前：SetReadyStock
*
* 機能：m_readyStock.textとm_readyStock.xを設定します
*
* 戻り値：
* 成功：0
* 失敗：-1
*/
int UI::SetReadyStock() {
	int ret;
	ret = IntToString(m_playerP->GetStock(), UI_TEXT_BUF, m_readyStock.text);
	m_readyStock.x = m_readyText[2].x - 1 - strlen(m_readyStock.text);

	return ret;
}

/*
* 名前：SetScoreText
*
* 機能：m_scoreTextとm_highScoreTextの文字列とX座標を設定します
*
* 戻り値：
* 成功：0
* 失敗：-1
*/
int UI::SetScoreText() {
	if (IntToString(m_scoreManagerP->GetScore(), UI_TEXT_BUF, m_scoreText.text) == -1) {		// 現在スコア
		return -1;
	}
	m_scoreText.x = M_SCORE_X - strlen(m_scoreText.text);

	if (IntToString(m_scoreManagerP->GetTopScore(), UI_TEXT_BUF, m_highScoreText.text) == -1) {	// ハイスコア
		return -1;
	}
	m_highScoreText.x = M_HIGH_SCORE_X - strlen(m_highScoreText.text);

	return 0;
}

/*
* 名前：SetRankingText
* 
* 機能：ランキングデータを元に、ランキングテキストを構成します
* 
* 引数：なし
* 
* 戻り値：
* 成功：0
* 失敗：-1
*/

int UI::SetRankingText() {
	
	ScoreManager::RANKING tempRanking = m_scoreManagerP->GetRanking();	// ランキングデータ読み込み

	for (int i = 0; i < ScoreManager::RANKING_MAX; i++) {
		strcpy_s(m_rankingNameText[i].text, ScoreManager::USER_NAME_BUF, tempRanking.userName[i]);	// 名前データをコピー
		IntToString(tempRanking.highScore[i], ScoreManager::USER_NAME_BUF, m_rankingScoreText[i].text);	// スコアをテキストに変換
		m_rankingScoreText[i].x = M_RANKING_SCORE_X - strlen(m_rankingScoreText[i].text);				// X座標を設定
	}

	return 0;
}

// 隠しメッセージ表示フラグ
void UI::SetIsDrawHiddenMessage(bool isDrawHiddenMessage) {
	m_isDrawHiddenMessage = isDrawHiddenMessage;
}