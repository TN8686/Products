#include "Enemy.h"

// ダミー用コンストラクタ
Enemy::Enemy() {
	m_CharcterId = static_cast<int>(EnemyId::DUMMY);
	m_Status = static_cast<int>(StatusId::NON_EXISTENT);		// 状態　0:非存在　1:画面外待機　2:動作中　3:破壊中
	m_X = 0;						// X座標
	m_Y = 0;						// Y座標
	m_SpeedX = 0;					// X移動速度
	m_SpeedY = 0;					// Y移動速度
	m_AccelerationX = 0;			// X加速度
	m_AccelerationY = 0;			// Y加速度
	m_Width = 0;					// 幅
	m_Height = 0;					// 高さ
	m_Score = 1;					// スコア
	m_IsShot = false;				// ショットフラグ　trueだと弾を撃つ
	m_isCollision = false;			// 食らい判定フラグ
	m_ShotInterval = 1;				// 弾を複数撃つ場合の射撃間隔　実際の処理では8倍して扱う
	m_Type = 0;						// 同型で複数タイプがある敵用
	m_BrokenType = 0;				// 破壊アニメーションタイプ
	m_imageHandleP= NULL;			// 画像ハンドルポインタ
	m_scoreManagerP = NULL;			// スコアマネージャポインタ
	m_pParentObject = NULL;			// 親オブジェクトポインタ
	m_IsFlip = true;				//反転フラグ
	m_BrokenImageAnimation = 0;		//破壊アニメーション加算
	m_Angle = 0;					//角度
	m_FCount = 0;					//フレームカウント
}

// ダミー以外用コンストラクタ
Enemy::Enemy(int charcterId, float x, float y,int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP) {
	m_CharcterId = charcterId;
	m_Status = static_cast<int>(StatusId::ACTIVE);		// 状態
	m_X = x;						// X座標
	m_Y = y;						// Y座標
	m_SpeedX = 0;					// X移動速度
	m_SpeedY = 0;					// Y移動速度
	m_AccelerationX = 0;			// X加速度
	m_AccelerationY = 0;			// Y加速度
	m_Width = LITTLE_SIZE;			// 幅
	m_Height = LITTLE_SIZE;			// 高さ
	m_Score = 1;					// スコア
	m_IsShot = false;				// ショットフラグ　trueだと弾を撃つ
	m_isCollision = true;			// 食らい判定フラグ
	m_ShotInterval = 1;				// 弾を複数撃つ場合の射撃間隔　実際の処理では8倍して扱う
	m_Type = type;					// 同型で複数タイプがある敵用
	m_BrokenType = brokenType;		// 破壊アニメーションタイプ
	m_imageHandleP = imageHandleP;	// 画像ハンドル配列ポインタ
	m_scoreManagerP = scoreManagerP;// スコアマネージャポインタ
	m_pParentObject = NULL;			// 親オブジェクトポインタ
	m_IsFlip = true;				//反転フラグ
	m_BrokenImageAnimation = 0;		//破壊アニメーション加算
	m_Angle = 0;					//角度
	m_FCount = 0;					//フレームカウント
}

void Enemy::m_Shot() {
	m_IsShot = true;
}

// Get関数
int Enemy::GetCharcterId() {	// キャラクタID
	return m_CharcterId;
}
int Enemy::GetStatus() {		// 状態
	return m_Status;
}
float Enemy::GetX() {			// X座標
	return m_X;
}
float Enemy::GetY() {			// Y座標
	return m_Y;
}
int Enemy::GetWidth() {			// Y座標
	return m_Width;
}
int Enemy::GetHeight() {		// Y座標
	return m_Height;
}
bool Enemy::GetIsShot() {		// ショットフラグ
	return m_IsShot;
}
bool Enemy::GetIsCollision() {	// 食らい判定
	return m_isCollision;
}
int Enemy::GetScore() {			// スコア
	return m_Score;
}

// Set関数
void Enemy::SetStatus(int status) {					// 状態セッター
	m_Status = status;
}

void Enemy::SetIsCollision(bool isCollision) {		// 食らい判定フラグセッター
	m_isCollision = isCollision;
}

void Enemy::SetPParentObject(Enemy* pParentObject){	// 親オブジェクトポインタセッター
	m_pParentObject = pParentObject;
}

void Enemy::SetIsShotFalse() {						// ショットフラグをfalseにする
	m_IsShot = false;
}

// (x1, y1)座標から見た(x2, y2)座標の、左を0度として反時計回りの32分割のラジアン値を返します
float Enemy::Angle(float x1, float y1, float x2, float y2) {
	int quarter = 0;	// 四等分でどっち方向か
	float angle = (float)atan2((double)y2 - (double)y1, (double)x2 - (double)x1);	// 厳密な角度

	// 判定回数を安定させるためにまず座標から四等分でどっちかを判定する
	if (x2 <= x1) {		// x2が右側
		if (y2 <= y1) {	// y2が上側
			quarter = 2;
		}
		else {			// y2が下側
			quarter = 1;
		}
	}else {				// x2が左側
		if (y2 <= y1) {	// y2が上側
			quarter = 3;
		}
		else {			// y2が下側
			quarter = 0;
		}
	}

	// 360度基準でのラジアン値に変換
	if (quarter >= 2) {
		angle = PI * 2 + angle;
	}

	/*
	角度チェックループ
	32等分した角度を、64分の1の角度分ズラして判定する事で、大体右なら右、みたいな判定にしている
	*/
	for (int i = 0; i <= 8; i++) {
		if (angle < (RAD_32 * (i + 1)) + (RAD_4 * quarter) - (RAD_32 / 2)) {
			float test = (RAD_32 * i) + (RAD_4 * quarter);
			return (RAD_32 * i) + (RAD_4 * quarter);
		}
	}

	return -1;	// エラー　式がどっかしら間違ってる
}

// 角度と基準速度を元にX、Y移動速度を設定する関数
void Enemy::SetSpeedFromAngle(float angle, float speed) {
	m_SpeedX = speed * (float)cos(angle);
	m_SpeedY = speed * (float)sin(angle);
}

// スコア加算関数　自身のスコアを加算する
void Enemy::AddScore() {
	if (m_scoreManagerP != NULL) {
		m_scoreManagerP->ScoreCalc(m_Score);
	}
}


/*
* 名前：DamageFunction
* 
* 機能：基本の被弾処理を行います　通常の敵はこの関数を呼ぶだけでOKです
* 
* 引数；(i) bool isHit ヒット情報
* 
* 戻り値：無し
* 
* 注意：通常の敵の場合はCalcの中で必ず一番最初にこの関数を呼び、その後自身のStatusを見るSwitch文を置いてください
*/
void Enemy::DamageFunction(bool isHit) {
	if (isHit) {				// ヒット判定
		AddScore();				// スコア加算
		m_isCollision = false;	// 判定を無くす
		//m_Status = static_cast<int>(StatusId::BROKEN);		// 状態をBROKENに
		m_Status++;												// インクリメントで済ませる
	}
}

// 計算系　Enemyクラスではダミーなのでなにもしない
// スパリオ計算用
void Enemy::Calc() {}

// 空中敵用計算
void Enemy::Calc(float playerX, float playerY, float groundMove, bool isHit) {}

// 地上敵用計算
void Enemy::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {}

// 描画　ダミーなのでなにもしない
void Enemy::Draw(int colorID) {}

// 破壊計算
void Enemy::BrokenCalc(float groundMove) {
	switch (m_BrokenType)					// ここでタイプで分岐して生成
	{
	case 0:		
		AerialBrokenCalc();					//空中敵計算
		break;
	case 1:
		GroundBrokenCalc(groundMove);		//地上物描画　クレーターあり
		break;
	case 2:
		GroundBrokenCalc2(groundMove);		//地上物2描画　クレーターなし
		break;
	default:
		break;
	}
}

// 破壊描画
void Enemy::BrokenDraw() {
	switch (m_BrokenType)					// ここでタイプで分岐して生成
	{
	case 0:
		AerialBrokenDraw();					//空中敵描画
		break;
	case 1:
		GroundBrokenDraw();					// 地上物描画　クレーターあり
		break;
	case 2:
		GroundBrokenDraw2();				// 地上物２描画　クレーターなし
		break;
	default:
		break;
	}
}

//空中敵破壊計算
void Enemy::AerialBrokenCalc() {
	m_IsFlip = true;
	switch (m_FCount % 4)			// ここでタイプで分岐して生成 フレームの余り算
	{
	case 0:
		m_IsFlip = false;
		m_Angle = 0;				//反転なし
		break;

	case 1:
	    m_Angle = 0;				//上下反転
		break;

	case 2:
		m_Angle = (double)PI;		//左右反転
		break;

	case 3:
		m_IsFlip = false;
		m_Angle = (double)PI;		//上下左右反転
		break;

	default:
		break;
	}

	if (m_FCount % 4 == 0 && m_FCount != 0) { //はじめを除く4フレーム毎　
		m_BrokenImageAnimation++;
	}
	if (m_BrokenImageAnimation >= 5) { //20フレーム後に非存在
		m_Status = static_cast<int>(StatusId::NON_EXISTENT);
	}
	m_FCount++;									//フレーム加算

	m_Y += m_SpeedY;						//Y移動加算
	m_X += m_SpeedX;						//X移動加算
}



//地上敵破壊計算
void Enemy::GroundBrokenCalc(float groundMove) {
	m_Y += groundMove;

	if (m_FCount % 8 == 0 && m_FCount != 0) { //はじめを除く8フレーム毎　
		m_BrokenImageAnimation++;
	}
	if (m_FCount >= 55) {  //55フレーム後に破壊後
		m_Status = static_cast<int>(StatusId::DESTROYED);
	}
	m_FCount++;
}

//地上敵破壊計算その2
void Enemy::GroundBrokenCalc2(float groundMove) {
	m_Y += groundMove;

	if (m_FCount % 4 == 0 && m_FCount != 0) { //はじめを除く4フレーム毎　
		m_BrokenImageAnimation++;
	}
	if (m_BrokenImageAnimation >= 7) {  //２８フレーム後に破壊後
		m_Status = static_cast<int>(StatusId::NON_EXISTENT);
	}
	m_FCount++;
}

//空中敵破壊描画
void Enemy::AerialBrokenDraw() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[0 + m_BrokenImageAnimation], true, m_IsFlip);
}

//地上敵破壊描画
void Enemy::GroundBrokenDraw() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[7 + m_BrokenImageAnimation], true, m_IsFlip);
}

//地上敵破壊描画その2
void Enemy::GroundBrokenDraw2() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[7 + m_BrokenImageAnimation], true, m_IsFlip);
}