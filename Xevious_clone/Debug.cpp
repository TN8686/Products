#include "Debug.h"

Debug::Debug(bool isDebugMode) {
	this->m_IsDebugMode = isDebugMode;	// デバッグモードフラグ
	this->m_IsStop = false;				// 計算処理停止フラグ
	this->m_IsStopKeyFirst = true;		// 停止キー初回入力フラグ
	this->m_IsAdvanceKeyFirst = true;	// コマ送りキー初回入力フラグ
	this->m_IsFrameAdvance = false;		// コマ送りフラグ
	SetDebugFont();
}

// デバッグ用のフォントサイズ設定
void Debug::SetDebugFont() {
	if (this->m_IsDebugMode) {
		SetFontSize(14);
		SetFontThickness(0);
	}
}

// 値を表示
void Debug::DrawPlayerValue(Player* player, int score, AreaManager* areaManager, int level) {
	if (this->m_IsDebugMode) {
		char textbuf[256];
		sprintf(textbuf, "X:%.1f\nY:%.1f\nStock:%d\nScore:%d\nArea:%d\nLoadArea:%d\nAreaLine:%d\nGroundEnemyAreaLine:%d\nAreaY:%.1f\nLevel:%d", player->GetX(), player->GetY(), player->GetStock(),
			score, areaManager->GetAria(), areaManager->GetLoadAria(), areaManager->GetLine(), areaManager->GetGroundEnemyLine(), areaManager->GetY(), level);
		DrawString(WINDOW_WIDTH + 2, TEMP_SCREEN_WIDTH, textbuf, GetColor(255, 255, 255));
	}
}

/******************************************************************
関数名：DrawDebugWindow

機能：デバッグモード用の領域に情報を表示

引数：
	int型	temp_screen	(i) 最新の仮画面のハンドル
	Player型	player	(i)	プレイヤーオブジェクト

返却値：なし

注意事項：なし
*********************************************************************/
void Debug::DrawDebugWindow(int temp_screen, Player* playerP, int score, AreaManager* areaManagerP, int level){
	if (this->m_IsDebugMode) {
		// デバッグ用描画領域に仮画面をまるごと描画
		DrawGraph(WINDOW_WIDTH, 0, temp_screen, TRUE);
		// 表示画面の表示外領域の一部が見えちゃうので目隠し
		DrawBox(WINDOW_WIDTH, TEMP_SCREEN_WIDTH, WINDOW_WIDTH + TEMP_SCREEN_WIDTH + TEMP_SCREEN_WIDTH, WINDOW_HEIGHT, GetColor(0, 0, 0), TRUE);
		// デバッグ用にプレイヤーデータの一部を出力
		DrawPlayerValue(playerP, score, areaManagerP, level);
	}
}

/******************************************************************
関数名：CalcStop

機能：
	スペースキーが入力される度、計算停止フラグをスイッチし、動作中と停止中を切り替えます
	計算停止中にピリオドキーが押された場合、1フレームだけ動作中にします

引数：	char型	key	(i)	キー入力情報

返却値：
	動作中：true
	停止中：false

注意事項：この関数自体はフラグ情報を返すのみなので、戻り値使ってif文で判定してください
*********************************************************************/
bool Debug::CalcStop(char key[], int padBuf) {
	if (this->m_IsDebugMode) {
		// パッドから停止できるバージョン
		if (!(padBuf & PAD_INPUT_8 || key[KEY_INPUT_SPACE]) && !this->m_IsStopKeyFirst) {			// スペースキーが入力されておらず、かつ停止初回入力フラグが立っていない
			this->m_IsStopKeyFirst = true;								// 停止初回入力フラグをtrueに
		}
		if ((padBuf & PAD_INPUT_8 || key[KEY_INPUT_SPACE]) && this->m_IsStopKeyFirst) {			// スペースキーが入力されていて、かつ停止初回入力である
			this->m_IsStop = !this->m_IsStop;							// 計算停止フラグを反転
			this->m_IsStopKeyFirst = false;								// 停止初回入力フラグをfalseに
		}

		/*// パッドから停止できないバージョン
		if (!(key[KEY_INPUT_SPACE]) && !this->m_IsStopKeyFirst) {			// スペースキーが入力されておらず、かつ停止初回入力フラグが立っていない
			this->m_IsStopKeyFirst = true;								// 停止初回入力フラグをtrueに
		}
		if ((key[KEY_INPUT_SPACE]) && this->m_IsStopKeyFirst) {			// スペースキーが入力されていて、かつ停止初回入力である
			this->m_IsStop = !this->m_IsStop;							// 計算停止フラグを反転
			this->m_IsStopKeyFirst = false;								// 停止初回入力フラグをfalseに
		}
		*/// ここまで

		if (!(padBuf & PAD_INPUT_7 || key[KEY_INPUT_PERIOD]) && !this->m_IsAdvanceKeyFirst) {		// ピリオドキーが入力されておらず、かつコマ送り初回入力フラグが立っていない
			this->m_IsAdvanceKeyFirst = true;							// コマ送り初回入力フラグをtrueに
		}
		if ((padBuf & PAD_INPUT_7 || key[KEY_INPUT_PERIOD]) && this->m_IsAdvanceKeyFirst) {		// ピリオドキーが入力されていて、かつコマ送り初回入力である
			this->m_IsFrameAdvance = true;
			this->m_IsAdvanceKeyFirst = false;							// コマ送り初回入力フラグをfalseに
		}

		if (!this->m_IsStop || this->m_IsFrameAdvance) {				// 停止中でない、またはコマ送りフラグがtrue
			this->m_IsFrameAdvance = false;
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}

// デバッグモードゲッター
bool Debug::GetIsDebugMode() {
	return m_IsDebugMode;
}