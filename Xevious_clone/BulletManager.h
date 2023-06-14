#pragma once
#include "Zapper.h"
#include "Blaster.h"
#include "Cursor.h"
#include "LandingPoint.h"
#include "SoundManager.h"

class BulletManager
{
public:
	// ザッパーヒット情報構造体
	typedef struct {
		bool enemy[ZAPPER_NUM];		// 敵ヒット情報
		bool baqura[ZAPPER_NUM];	// バキュラヒット情報
	}ZAPPER_HIT;

	BulletManager(SoundManager* soundManagerP);
	~BulletManager();

	void CreateZapper(float playerX, float playerY, char key);
	void CreateBlaster(float playerX, float playerY, char key);

	void Calc(int frameCount);
	void CursorCalc(float playerX, float playerY, int frameCount);
	void LandingPointCalc();
	void BlasterCalc();

	void ZapperDraw();
	void CursorDraw();
	void LandingPointDraw();
	void BlasterDraw();

	float GetZapperX(int zapperId);
	float GetZapperY(int zapperId);
	bool GetZapperOnActive(int zapperId);
	int GetBlasterFrameCount();
	float GetLandingPointX();
	float GetLandingPointY();
	float GetCursorX();
	float GetCursorY();

	void SetOnCursor(bool onCursor);

	void SetZapperOnActive(int zapperId, bool onActive);
	void SetZapperHit(ZAPPER_HIT zapperHit);

	static const int M_BULLET_SIZE = 16;			// 弾のサイズ
	static const int M_CURSOR_SIZE = 16;			// カーソルのサイズ

	static const int M_BLASTER_FRAME = 38;			// ブラスター発射フレーム
private:
	static const int M_BULLET_IMG_DIVSION_NUM = 72;	// 弾画像分割数
	static const int M_BULLET_IMG_DIVSION_X = 12;	// の、X分割数
	static const int M_BULLET_IMG_DIVSION_Y = 6;	// と、Y分割数
	static const int M_RAPID_FRAME = 20;			// 自動連射の発射間隔
	static const int M_BLASTER_WAIT = 2;			// ブラスター発射待機フレーム


	static const int M_CURSOR_IMG_DIVSION_NUM = 6;  // カーソル画像分割数
	static const int M_CURSOR_IMG_DIVSION_X = 6;	// の、X分割数
	static const int M_CURSOR_IMG_DIVSION_Y = 1;	// と、Y分割数

	int m_BulletImage[M_BULLET_IMG_DIVSION_NUM];	// 弾画像
	int m_CursorImage[M_CURSOR_IMG_DIVSION_NUM];	// カーソル画像

	bool m_OnCursor;								// カーソルが重なっているか否か

	int m_ZapperAnimationId;						// 弾アニメーションID
	bool m_isZapperFlip;							// 左右反転フラグ
	int m_FrameCount;								// フレームカウンタ
	int m_BlasterFrameCount;						// ブラスター発射待機フレームカウンタ
	bool m_IsBlasterLaunching;						// ブラスター発射中フラグ

	ZAPPER_HIT m_zapperHit;							// ザッパーヒット情報

	Zapper* m_zapper[ZAPPER_NUM];					// ザッパーポインタ配列
	Cursor* m_cursor;								// カーソルポインタ
	LandingPoint* m_landingPoint;					// 着弾カーソルポインタ
	Blaster* m_blaster;								// ブラスターポインタ
	SoundManager* m_soundManagerP;					// サウンドマネージャポインタ
};

