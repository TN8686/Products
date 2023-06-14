#include "BulletManager.h"

// コンストラクタ
BulletManager::BulletManager(SoundManager* soundManagerP) {
	LoadDivGraph("img/Xevious_OBJ_bullet1.png", M_BULLET_IMG_DIVSION_NUM,
		M_BULLET_IMG_DIVSION_X, M_BULLET_IMG_DIVSION_Y,
		M_BULLET_SIZE, M_BULLET_SIZE, this->m_BulletImage, FALSE);	// 弾画像
	
	this->m_ZapperAnimationId = 0;
	this->m_isZapperFlip = false;
	this->m_FrameCount = 0;
	this->m_BlasterFrameCount = this->M_BLASTER_FRAME + 1;
	this->m_IsBlasterLaunching = false;
	memset(&this->m_zapperHit, false, sizeof(ZAPPER_HIT));

	// カーソル画像
	LoadDivGraph("img/Xevious_OBJ_solvalou.png", M_CURSOR_IMG_DIVSION_NUM, M_CURSOR_IMG_DIVSION_X, M_CURSOR_IMG_DIVSION_Y,
		M_CURSOR_SIZE, M_CURSOR_SIZE, m_CursorImage, FALSE);	// 画像	全体を6等分　横6等分　縦1等分

	this->m_soundManagerP = soundManagerP;						// サウンドマネージャポインタ

	this->m_cursor = new Cursor(m_CursorImage);					// カーソル実体宣言
	this->m_landingPoint = new LandingPoint(m_CursorImage);		// 着弾カーソルの実体宣言
	this->m_blaster = new Blaster(this->m_BulletImage);			// ブラスターの実体宣言

	for (int i = 0; i < ZAPPER_NUM; i++) {					// ザッパーの実体宣言
		this->m_zapper[i] = new Zapper(this->m_BulletImage);
	}

}

// デストラクタ
BulletManager::~BulletManager() {
	delete m_cursor;
	delete m_landingPoint;
	delete m_blaster;

	for (int i = 0; i < ZAPPER_NUM; i++) {
		delete this->m_zapper[i];
	}
}

// ザッパー生成
void BulletManager::CreateZapper(float playerX, float playerY, char key) {
	if (key) {												// キーが押されていたら
		if (this->m_FrameCount % M_RAPID_FRAME == 0) {		// フレームカウントが発射間隔フレーム経過する毎に
			for (int i = 0; i < ZAPPER_NUM; i++) {		// 空きを探すループ
				if (!this->m_zapper[i]->GetOnActive()) {		// 空きがあったら
					this->m_zapper[i]->SetX(playerX);			// X座標設定
					this->m_zapper[i]->SetY(playerY + 6);		// Y座標設定
					this->m_zapper[i]->SetOnActive(true);		// 状態フラグをtrueに
					this->m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::ZapperShoot));	// SE再生
					break;
				}
			}
		}
		this->m_FrameCount++;								// フレームカウントをインクリメント
	}
	else if(this->m_FrameCount != 0){						// キーが押されていない、かつフレームカウントが0でない時
		this->m_FrameCount = 0;								// フレームカウントを0に
	}
}

// ブラスター生成
void BulletManager::CreateBlaster(float playerX, float playerY, char key){

	// ブラスター発射中でない
	if (this->m_BlasterFrameCount > this->M_BLASTER_FRAME) {
		if (key) {	// 発射キーが入力されている
			this->m_BlasterFrameCount = 0;
			this->m_soundManagerP->SePlay(static_cast<int>(SeId::BlasterShoot));	// SE再生
		}
	}
	else {
		if (this->m_BlasterFrameCount == this->M_BLASTER_WAIT) {
			this->m_landingPoint->Create(this->m_cursor->GetX(), this->m_cursor->GetY());	// 着弾カーソル生成
			this->m_blaster->Create(playerX, playerY);
			this->m_IsBlasterLaunching = true;
		}
	}
}

// 計算
void BulletManager::Calc(int frameCount) {

	if (frameCount % 2 == 0) {						// 2フレーム毎に
		this->m_ZapperAnimationId++;				// アニメーションIDをインクリメント

		if (this->m_ZapperAnimationId >= 4) {		// アニメーションIDが4だったら
			this->m_ZapperAnimationId = 0;			// アニメーションIDを0に
		}
	}

	this->m_isZapperFlip = !this->m_isZapperFlip;	// 左右反転

	for (int i = 0; i < ZAPPER_NUM; i++) {							// 計算ループ
		if (this->m_zapperHit.enemy[i]) {
			this->m_zapper[i]->SetOnActive(false);	// 状態フラグをfalseに
			this->m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::EnemyStriken_Zapper));
		}


		this->m_zapper[i]->SetAnimationId(this->m_ZapperAnimationId);	// アニメーションIDをセット
		this->m_zapper[i]->SetIsTurn(this->m_isZapperFlip);				// 左右反転フラグをセット
		this->m_zapper[i]->Calc();										// 計算

		if (this->m_zapper[i]->GetY() < 0) {			// 画面外に出たら
			this->m_zapper[i]->SetOnActive(false);	// 状態フラグをfalseに
		}

	}

	if (this->m_BlasterFrameCount <= this->M_BLASTER_FRAME) {
		this->m_BlasterFrameCount++;
	}
	if (this->m_BlasterFrameCount == this->M_BLASTER_FRAME) {
		this->m_IsBlasterLaunching = false;
	}
}

/******************************************************************
関数名：CursorCalc

機能：自機カーソルの計算を行います

引数：
float型	playerX		(i)
float型	playerY		(i)
int型	frameCount	(i)

返却値：なし

注意事項：

*********************************************************************/
void BulletManager::CursorCalc(float playerX, float playerY, int frameCount) {
	this->m_cursor->Calc(playerX, playerY, this->m_OnCursor, this->m_IsBlasterLaunching, frameCount);
}

// 着弾カーソルの計算を行います
void BulletManager::LandingPointCalc() {
	this->m_landingPoint->Calc();
}

// ブラスターの計算を行います
void BulletManager::BlasterCalc() {
	this->m_blaster->Calc();
}


// ザッパーの描画を行います
void BulletManager::ZapperDraw() {
	for (int i = 0; i < ZAPPER_NUM; i++) {
		if (this->m_zapper[i]->GetOnActive()) {			// アクティブな物のみ
			this->m_zapper[i]->Draw();

		}
	}
}

// 自機カーソルの描画を行います
void BulletManager::CursorDraw() {
	this->m_cursor->Draw();
}

// 着弾カーソルの描画を行います
void BulletManager::LandingPointDraw() {
	this->m_landingPoint->Draw();
}

// ブラスターの描画を行います
void BulletManager::BlasterDraw() {
	this->m_blaster->Draw();
}

/******************************************************************
関数名：GetZapperX

機能：引数で指定したIDのザッパーのX座標を返します

引数：	int型	zapperId	(i)	ザッパーのID

返却値：
	成功：X座標
	失敗：0
注意事項：
	１：zapperIdにはBulletManager::M_ZAPPER_NUM未満（3未満）の値を使用してください
	２：弾がアクティブでない、または不正なIDの場合0を返します
*********************************************************************/
float BulletManager::GetZapperX(int zapperId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (zapperId < ZAPPER_NUM && this->m_zapper[zapperId]->GetOnActive()) {
			return this->m_zapper[zapperId]->GetX();
	}
	return 0;
}

/******************************************************************
関数名：GetZapperY

機能：引数で指定したIDのザッパーのY座標を返します

引数：	int型	zapperId	(i)	ザッパーのID

返却値：
	成功：Y座標
	失敗：0
注意事項：
	１：zapperIdにはBulletManager::M_ZAPPER_NUM未満の値を使用してください
	２：弾がアクティブでない、または不正なIDの場合0を返します
*********************************************************************/
float BulletManager::GetZapperY(int zapperId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (zapperId < ZAPPER_NUM && this->m_zapper[zapperId]->GetOnActive()) {
			return this->m_zapper[zapperId]->GetY();
	}
	return 0;
}

/******************************************************************
関数名：GetZapperY

機能：引数で指定したIDのザッパーのアクティブ状態を返します

引数：	int型	zapperId	(i)	ザッパーのID

返却値：	bool型
	成功：アクティブ状態
	失敗：0
注意事項：
	１：zapperIdにはBulletManager::M_ZAPPER_NUM未満の値を使用してください
	２：不正なIDの場合falseを返します
*********************************************************************/
bool BulletManager::GetZapperOnActive(int zapperId) {
	if (zapperId < ZAPPER_NUM) {
		return this->m_zapper[zapperId]->GetOnActive();
	}
	return false;
}

int BulletManager::GetBlasterFrameCount() {
	return this->m_BlasterFrameCount;
}

float BulletManager::GetLandingPointX() {
	return this->m_landingPoint->GetX();
}

float BulletManager::GetLandingPointY() {
	return this->m_landingPoint->GetY();
}

float BulletManager::GetCursorX() {
	return this->m_cursor->GetX();
}

float BulletManager::GetCursorY() {
	return this->m_cursor->GetY();
}


// オンカーソルセッター
void BulletManager::SetOnCursor(bool onCursor) {
	this->m_OnCursor = onCursor;
}

/******************************************************************
関数名：SetZapperOnActive

機能：引数で指定したIDのザッパーのアクティブ状態を設定します

引数：	int型	zapperId	(i)	ザッパーのID
		bool型	onActive	(i)	アクティブ状態

返却値：なし

注意事項：
	１：zapperIdにはBulletManager::M_ZAPPER_NUM未満の値を使用してください
	２：不正なIDの場合、なにもしません
*********************************************************************/
void BulletManager::SetZapperOnActive(int zapperId, bool onActive) {
	if (zapperId < ZAPPER_NUM) {
			this->m_zapper[zapperId]->SetOnActive(onActive);
	}
}

// ザッパーヒット情報受け取り
void BulletManager::SetZapperHit(ZAPPER_HIT zapperHit) {
	this->m_zapperHit = zapperHit;
}