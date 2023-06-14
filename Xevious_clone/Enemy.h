#pragma once

#include <math.h>
#include "Xevious_clone.h"
#include "EnemyManagerConst.h"
#include "ScoreManager.h"

using namespace EnemyManagerConst;

// 敵画像ハンドルを一纏めで扱う構造体
typedef struct {
	int little[ENEMY_COLOR_NUM][LITTLE_IMG_DIVSION_NUM];		// 敵画像ハンドル配列
	int big[ENEMY_COLOR_NUM][BIG_IMG_DIVSION_NUM];				// 大型敵画像ハンドル配列
	int broken[BROKEN_IMG_DIVSION_NUM];							// 破壊画像ハンドル配列
	int spario[SPARIO_IMG_DIVSION_NUM];							// 弾画像ハンドル配列
	int zakato[ZAKATO_IMG_DIVSION_NUM];							// ザカート出現画像ハンドル配列
	int andorGenesisBody[ANDOR_GENESIS_BODY_IMG_DIVSION_NUM];	// アンドアジェネシス本体画像ハンドル配列
	int andorGenesisParts[ANDOR_GENESIS_PARTS_IMG_DIVSION_NUM];	// アンドアジェネシスパーツ画像ハンドル配列
} IMAGE_HANDLE_STRUCT;

// 状態定数	#defineではなくenumで扱うようにしました
enum class StatusId : int {
	NON_EXISTENT,
	ACTIVE,
	BROKEN,
	DESTROYED
};

// 破壊タイプID
enum class BrokenTypeId : int {
	AERIAL,
	GROUND_A,
	GROUND_B
};

// 敵ID　上から順に0から1ずつ増えている　中身を意識する必要は無し
enum class EnemyId : int {
	DUMMY,
	TOROID,
	JARA,
	TORKAN,
	KAPI,
	TERRAZI,
	ZOSHI,
	GIDDO_SPARIO,
	ZAKATO,
	BRAG_ZAKATO,
	GARU_ZAKATO,
	BACURA,
	ZEPNITE,	// 右側　正しい綴り不明
	CASNITE,	// 左側　正しい綴り不明
	SPARIO,
	BRAG_SPARIO,
	BARRA,
	ZOLBAK,
	LOGRAM,
	BOZA_LOGRAM,
	DEROTA,
	DOMOGRAM,
	GROBDA,
	GARU_BARRA,
	GARU_DEROTA,
	SOL,
	CRATER,
	SPFLAG,
	MESSAGE,
	ANDOR_GENESIS_BODY,
	ANDOR_GENESIS_CORE,
	ANDOR_GENESIS_PARTS,
	HIDDEN_MESSAGE
};

class Enemy
{
protected:
	int m_CharcterId;						// 敵の種類ID
	int m_Status;							// 状態　0:非存在　1:画面外待機　2:動作中　3:破壊アニメーション中　4:破壊終了
	float m_X;								// X座標
	float m_Y;								// Y座標
	float m_SpeedX;							// X移動速度
	float m_SpeedY;							// Y移動速度
	float m_AccelerationX;					// X加速度
	float m_AccelerationY;					// Y加速度
	int m_Width;							// 幅
	int m_Height;							// 高さ
	int m_Score;							// スコア
	int m_ShotInterval;						// 弾を複数撃つ場合の射撃間隔　実際の処理では8倍して扱う
	bool m_IsShot;							// ショットフラグ
	bool m_isCollision;						// 食らい判定フラグ
	int m_Type;								// 同じ敵で複数行動パターンがある敵用
	int m_BrokenType;						// 破壊アニメーションタイプ　0:通常
	IMAGE_HANDLE_STRUCT* m_imageHandleP;	// 画像ハンドルポインタ
	ScoreManager* m_scoreManagerP;			// スコアマネージャポインタ
	Enemy* m_pParentObject;					// 親オブジェクトポインタ

	double m_Angle;							// 角度計算
	bool m_IsFlip;							// 反転フラグ
	int m_BrokenImageAnimation;				// 破壊アニメーション加算
	int m_FCount;							// フレームカウント

	static constexpr float M_SPEED_TABLE[4] = { 1.5, 2, 3, 4 };	// 速度テーブル
	static constexpr int NOT_SHOT_LINE = TEMP_SCREEN_HEIGHT - 88;	// 射撃不可ライン

	static constexpr float RAD_32 = 0.19634954f;	// 32分割のラジアン値
	static constexpr float RAD_4 = 1.57079632f;		// 4分割のラジアン値

	// ショット関数　呼ぶとそのフレームだけ弾を撃つ	（実際はフラグをtrueにしてるだけ　falseにするのはマネージャ側が常に管理）
	void m_Shot();

public:
	Enemy();
	Enemy(int charcterId, float x, float y, int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);	// コンストラクタ宣言

	// Get関数
	int GetCharcterId();	// キャラクタID
	int GetStatus();		// 状態
	float GetX();			// X座標
	float GetY();			// Y座標
	int GetWidth();			// 幅
	int GetHeight();		// 高さ
	bool GetIsShot();		// ショットフラグ
	bool GetIsCollision();	// 当たり判定フラグ
	int GetScore();			// スコア

	// Set関数
	void SetStatus(int status);
	void SetIsCollision(bool isCollision);
	void SetPParentObject(Enemy* pParentObject);
	void SetIsShotFalse();

	float Angle(float x1, float y1, float x2, float y2);	// (x1, y1)座標から見た(x2, y2)座標の、右を0度として反時計回りの32分割のラジアン値を返します
	void SetSpeedFromAngle(float angle, float speed);		// 角度を元にX、Y移動速度を設定する関数
	void AddScore();	// スコア加算関数
	void DamageFunction(bool isHit);	// 基本の被弾処理関数

	virtual void Calc();	// スパリオ計算用
	virtual void Calc(float playerX, float playerY, float groundMove, bool isHit);	// 空中敵計算用
	virtual void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit);	// 地上物計算用
	virtual void Draw(int colorID);	// 描画

	void BrokenCalc(float groundMove);	// 破壊処理
	void BrokenDraw();	// 破壊処理

	void GroundBrokenCalc(float groundMove); //地上敵破壊アニメーション計算
	void GroundBrokenDraw(); //地上敵破壊アニメーション
	void GroundBrokenCalc2(float groundMove); //地上敵破壊アニメーション計算その2
	void GroundBrokenDraw2(); //地上敵破壊アニメーション2
	void AerialBrokenCalc(); //破壊アニメーション計算
	void AerialBrokenDraw(); //破壊アニメーション


};