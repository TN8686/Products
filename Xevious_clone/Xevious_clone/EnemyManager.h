#pragma once

#include "Xevious_clone.h"
#include "EnemyManagerConst.h"
#include "ScoreManager.h"
#include "AreaManager.h"
#include "SoundManager.h"
#include "UI.h"

#include "Enemy.h"
#include "Spario.h"

#include "Toroid.h"
#include "Torkan.h"
#include "Zoshi.h"
#include "Jara.h"
#include "Giddospario.h"

#include "Barra.h"
#include "Zolbak.h"
#include "Grobda.h"
#include "Logram.h"
#include "BozaLogram.h"
#include "Crater.h"
#include "Derota.h"
#include "Sol.h"
#include "SPFlag.h"
#include "HiddenMessage.h"

using namespace EnemyManagerConst;

class EnemyManager
{
public:

	// 地上敵テーブル構造体
	typedef struct{
		int characterID;
		int type;
		int brokenType;
		float offsetX;
		float offsetY;
	} GROUND_TABLE;

	// 空中敵テーブル構造体
	typedef struct {
		int characterID;
		int type;
		int num;
	} AERIAL_TABLE;

	// 地上敵配置情報構造体
	typedef struct {
		int tableID;
		int areaX;
		int areaY;
	} ENEMY_MAP;

	// 敵ヒット情報構造体
	typedef struct {
		bool aerial[AERIAL_NUM];
		bool ground[GROUND_NUM];
	}ENEMY_HIT;

	EnemyManager(ScoreManager* pScoreManager, AreaManager* pAreaManager, SoundManager* pSoundManager, UI* pUI);	// コンストラクタ宣言
	~EnemyManager(); // デストラクタ宣言

	void Init();	// 初期化関連
	void Restart();

	// 生成
	float RandomX(float playerX, bool isDistance);
	int GroundGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, Enemy* pParentObject);		// 地上敵生成
	void AerialGenerater(float playerX, float playerY);	// 空中敵生成
	void BulletGenerater(float x, float y, float playerX, float playerY);	// 弾生成

	int AreaDataCheck(float playerX);

	// 計算
	void Calc(int frameCount);	// 全体に関わる物の計算
	void GroundCalc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove);	// 地上物計算
	void AerialCalc(float playerX, float playerY, float groundMove);	// 空中敵計算
	void SparioCalc();	// 弾計算

	// 描画
	void GroundDraw();	// 地上物描画
	void AerialDraw();	// 空中敵描画
	void SparioDraw();	// 弾描画

	// セッター
	void SetIsAerialGenerate();	// 空中敵出現フラグ
	void SetEnemyHit(ENEMY_HIT enemyHit);

	// ゲッター
	float GetAerialX(int aerialEnemiesId);
	float GetAerialY(int aerialEnemiesId);
	int GetAerialStatus(int aerialEnemiesId);
	bool GetAerialIsCollision(int aerialEnemiesId);

	int GetGroundCharcterId(int groundEnemiesId);	// キャラクタID
	float GetGroundX(int groundEnemiesId);
	float GetGroundY(int groundEnemiesId);
	int GetGroundStatus(int groundEnemiesId);
	bool GetGroundIsCollision(int groundEnemiesId);

	float GetBulletX(int bulletId);
	float GetBulletY(int bulletId);
	int GetBulletStatus(int bulletId);

	int GetAerialLevel();

private:
	int GroundAreaDataSet();				// 地上敵情報を配置
	int AerialPatternSet(int patternID);	// 空中敵情報配置
	int AerialGenerateStart();				// レベル固定値加算＋空中敵情報配置＋空中敵生成開始
	int LevelUpByScore();					// スコア依存の敵レベル加算
	int LoadLineData();						// ライン処理情報ロード
	int LoadAreaData();						// エリア情報ロード

	// ボザログラム生成処理
	int BozaLogramGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, int i);

	// 画面外チェッカー
	bool OffScreenChecker(float x, float y);
	bool OffScreenChecker2(float x, float y);


	static constexpr int LEVEL_ADD_VALUE[LEVEL_NUM] = { 0, 2, 6, 16 };		// レベル固定加算値
	static constexpr int LEVEL_SUBTRAC_VALUE[LEVEL_NUM] = { 24, 16, 8, 0 };	// レベル固定減算値

	// テーブルを定数として宣言	ひとまずエリア1に必要な分だけ
	const GROUND_TABLE GROUND_ENEMY_TABLE[ENEMY_TABLE_NUM] = {
		static_cast<int>(EnemyId::DUMMY), 0, 0, 0, 0,												// 0 最初はダミーを配置しておく
		static_cast<int>(EnemyId::SPFLAG), 0, 0, 0, 0,												// 1
		static_cast<int>(EnemyId::BARRA), 0, static_cast<int>(BrokenTypeId::GROUND_A), 0, 0,		// 2
		static_cast<int>(EnemyId::LOGRAM), 0, static_cast<int>(BrokenTypeId::GROUND_A), 0, 0,		// 3
		static_cast<int>(EnemyId::ZOLBAK), 0, static_cast<int>(BrokenTypeId::GROUND_A), 0, 0,		// 4
		static_cast<int>(EnemyId::SOL), 0, static_cast<int>(BrokenTypeId::GROUND_A), 0, 0,			// 5
		static_cast<int>(EnemyId::GROBDA), 0, static_cast<int>(BrokenTypeId::GROUND_A), 2, 0,		// 6
		static_cast<int>(EnemyId::BOZA_LOGRAM), 0, static_cast<int>(BrokenTypeId::GROUND_A), 4, 0,	// 7
	};

	const AERIAL_TABLE AERIAL_ENEMY_TABLE[ENEMY_TABLE_NUM][AERIAL_TABLE_PATTERN_NUM] = {
		{static_cast<int>(EnemyId::TOROID), 0, 3},
		{static_cast<int>(EnemyId::TOROID), 0, 3},
		{static_cast<int>(EnemyId::TOROID), 0, 3},

		{static_cast<int>(EnemyId::TOROID), 0, 4},
		{static_cast<int>(EnemyId::TOROID), 0, 4},
		{static_cast<int>(EnemyId::TOROID), 0, 4},

		{static_cast<int>(EnemyId::TOROID), 0, 5},
		{static_cast<int>(EnemyId::TOROID), 0, 5},
		{static_cast<int>(EnemyId::TOROID), 0, 5},

		{static_cast<int>(EnemyId::TORKAN), 0, 2},
		{static_cast<int>(EnemyId::TORKAN), 0, 2},
		{static_cast<int>(EnemyId::TORKAN), 0, 2},

		{static_cast<int>(EnemyId::TOROID), 0, 4},
		{static_cast<int>(EnemyId::TOROID), 0, 4},

		{static_cast<int>(EnemyId::TOROID), 0, 3, static_cast<int>(EnemyId::TOROID), 1, 1},
		{static_cast<int>(EnemyId::TOROID), 0, 3, static_cast<int>(EnemyId::TOROID), 1, 1},

		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 2},
		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 2},

		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 3},
		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 3},

		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 4},
		{static_cast<int>(EnemyId::TOROID), 0, 2, static_cast<int>(EnemyId::TOROID), 1, 4},

		{static_cast<int>(EnemyId::TORKAN), 0, 3},
		{static_cast<int>(EnemyId::TORKAN), 0, 3},

		{static_cast<int>(EnemyId::TORKAN), 0, 4},
		{static_cast<int>(EnemyId::TORKAN), 0, 4},

		{static_cast<int>(EnemyId::TORKAN), 0, 5},
		{static_cast<int>(EnemyId::TORKAN), 0, 5},

		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 3},
		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 3},

		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 4},
		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 4},

		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 5},
		{static_cast<int>(EnemyId::GIDDO_SPARIO), 0, 5},

		{static_cast<int>(EnemyId::ZOSHI), 0, 3},
		{static_cast<int>(EnemyId::ZOSHI), 0, 3},

		{static_cast<int>(EnemyId::ZOSHI), 0, 4},
		{static_cast<int>(EnemyId::ZOSHI), 0, 4},

		{static_cast<int>(EnemyId::ZOSHI), 0, 5},
		{static_cast<int>(EnemyId::ZOSHI), 0, 5},

		{static_cast<int>(EnemyId::JARA), 0, 3},
		{static_cast<int>(EnemyId::JARA), 0, 3},

		{static_cast<int>(EnemyId::JARA), 0, 4},
		{static_cast<int>(EnemyId::JARA), 0, 4},

		{static_cast<int>(EnemyId::JARA), 0, 5},
		{static_cast<int>(EnemyId::JARA), 0, 5},	// ひとまず敵が作ってある45まで

	};

	// ライン処理定数
	static constexpr int APP_STOP					 = 1;
	static constexpr int APP_START_LEVEL			 = 2;
	static constexpr int APP_START_PATTERN			 = 3;
	static constexpr int LEVEL_SCORE_UPDATE			 = 4;
	static constexpr int APP_ALONE_ENEMY			 = 5;
	static constexpr int SHOT_INTERVAL_MAX_CHANGE	 = 6;

	// 変数群
	bool m_IsAerialGenerate;				// 空中敵出現フラグ
	int m_AerialLevel;						// 現在の空中敵レベル
	int m_AppAerialPattern[AERIAL_NUM];		// 出現敵配列
	int m_AppAerialTypePattern[AERIAL_NUM];	// 出現敵タイプ配列

	ENEMY_HIT m_enemyHit;					// ヒット情報
	IMAGE_HANDLE_STRUCT m_imageHandle;		// 画像ハンドル配列

	int m_enemyDataNum = 0;					// ロードの際のエネミーデータの数
	ENEMY_MAP m_enemyData[AREA_DATA_GROUND_MAX]; // 敵配置情報セット用構造体 本来はこの構造の配列をバイナリデータにしてロードする 配列数は仮で64
	int m_GroundAreaData[AreaManager::LINE_MAX][AreaManager::AREA_CHARACTER_WIDTH];	// 実際に使用される敵配置データ　ここの値を読み取り、そのIDの敵テーブルの情報で敵を配置する

	int lineProcessData[AreaManager::LINE_MAX][LINE_PROCESS_NUM][LINE_PROCESS_NUM];	// ライン処理命令データ

	int m_FrameCount;		// 経過フレーム
	bool m_IsIncrement;		// 敵カラーの値を増やすか減らすか
	int m_EnemyColor;		// 現在の敵カラー
	int m_CraterColor;		// 現在のクレーターのカラー
	int m_BulletColor;		// 現在の弾カラー
	int m_GiddosparioColor;	// 現在のギドスパリオのカラー

	int m_shotIntervalMax;	// 地上敵の射撃間隔最大値
	int m_difficulty;		// 難易度
	int m_sortie;			// ソルバルウ出撃数

	ScoreManager* m_pScoreManager;		// スコアマネージャポインタ
	AreaManager* m_pAreaManager;		// エリアマネージャポインタ
	SoundManager* m_pSoundManager;		// サウンドマネージャポインタ
	UI* m_pUI;							// UIポインタ

	// 乱数関連
	std::default_random_engine m_Seed;					// シード
	std::uniform_int_distribution<int> m_RandEnemyX;	// 乱数生成オブジェクト

	// オブジェクトのポインタ宣言
	Enemy* m_GroundEnemies[GROUND_NUM];		// 地上物
	Enemy* m_Bacuras[BACURA_NUM];			// バキュラ
	Enemy* m_AerialEnemies[AERIAL_NUM];		// 空中敵
	Enemy* m_spario[SPARIO_NUM];			// 通常弾（スパリオ）

};