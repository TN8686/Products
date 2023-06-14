#include "Player.h"
#include "EnemyManager.h"

// コンストラクタ
Player::Player(SoundManager* pSoundManager) {
	m_width = PLAYER_SIZE;											// 幅
	m_height = PLAYER_SIZE;											// 高さ
	m_speed = 1.0f;													// Y方向、斜め方向への移動速度
	m_speed_x = 1.5f;												// X方向のみへの移動速度
	LoadDivGraph("img/Xevious_OBJ_solvalou.png", PLAYER_IMG_DIVSION_NUM, PLAYER_IMG_DIVSION_NUM, 1, m_width, m_height, m_image, FALSE);	// 画像	全体を6等分　横6等分　縦1等分
	LoadDivGraph("img/Xevious_OBJ_solvalouBroken.png", BROKEN_IMG_DIVSION_NUM, BROKEN_IMG_DIVSION_NUM, 1, BROKEN_IMG_SIZE, BROKEN_IMG_SIZE, m_brokenImage, FALSE);	// 画像	全体を6等分　横6等分　縦1等分

	m_isInvincible = false;											// 無敵フラグ
	m_isDraw = true;												// 描画フラグ
	m_isFlip = false;												// 破壊描画時の反転フラグ
	m_pSoundManager = pSoundManager;								// サウンドマネージャポインタ

	Init();
}

void Player::Init() {
	m_x = static_cast<float>(TEMP_SCREEN_WIDTH / 2.0f);							// 初期X座標
	m_y = static_cast<float>(TEMP_SCREEN_HEIGHT - m_height - CHARACTER_SIZE);	// 初期Y座標
	m_onActive = true;															// アクティブ状態
	m_isHit = false;															// ヒット情報
	m_brokenAnimID = 0;															// 破壊アニメーションID
	m_frameCount = 0;															// 破壊アニメーション用フレームカウント
	m_stock = STOCK_INIT;														// 初期残機
	}

void Player::Restart(){
	m_x = static_cast<float>(TEMP_SCREEN_WIDTH / 2.0f);							// 初期X座標
	m_y = static_cast<float>(TEMP_SCREEN_HEIGHT - m_height - CHARACTER_SIZE);	// 初期Y座標
	m_onActive = true;															// アクティブ状態
	m_isHit = false;															// ヒット情報
	m_brokenAnimID = 0;															// 破壊アニメーションID
	m_frameCount = 0;															// 破壊アニメーション用フレームカウント
	m_stock--;																	// ストックを減らす
}

// Get関数系
float Player::GetX() {
	return m_x;
}
float Player::GetY() {
	return m_y;
}
int Player::GetStock() {
	return m_stock;
}
bool Player::GetOnActive() {
	return m_onActive;
}

// Set関数系
void Player::SetIsHit(bool isHit) {
	m_isHit = isHit;
}

// エクステンド
void Player::Extend() {
	m_stock++;
}

// 計算関数
void Player::Calc(char keyBuf[], int padBuf, int frameCount) {
	
	if (m_isHit) {			// ヒット処理
		if (!m_isInvincible) {
			m_onActive = false;	// アクティブ状態をfalseに
			if (!m_pSoundManager->GameStartBgmCheck()) {
				m_pSoundManager->BgmStop();
			}
			m_pSoundManager->NoisePlay(static_cast<int>(NoiseSeId::Miss));
		}
		else {
			m_isDraw = !m_isDraw;
		}
	}
	else {
		m_isDraw = true;
	}

	if (m_onActive) {

		// 上下移動
		if (padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) {	// 上
			m_y -= m_speed;
		}
		if (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) {	// 下
			m_y += m_speed;
		}

		// 左右移動　左右移動時と斜め移動時で横の移動速度が違う
		if (padBuf & PAD_INPUT_LEFT || keyBuf[KEY_INPUT_LEFT]) {	// 左
			if (!(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && !(padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) ||	// 上も下も押されていない時
				(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN])) {	// または上も下も押されている時、横移動判定
				m_x += m_speed_x;
			}
			else {												// どちらでもない時は斜め移動判定
				m_x += m_speed;
			}
		}
		if (padBuf & PAD_INPUT_RIGHT || keyBuf[KEY_INPUT_RIGHT]) {	// 右　以下左と同じ
			if (!(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && !(padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN]) ||	// 上も下も押されていない時
				(padBuf & PAD_INPUT_UP || keyBuf[KEY_INPUT_UP]) && (padBuf & PAD_INPUT_DOWN || keyBuf[KEY_INPUT_DOWN])) {	// または上も下も押されている時、横移動判定
				m_x -= m_speed_x;
			}
			else {
				m_x -= m_speed;
			}

		}

		// 移動限界
		if (m_y < Y_MAX) {	//上 多分この位置で合ってるけど一応仮の値
			m_y = Y_MAX;
		}
		if (m_y > (float)TEMP_SCREEN_HEIGHT - (float)m_height) {	// 下
			m_y = (float)TEMP_SCREEN_HEIGHT - (float)m_height;
		}
		if (m_x < TEMP_OFFSCREEN_WIDTH) {									// 右
			m_x = TEMP_OFFSCREEN_WIDTH;
		}
		if (m_x > (float)TEMP_SCREEN_WIDTH - (float)m_width) {		// 左
			m_x = (float)TEMP_SCREEN_WIDTH - (float)m_width;
		}
	}
	else {
		BrokenCalc(frameCount);
	}

	m_isHit = false;	// ヒット情報初期化
}

// 描画関数
void Player::Draw() {
	if (m_onActive) {
		if (m_isDraw) {
			DrawGraph((int)m_y, (int)m_x, m_image[0], TRUE);
		}
	}
	else {
		BrokenDraw();
	}
}

void Player::BrokenCalc(int frameCount) {
	if (m_frameCount != 0 && m_frameCount % 8 == 0) {
		m_brokenAnimID++;
	}

	if (frameCount % 4 == 0) {
		m_isFlip = !m_isFlip;
	}

	if (m_brokenAnimID == 7) {
		m_x = 0;
		m_y = 0;
	}

	m_frameCount++;
}

void Player::BrokenDraw() {
	if (m_isFlip) {
		DrawGraph(static_cast<int>(m_y) - 8, static_cast<int>(m_x) - 8, m_brokenImage[m_brokenAnimID], true);
	}
	else {
		DrawTurnGraph(static_cast<int>(m_y) - 8, static_cast<int>(m_x) - 8, m_brokenImage[m_brokenAnimID], true);
	}
}
