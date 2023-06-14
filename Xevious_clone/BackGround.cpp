#include "BackGround.h"

BackGround::BackGround() {
	m_speed = M_SPEED_INIT;													// 背景スクロール速度
	m_startImage = LoadGraph("img/map_ready.png");							// 開始時の森の画像
	m_mapImage = LoadGraph("img/map.png");									// 全体マップ画像
	GetGraphSize(m_startImage, &m_startImageHeight, &m_startImageWidth);	// 画像サイズ
	GetGraphSize(m_mapImage, &m_mapImageHeight, &m_mapImageWidth);			// 画像サイズ
	Init();																	// 背景構造体配列初期化

}


/******************************************************************
関数名：Init

機能：背景構造体配列の情報を初期化します

引数：なし

返却値：なし
*********************************************************************/
void BackGround::Init() {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {											// 背景構造体配列数分
		m_backGround[i].image = m_startImage;												// 割り当てる画像を開始時の森に
		m_backGround[i].x = static_cast<float>(TEMP_OFFSCREEN_WIDTH);						// X座標
		m_backGround[i].y = static_cast<float>(TEMP_SCREEN_HEIGHT - (i + 1) * M_HEIGHT);	// Y座標　下から配置していく
		m_backGround[i].imageX = 0;															// 画像のX座標
		m_backGround[i].imageY = m_startImageHeight - (i + 1) * M_HEIGHT;					// 画像のY座標
	}
}

// 背景のスクロール速度を返します
float BackGround::GetSpeed() {
	return m_speed;
}

/******************************************************************
関数名：Calc

機能：背景全体の計算を行います

引数：
	int型	area	(i)	エリアID
	int型	line	(i)	エリアライン
	bool型	isStart	(i)	開始時か否か

返却値：なし
*********************************************************************/
void BackGround::Calc(int area, int line, bool isStart) {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {											// 背景構造体配列数分
		m_backGround[i].y += m_speed;														// スクロール速度分加算

		if (m_backGround[i].y >= TEMP_SCREEN_HEIGHT) {										// 画面外に出たら
			m_backGround[i].y -= TEMP_SCREEN_HEIGHT;										// Y座標を一画面分上に

			if (isStart) {																	// 開始時だったら
				m_backGround[i].imageY = m_startImageHeight - (line + 1) * CHARACTER_SIZE;	// 画像の参照Y座標を現在のラインの位置に応じた物に更新
			}
			else {																			// 開始時でなかったら
				if (m_backGround[i].image != m_mapImage) {									// 参照画像ハンドルが全体マップ画像でなかったら
					m_backGround[i].image = m_mapImage;										// 参照画像ハンドルを全体マップ画像に
				}
				m_backGround[i].imageY = m_mapImageHeight - (line + 1) * CHARACTER_SIZE;	// 画像の参照Y座標を現在のラインの位置に応じた物に更新

				if (m_backGround[i].imageX != M_IMAGE_X[area]) {							// 現在の参照X座標が現在のエリアの参照X座標と異なっていたら
					m_backGround[i].imageX = M_IMAGE_X[area];								// エリアデータに応じたX座標を代入
				}
			}
		}
	}

}

void BackGround::Draw() {
	for (int i = 0; i < M_BACK_GROUND_NUM; i++) {
		DrawRectGraph(static_cast<int>(m_backGround[i].y), static_cast<int>(m_backGround[i].x),	// X座標、Y座標
			m_backGround[i].imageY, m_backGround[i].imageX, M_HEIGHT, M_WIDTH,					// 参照画像のX座標、Y座標、描画する幅、描画する高さ
			m_backGround[i].image, false, false);												// 参照画像ハンドル、
	}
}