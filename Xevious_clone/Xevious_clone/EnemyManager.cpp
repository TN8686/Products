#include "EnemyManager.h"

// コンストラクタ
EnemyManager::EnemyManager(ScoreManager* pScoreManagerP, AreaManager* pAreaManagerP, SoundManager* pSoundManager, UI* pUI) {
	int i;
	char imageName[IMAGE_NAME_BUF];					// ファイル名変数
	char bigImageName[IMAGE_NAME_BUF];				// ファイル名変数
	memset(imageName, 0x00, sizeof(imageName));		// 初期化
	memset(bigImageName, 0x00, sizeof(imageName));	// 初期化

	memset(m_AppAerialPattern, 0, sizeof(m_AppAerialPattern));	// 空中敵出現パターン初期化
	memset(&m_enemyHit, false, sizeof(ENEMY_HIT));				// ヒット情報初期化
	m_pScoreManager = pScoreManagerP;							// スコアマネージャポインタ
	m_pAreaManager = pAreaManagerP;								// エリアマネージャポインタ
	m_pSoundManager = pSoundManager;							// サウンドマネージャポインタ
	m_pUI = pUI;												// UIポインタ

	memset(m_enemyData, 0x00, sizeof(m_enemyData));			// エリア配置情報初期化
	memset(lineProcessData, 0x00, sizeof(lineProcessData));	// ライン処理命令データ初期化

	// 画像読み込み
	for (i = 0; i < ENEMY_COLOR_NUM; i++) {	// カラーパターンが別画像の物を画像配列に割り当てるループ
		switch (i)
		{
		case 0:
			strcpy_s(imageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemy_col0.png");
			strcpy_s(bigImageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemyBig_col0.png");
			break;
		case 1:
			strcpy_s(imageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemy_col1.png");
			strcpy_s(bigImageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemyBig_col1.png");
			break;
		case 2:
			strcpy_s(imageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemy_col2.png");
			strcpy_s(bigImageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemyBig_col2.png");
			break;
		case 3:
			strcpy_s(imageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemy_col3.png");
			strcpy_s(bigImageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemyBig_col3.png");
			break;
		case 4:
			strcpy_s(imageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemy_col4.png");
			strcpy_s(bigImageName, IMAGE_NAME_BUF, "img/Xevious_OBJ_enemyBig_col4.png");
			break;
		default:
			break;
		}
		LoadDivGraph(imageName, LITTLE_IMG_DIVSION_NUM,
			LITTLE_IMG_DIVSION_X, LITTLE_IMG_DIVSION_Y,
			LITTLE_SIZE, LITTLE_SIZE, m_imageHandle.little[i], FALSE);	// 小型敵画像	全体を80等分　横8等分　縦10等分

		LoadDivGraph(bigImageName, BIG_IMG_DIVSION_NUM,
			BIG_IMG_DIVSION_X, BIG_IMG_DIVSION_Y,
			BIG_SIZE, BIG_SIZE, m_imageHandle.big[i], FALSE);			// 大型敵画像
	}
	LoadDivGraph("img/Xevious_OBJ_enemyBroken.png", BROKEN_IMG_DIVSION_NUM,
		BROKEN_IMG_DIVSION_X, BROKEN_IMG_DIVSION_Y,
		BIG_SIZE, BIG_SIZE, m_imageHandle.broken, FALSE);	// 破壊画像	全体を14等分　横7等分　縦2等分

	LoadDivGraph("img/Xevious_OBJ_bullet1.png", SPARIO_IMG_DIVSION_NUM,
		SPARIO_IMG_DIVSION_X, SPARIO_IMG_DIVSION_Y,
		LITTLE_SIZE, LITTLE_SIZE, m_imageHandle.spario, FALSE);	// 弾画像

	LoadDivGraph("img/Xevious_OBJ_bullet2.png", ZAKATO_IMG_DIVSION_NUM,
		ZAKATO_IMG_DIVSION_X, ZAKATO_IMG_DIVSION_Y,
		BIG_SIZE, BIG_SIZE, m_imageHandle.zakato, FALSE);	// ザカート系出現画像	全体を30等分　横5等分　縦6等分

	m_FrameCount = 0;			// 経過フレーム
	m_IsIncrement = false;		// 敵点滅用フラグ変数
	m_EnemyColor = 0;			// 現在の敵カラー
	m_CraterColor = 0;			// 現在のクレーターカラー
	m_BulletColor = 0;			// 現在の弾カラー
	m_GiddosparioColor = 0;		// 現在のギドスパリオのカラー
	m_difficulty = 1;			// 難易度　後で引数から設定に変更？　1:標準

	// X座標用ランダム生成器
	std::random_device rd;
	std::default_random_engine tmpRandSeed(rd());
	m_Seed = tmpRandSeed;
	std::uniform_int_distribution<int> tmpRand(CARACTER_X_MIN, CARACTER_X_MAX);
	m_RandEnemyX = tmpRand;

	// ダミーオブジェクト生成
	for (i = 0; i < GROUND_NUM; i++) {
		m_GroundEnemies[i] = new Enemy();
	}
	for (i = 0; i < AERIAL_NUM; i++) {
		m_AerialEnemies[i] = new Enemy();
	}
	for (i = 0; i < SPARIO_NUM; i++) {
		m_spario[i] = new Enemy();
	}

	Init();			// リスタート時と同じ処理
}

// デストラクタ
EnemyManager::~EnemyManager() {
	int i;

	// オブジェクトのメモリを解放
	for (i = 0; i < GROUND_NUM; i++) {
		delete m_GroundEnemies[i];
	}
	for (i = 0; i < AERIAL_NUM; i++) {
		delete m_AerialEnemies[i];
	}
	for (i = 0; i < SPARIO_NUM; i++) {
		delete m_spario[i];
	}
}

void EnemyManager::Init() {
	int i;
	
	LoadAreaData();											// エリア配置情報をロード
	GroundAreaDataSet();									// データを元に地上物を割り当て
	LoadLineData();											// ライン処理情報をロード

	// オブジェクトを全て非存在、食らい判定無しに
	for (i = 0; i < GROUND_NUM; i++) {
		m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		m_GroundEnemies[i]->SetIsCollision(false);
	}
	for (i = 0; i < AERIAL_NUM; i++) {
		m_AerialEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		m_AerialEnemies[i]->SetIsCollision(false);
	}
	for (i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
	}

	// 隠しメッセージオブジェクト生成
	GroundGenerater(TEMP_OFFSCREEN_WIDTH - 4, 0, static_cast<int>(EnemyId::HIDDEN_MESSAGE), 0, 0, 0, NULL);

	m_shotIntervalMax = 255;	// 地上敵の射撃間隔最大値
	m_sortie = 0;				// 出撃回数

	m_IsAerialGenerate = false;							// 敵生成フラグ
	m_AerialLevel = 0;									// 空中敵レベル
	m_sortie++;											// 出撃回数を加算
}

void EnemyManager::Restart() {
	int i;

	LoadAreaData();											// エリア配置情報をロード
	GroundAreaDataSet();									// データを元に地上物を割り当て
	LoadLineData();											// ライン処理情報をロード

	// オブジェクトを全て非存在、食らい判定無しに
	for (i = 0; i < GROUND_NUM; i++) {
		m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		m_GroundEnemies[i]->SetIsCollision(false);
	}
	for (i = 0; i < AERIAL_NUM; i++) {
		m_AerialEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		m_AerialEnemies[i]->SetIsCollision(false);
	}
	for (i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
	}

	// 隠しメッセージオブジェクト生成
	if (m_pAreaManager->GetAria() == 0) {
		GroundGenerater(TEMP_OFFSCREEN_WIDTH - 4, 0, static_cast<int>(EnemyId::HIDDEN_MESSAGE), 0, 0, 0, NULL);
	}

	m_shotIntervalMax = 255;	// 地上敵の射撃間隔最大値
	m_sortie = 0;				// 出撃回数

	m_IsAerialGenerate = false;							// 敵生成停止
	m_AerialLevel -= LEVEL_SUBTRAC_VALUE[m_difficulty];	// 難易度に応じてレベルダウン
	if (m_AerialLevel < 0) {							// 0未満だったら0に補正
		m_AerialLevel = 0;
	}
	m_sortie++;											// 出撃回数を加算
}

// Set関数
void EnemyManager::SetIsAerialGenerate() {	// 空中敵出現フラグON関数
	m_IsAerialGenerate = true;
}

void EnemyManager::SetEnemyHit(ENEMY_HIT enemyHit) {
	m_enemyHit = enemyHit;
}

// エリア配置データをロードします　が、現状は仮にエリア1のデータをベタ書きしているだけ
int EnemyManager::LoadAreaData() {
	memset(m_enemyData, 0x00, sizeof(m_enemyData));			// 敵配置情報初期化

	m_enemyData[0] = { 2,16,44 };							// 仮のエリア1地上敵情報　本来はロードする
	m_enemyData[1] = { 4,14,44 };

	m_enemyData[2] = { 2,16,76 };
	m_enemyData[3] = { 3,14,76 };

	m_enemyData[4] = { 2,11,87 };
	m_enemyData[5] = { 3,11,89 };

	m_enemyData[6] = { 4, 8,97 };
	m_enemyData[7] = { 3,10,99 };

	m_enemyData[8] = { 3,14,136 };
	m_enemyData[9] = { 4,14,138 };

	m_enemyData[10] = { 5,4,153 };

	m_enemyData[11] = { 6,11,161 };
	m_enemyData[12] = { 6,11,164 };
	m_enemyData[13] = { 6,11,167 };

	m_enemyData[14] = { 1,0,190 };

	m_enemyData[15] = { 3,14,198 };
	m_enemyData[16] = { 2,12,198 };


	m_enemyData[17] = { 4,5,219 };
	m_enemyData[18] = { 3,7,221 };
	m_enemyData[19] = { 4,9,223 };
	m_enemyData[20] = { 3,11,225 };
	m_enemyData[21] = { 4,13,227 };

	m_enemyData[22] = { 3,24,223 };

	m_enemyData[23] = { 5,26,235 };

	m_enemyData[24] = { 7,11,237 };

	return 0;
}

// ライン処理データをロードします　が、現状は仮にエリア1のデータをベタ書きしているだけ
int EnemyManager::LoadLineData() {
	memset(lineProcessData, 0x00, sizeof(lineProcessData));	// ライン処理命令データ初期化

	lineProcessData[0][0][0] = SHOT_INTERVAL_MAX_CHANGE;	// 仮のエリア1ライン処理データ　本来はロードする
	lineProcessData[0][0][1] = 255;
	lineProcessData[5][0][0] = APP_START_LEVEL;
	lineProcessData[23][0][0] = APP_START_LEVEL;
	lineProcessData[43][0][0] = APP_START_LEVEL;
	lineProcessData[63][0][0] = APP_STOP;
	lineProcessData[75][0][0] = APP_START_LEVEL;
	lineProcessData[99][0][0] = APP_STOP;
	lineProcessData[103][0][0] = LEVEL_SCORE_UPDATE;
	lineProcessData[103][0][1] = 1;
	lineProcessData[111][0][0] = APP_START_LEVEL;
	lineProcessData[127][0][0] = APP_START_LEVEL;
	lineProcessData[143][0][0] = APP_START_LEVEL;
	lineProcessData[163][0][0] = LEVEL_SCORE_UPDATE;
	lineProcessData[163][0][1] = 1;
	lineProcessData[167][0][0] = APP_START_LEVEL;
	lineProcessData[175][0][0] = APP_START_LEVEL;
	lineProcessData[187][0][0] = APP_START_LEVEL;
	lineProcessData[199][0][0] = APP_START_PATTERN;
	lineProcessData[199][0][1] = 4;
	lineProcessData[200][0][0] = SHOT_INTERVAL_MAX_CHANGE;
	lineProcessData[200][0][1] = 24;
	lineProcessData[235][0][0] = APP_STOP;

	return 0;
}

/*
* 名前：GroundAreaDataSet
*
* 機能：m_GroundAreaDataを初期化し、m_enemyDataを元に地上物をテーブルに割り当てます
*
* 引数：なし
*
* 戻り値：配置した地上物の数
*/
int EnemyManager::GroundAreaDataSet() {
	memset(m_GroundAreaData, 0x00, sizeof(m_GroundAreaData));	// 地上敵エリア情報初期化
	int i;

	for (i = 0; i < AREA_DATA_GROUND_MAX; i++) {
		if (m_enemyData[i].tableID == 0) {	// IDが0のデータが出てきたらそこで終了
			break;
		}
		m_GroundAreaData[m_enemyData[i].areaY][m_enemyData[i].areaX] = m_enemyData[i].tableID;	// 割り当て
	}

	return i;
}

/*
* 名前：AerialPatternSet
* 
* 機能：引数で指定された値のIDの敵パターンをセットします
* 
* 引数：敵パターンID
* 
* 戻り値：生成される敵の数
*/
int EnemyManager::AerialPatternSet(int level) {

	memset(m_AppAerialPattern, 0x00, sizeof(m_AppAerialPattern));			// 初期化
	memset(m_AppAerialTypePattern, 0x00, sizeof(m_AppAerialTypePattern));	// 初期化

	int patternID = 0;

	for (int j = 0; j < AERIAL_TABLE_PATTERN_NUM; j++) {								// テーブル一つにつき二種類の敵
		for (int i = 0; i < AERIAL_ENEMY_TABLE[level][j].num; i++) {					// その敵の配置数分ループ 
			m_AppAerialPattern[patternID] = AERIAL_ENEMY_TABLE[level][j].characterID;	// IDをセット
			m_AppAerialTypePattern[patternID] = AERIAL_ENEMY_TABLE[level][j].type;		// タイプをセット
			patternID++;																// 配置場所をインクリメント
		}
	} 

	return patternID;
}

/*
* 名前：AerialPatternSet
* 
* 機能：空中敵のレベルを難易度に応じて上げ、生成パターンをセットし、空中敵出現フラグをtrueにします
* 
* 引数：なし
* 
* 戻り値：生成される敵の数
*/
int EnemyManager::AerialGenerateStart() {
	int ret;
	m_AerialLevel += LEVEL_ADD_VALUE[m_difficulty];	// 難易度依存でレベルを固定値加算

	if (m_AerialLevel >= 46) {	// 本来は128以上の時に64減算する処理　現状は敵不足のため適当に無難な値
		m_AerialLevel -= 32;
	}

	ret = AerialPatternSet(m_AerialLevel);	// パターンをセット
	
	m_IsAerialGenerate = true;				// 空中敵出現開始
	return ret;								// 生成される敵の数
}

/*
* 名前：LevelUpByScore
* 
* 機能：スコアに応じて空中敵出現レベルを加算します
* 
* 引数：なし
* 
* 戻り値：レベルに加算した値
*/

int EnemyManager::LevelUpByScore() {
	int n = (m_pScoreManager->GetScore() / 100);					// 現在スコアを100で割る
	n = n + 0x6 * (n / 10) + 0x60 * (n / 100) + 0x600 * (n / 1000);	// その値をBCDに変換

	if (m_sortie != 0) {	// 念のため0除算回避
		n /= m_sortie;		// 出撃数で割る
	}

	if (n > 16) {			// 最大16
		n = 16;
	}

	m_AerialLevel += n;		// レベルに加算

	return n;
}

// 全体に関わる物の計算
void EnemyManager::Calc(int frameCount) {

	// 弾カラー選択
	if (frameCount % 2 == 0) {	// 2フレーム毎
		m_BulletColor++;
		m_GiddosparioColor++;

		if (m_BulletColor == 4) {
			m_BulletColor = 0;
		}

		if (m_GiddosparioColor == 16) {
			m_GiddosparioColor = 0;
		}
	}

	// 地上クレーターカラー選択
	if (frameCount % 4 == 0) {
		m_CraterColor++;

		if (m_CraterColor == 2) {
			m_CraterColor = 0;
		}
	}

	// 敵点滅カラー選択
	if (frameCount % 8 == 0) {	// 8フレーム毎
		if (m_EnemyColor == 0 || m_EnemyColor == 4) {
			m_IsIncrement = !m_IsIncrement;
		}

		if (m_IsIncrement) {
			m_EnemyColor++;
		}
		else {
			m_EnemyColor--;
		}

	}
}

// 後でバキュラにも対応予定
/******************************************************************
関数名：RandomX

機能：敵出現X座標をランダムに決定する関数です

引数：	bool型	isDistance	(i)	false	: 全体からランダム
								true	: プレイヤーから8キャラクタ以上離れた位置でランダム

返却値： X座標

注意事項：キャラクタ単位の表示画面内のX座標1～25から選択され、正確なX座標に変換して返します
*********************************************************************/
float EnemyManager::RandomX(float playerX, bool isDistance) {
	int x;
	int px;
	px = static_cast<int>(playerX) / CHARACTER_SIZE;
	if (isDistance) {
		do {
			x = m_RandEnemyX(m_Seed);
		} while (x < px + 8 && x >= px - 8);
	}
	else {
		x = m_RandEnemyX(m_Seed);
	}
	return static_cast<float>(CHARACTER_SIZE * x + TEMP_OFFSCREEN_WIDTH);
}

// 地上物生成
int EnemyManager::GroundGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, Enemy* pParentObject) {
	if (characterId == static_cast<int>(EnemyId::SPFLAG)) {		// SPフラッグは専用枠
		delete m_GroundEnemies[0];								// 現在のオブジェクトを削除
		m_GroundEnemies[0] = new SPFlag(RandomX(playerX, true), y, &m_imageHandle, m_pScoreManager);
		return 0;
	}

	if (characterId == static_cast<int>(EnemyId::HIDDEN_MESSAGE)) {		// SPフラッグは専用枠
		delete m_GroundEnemies[0];										// 現在のオブジェクトを削除
		m_GroundEnemies[0] = new HiddenMessage(x, y, m_pScoreManager, m_pUI);
		return 0;
	}

	int i = -1;	// iを一旦-1で初期化する事で、なにもしなかった場合は-1を返す
	if (characterId != 0) {	// 0はなにもしない　ダミーのIDと一致しているので、ダミーを生成したい場合は直接行う事
		for (i = 2; i < GROUND_NUM; i++) {						// オブジェクトの状態をチェック　最初の二つはカーソルが反応しない物（SPフラッグ等）専用枠
			if (m_GroundEnemies[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT)) {	// 対象オブジェクト枠が非存在
				delete m_GroundEnemies[i];							// 現在のオブジェクトを削除
				switch (characterId)						// ここでキャラクタIDで分岐して生成
				{
				case static_cast<int>(EnemyId::BARRA):		// バーラ
					m_GroundEnemies[i] = new Barra(x, y, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::ZOLBAK):		// ゾルバク				
					m_GroundEnemies[i] = new Zolbak(x, y, &m_imageHandle, m_pScoreManager, &m_AerialLevel);
					break;

				case static_cast<int>(EnemyId::GROBDA):	// グロブダーA
					m_GroundEnemies[i] = new Grobda(x, y, type, brokenType, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::LOGRAM):		// ログラム
					m_GroundEnemies[i] = new Logram(x, y, m_shotIntervalMax, &m_imageHandle, m_pScoreManager, pParentObject);
					break;

				case static_cast<int>(EnemyId::BOZA_LOGRAM):// ボザログラム
					BozaLogramGenerater(x, y, characterId, type, brokenType, playerX, i);	// 専用処理　関数に切り出してある
					break;

				case static_cast<int>(EnemyId::DEROTA):		// デロータ
					m_GroundEnemies[i] = new Derota(x, y, m_shotIntervalMax, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::SOL):		// ソル
					m_GroundEnemies[i] = new Sol(x, y, &m_imageHandle, m_pScoreManager);
					break;

				default:									// どれにも一致しなかったら、ポインタが空になっている状況を回避するためダミーを配置
					m_GroundEnemies[i] = new Enemy();
					break;
				}
				break;
			}
		}
	}

	return i != GROUND_NUM ? i : -1;	// 成功なら生成されたID、失敗なら-1を返す
}

// ボザログラム生成　コメント書かなきゃ…
int EnemyManager::BozaLogramGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, int i) {
	if (i >= 0 && i < GROUND_NUM) {									// iが不正な値でないかチェック
		int groundID[BozaLogram::BOZA_LOGRAM_NUM] = { 0x00 };		// 本体とログラムのID
		Enemy** tempPP[BozaLogram::BOZA_LOGRAM_NUM] = { NULL };		// 格納先を示すダブルポインタ

		m_GroundEnemies[i] = new Enemy();									// ダミーを仮生成
		m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::ACTIVE));	// 仮置きのダミーをアクティブにし、同じ位置に生成不可にする

		// 生成した場所のIDを配列に入れていく　最初に本体、最後に上のログラムであること
		groundID[0] = i;
		groundID[1] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET, y + BozaLogram::BOZA_LOGRAM_OFFSET * 2, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);	// ログラム下
		groundID[2] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET * 2, y + BozaLogram::BOZA_LOGRAM_OFFSET, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);	// ログラム左
		groundID[3] = GroundGenerater(x, y + BozaLogram::BOZA_LOGRAM_OFFSET, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);											// ログラム右
		groundID[4] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET, y, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);											// ログラム上

		for (int j = 0; j < BozaLogram::BOZA_LOGRAM_NUM; j++) {		// IDをチェックし、生成できている場合は生成先のポインタをダブルポインタ配列に入れる
			if (groundID[j] != -1) {
				tempPP[j] = &m_GroundEnemies[groundID[j]];
			}
		}

		delete m_GroundEnemies[i];									// 仮置きしていたダミーオブジェクトを削除
		m_GroundEnemies[i] = new BozaLogram(x + BozaLogram::BOZA_LOGRAM_OFFSET, y + BozaLogram::BOZA_LOGRAM_OFFSET, &m_imageHandle, m_pScoreManager, tempPP);	// 本体を生成

		for (int j = 1; j < BozaLogram::BOZA_LOGRAM_NUM; j++) {
			if (groundID[j] != -1) {
				m_GroundEnemies[groundID[j]]->SetPParentObject(m_GroundEnemies[i]);	// 子ログラムにボザログラムのポインタをセット
			}
		}

		return 0;
	}
	return -1;

}

// 空中敵生成
void EnemyManager::AerialGenerater(float playerX, float playerY) {

	if (m_IsAerialGenerate) {

		for (int i = 0; i < AERIAL_NUM; i++) {						// オブジェクトの状態をチェック
			// 対象オブジェクト枠が非存在で、生成対象がダミーじゃない、または生成対象がダミーかつ生成先がダミーじゃない
			if (m_AerialEnemies[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT) &&

				(m_AppAerialPattern[i] != static_cast<int>(EnemyId::DUMMY) ||
					m_AppAerialPattern[i] == static_cast<int>(EnemyId::DUMMY) && m_AerialEnemies[i]->GetCharcterId() != static_cast<int>(EnemyId::DUMMY))) {

				delete m_AerialEnemies[i];					// 現在のオブジェクトを削除
				switch (m_AppAerialPattern[i])				// ここでキャラクタIDで分岐して生成
				{
				case static_cast<int>(EnemyId::TOROID):		// トーロイド
					m_AerialEnemies[i] = new Toroid(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::TORKAN):		// タルケン
					m_AerialEnemies[i] = new Torkan(RandomX(playerX, false), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::ZOSHI):		// ゾシー
					if (m_AppAerialTypePattern[i] == 2) {	// バックゾシーだったら
						m_AerialEnemies[i] = new Zoshi(RandomX(playerX, true), TEMP_SCREEN_HEIGHT, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);

					}
					else {									// それ以外
						m_AerialEnemies[i] = new Zoshi(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					}
					break;

				case static_cast<int>(EnemyId::JARA):		// ジアラ
					m_AerialEnemies[i] = new Jara(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::GIDDO_SPARIO):	// ギドスパリオ
					m_AerialEnemies[i] = new Giddospario(RandomX(playerX, false), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				default:	// どれにも一致しなかったら一応ダミー
					m_AerialEnemies[i] = new Enemy();
					break;
				}
			}
		}
	}
}

// 通常弾生成
void EnemyManager::BulletGenerater(float x, float y, float playerX, float playerY) {
	for (int i = 0; i < SPARIO_NUM; i++) {												// オブジェクトの状態をチェック
		if (m_spario[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT)) {		// 対象オブジェクト枠が非存在
			delete m_spario[i];															// 現在のオブジェクトを削除
			m_spario[i] = new Spario(x, y, playerX, playerY, 0, &m_imageHandle);		// タイプ設定は仮
			break;
		}
	}
}

/*
* 名前：AreaDataCheck
*
* 機能：配置情報をチェックし、敵を生成関連処理を行います
*
* 引数：(i) int playerX
*
* 戻り値：現状0のみ
*/
int EnemyManager::AreaDataCheck(float playerX) {
	int line = m_pAreaManager->GetLine();			// 現在のライン
	int y = m_pAreaManager->GetGroundEnemyLine();	// 現在の地上敵ライン
	int id = 0;										// 生成する敵テーブルID
	GROUND_TABLE tempT;								// 短く表記するための仮敵データ

	if (!m_pAreaManager->GetIsStart()) {			// 開始時でない時

		if (line == 0) {
			GroundAreaDataSet();	// データを元に地上物を割り当て
			LoadLineData();			// 

		}

		for (int i = 0; i < LINE_PROCESS_NUM; i++) {			// ライン処理
			switch (lineProcessData[line][i][0])
			{
			case APP_STOP:										// 空中敵生成停止
				m_IsAerialGenerate = false;						// 生成フラグをfalseに
				break;

			case APP_START_LEVEL:								// レベルに応じた空中敵生成開始
				AerialGenerateStart();							// 生成開始
				break;

			case APP_START_PATTERN:								// 固定パターンでの空中敵生成開始
				AerialPatternSet(lineProcessData[line][0][1]);	// パターンをセット
				m_IsAerialGenerate = true;						// 生成フラグをtrueに
				break;

			case LEVEL_SCORE_UPDATE:							// スコア依存でレベル加算
				LevelUpByScore();
				break;

			case APP_ALONE_ENEMY:								// 敵単体生成
				// 未実装
				break;

			case SHOT_INTERVAL_MAX_CHANGE:						// 地上敵射撃間隔変更
				m_shotIntervalMax = lineProcessData[line][0][1];
				break;

			default:
				break;
			}
			lineProcessData[line][i][0] = 0;

		}
		
		for (int i = AreaManager::AREA_CHARACTER_WIDTH - 1; i >= 0; i--) {
			id = m_GroundAreaData[y][i];		// 敵IDを取得
			if (id != 0) {						// IDが0でない時
				tempT = GROUND_ENEMY_TABLE[id];	// そのIDの敵情報を取得
				GroundGenerater(static_cast<float>(i * CHARACTER_SIZE) + tempT.offsetX, tempT.offsetY,
					tempT.characterID, tempT.type, tempT.brokenType, playerX, NULL);	// 対象IDの情報で生成
				m_GroundAreaData[y][i] = 0;		// 生成したらそこのIDを空に
				return 0;						// 1フレームに一回しか生成しない
			}
		}
	}

	return 0;
}

// 地上物計算
void EnemyManager::GroundCalc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove) {
	float tempX = 0;	// クレーター生成用座標保持変数
	float tempY = 0;

	for (int i = 0; i < GROUND_NUM; i++) {

		// 基本動作
		if (m_GroundEnemies[i]->GetStatus() != static_cast<int>(StatusId::DESTROYED)) {
			m_GroundEnemies[i]->Calc(playerX, playerY, cursorX, cursorY, landingPointX, landingPointY, groundMove, m_enemyHit.ground[i]);	// 基本の計算

			if (m_GroundEnemies[i]->GetIsShot()) {	// ショットフラグが立っていたら弾を生成してfalseに
				m_GroundEnemies[i]->SetIsShotFalse();
				BulletGenerater(m_GroundEnemies[i]->GetX(), m_GroundEnemies[i]->GetY(), playerX, playerY);
			}

		}
		else {
			tempX = m_GroundEnemies[i]->GetX();					// 座標を保持
			tempY = m_GroundEnemies[i]->GetY();
			delete m_GroundEnemies[i];							// 現在のオブジェクトを削除
			m_GroundEnemies[i] = new Crater(tempX, tempY, &m_imageHandle);	// クレーター生成
		}

		// 上以外の画面外で消滅
		if (OffScreenChecker2(m_GroundEnemies[i]->GetX(), m_GroundEnemies[i]->GetY())) {
			m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// 空中敵計算
void EnemyManager::AerialCalc(float playerX, float playerY, float groundMove) {
	for (int i = 0; i < AERIAL_NUM; i++) {

		m_AerialEnemies[i]->Calc(playerX, playerY, groundMove, m_enemyHit.aerial[i]);

		if (m_AerialEnemies[i]->GetIsShot()) {	// ショットフラグが立っていたら弾生成
			m_AerialEnemies[i]->SetIsShotFalse();	// ショットフラグをfalseに
			BulletGenerater(m_AerialEnemies[i]->GetX(), m_AerialEnemies[i]->GetY(), playerX, playerY);	// 弾生成
		}

		// 画面外で消滅
		if (OffScreenChecker(m_AerialEnemies[i]->GetX(), m_AerialEnemies[i]->GetY())) {
			m_AerialEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// 弾計算
void EnemyManager::SparioCalc() {
	for (int i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->Calc();

		// 画面外で消滅
		if (OffScreenChecker(m_spario[i]->GetX(), m_spario[i]->GetY())) {
			m_spario[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// 地上物描画
void EnemyManager::GroundDraw() {
	for (int i = 0; i < GROUND_NUM; i++) {
		switch (m_GroundEnemies[i]->GetCharcterId())
		{
		case static_cast<int>(EnemyId::CRATER):
			m_GroundEnemies[i]->Draw(m_CraterColor);
			break;

		default:
			m_GroundEnemies[i]->Draw(m_EnemyColor);
			break;
		}
	}
}

// 弾描画
void EnemyManager::SparioDraw() {
	for (int i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->Draw(m_BulletColor);
	}
}

// 空中敵描画
void EnemyManager::AerialDraw() {
	for (int i = 0; i < AERIAL_NUM; i++) {
		switch (m_AerialEnemies[i]->GetCharcterId())
		{
		case static_cast<int>(EnemyId::GIDDO_SPARIO):
			m_AerialEnemies[i]->Draw(m_GiddosparioColor);
			break;

		default:
			m_AerialEnemies[i]->Draw(m_EnemyColor);
			break;
		}
	}
}

// 画面外チェック　引数のx, y座標が画面外だったらtrue 画面内だったらfalseを返します
// 空中敵用　地上敵はOffScreenChecker2を使用してください
bool EnemyManager::OffScreenChecker(float x, float y) {
	if (y > TEMP_SCREEN_HEIGHT || x > TEMP_SCREEN_WIDTH ||
		x <= 0 || y < 0) {
		return true;
	}
	return false;
}

// 画面外チェック2　画面上方向を除く三方向で画面外だったらtrue 画面内だったらfalseを返します
// 地上敵用　空中敵はOffScreenCheckerを使用してください
bool EnemyManager::OffScreenChecker2(float x, float y) {
	if (y > TEMP_SCREEN_HEIGHT || x > TEMP_SCREEN_WIDTH ||
		x <= 0) {
		return true;
	}
	return false;
}

/******************************************************************
関数名：GetAerialX

機能：引数で指定したIDの空中敵のX座標を返します

引数：	int型	aerialEnemiesId	(i)	空中敵のID

返却値：
	成功：X座標
	失敗：-1
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：敵がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetAerialX(int aerialEnemiesId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (aerialEnemiesId < AERIAL_NUM && m_AerialEnemies[aerialEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_AerialEnemies[aerialEnemiesId]->GetX();
	}
	return -1;
}

/******************************************************************
関数名：GetAerialY

機能：引数で指定したIDの空中敵のY座標を返します

引数：	int型	aerialEnemiesId	(i)	空中敵のID

返却値：
	成功：Y座標
	失敗：-1
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：敵がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetAerialY(int aerialEnemiesId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (aerialEnemiesId < AERIAL_NUM && m_AerialEnemies[aerialEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_AerialEnemies[aerialEnemiesId]->GetY();
	}
	return -1;
}

/******************************************************************
関数名：GetAerialStatus

機能：引数で指定したIDの空中敵の状態を返します

引数：	int型	aerialEnemiesId	(i)	空中敵のID

返却値：
	int型
		成功：状態
		失敗：-1
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：不正なIDの場合-1を返します
*********************************************************************/
int EnemyManager::GetAerialStatus(int aerialEnemiesId) {
	// 引数が正しいIDの場合
	if (aerialEnemiesId < AERIAL_NUM) {
		return m_AerialEnemies[aerialEnemiesId]->GetStatus();
	}
	return -1;
}

/******************************************************************
関数名：GetAerialStatus

機能：引数で指定したIDの空中敵の状態を返します

引数：	int型	aerialEnemiesId	(i)	空中敵のID

返却値：
	int型
		成功：状態
		失敗：-1
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：不正なIDの場合-1を返します
*********************************************************************/
int EnemyManager::GetBulletStatus(int bulletId) {
	// 引数が正しいIDの場合
	if (bulletId < SPARIO_NUM) {
		return m_spario[bulletId]->GetStatus();
	}
	return -1;
}

/******************************************************************
関数名：GetAerialIsCollision

機能：引数で指定したIDの空中敵の接触判定の有無を返します

引数：	int型	aerialEnemiesId	(i)	空中敵のID

返却値：
	bool型
		成功：判定あり：true　判定無し：false
		失敗：false
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：不正なIDの場合falseを返します
*********************************************************************/
bool EnemyManager::GetAerialIsCollision(int aerialEnemiesId) {
	if (aerialEnemiesId < AERIAL_NUM) {
		return m_AerialEnemies[aerialEnemiesId]->GetIsCollision();
	}
	return false;
}

/******************************************************************
関数名：GetGroundCharcterId

機能：引数で指定したIDの地上敵のキャラクタIDを返します

引数：	int型	groundEnemiesId	(i)	地上敵のID

返却値：
	成功：キャラクタID
	失敗：-1
注意事項：
	１：groundEnemiesIdにはGROUND_NUM未満の値を使用してください
	２：敵がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
int EnemyManager::GetGroundCharcterId(int groundEnemiesId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetCharcterId();
	}
	return -1;
}

/******************************************************************
関数名：GetGroundX

機能：引数で指定したIDの地上敵のX座標を返します

引数：	int型	groundEnemiesId	(i)	空中敵のID

返却値：
	成功：X座標
	失敗：-1
注意事項：
	１：groundEnemiesIdにはGROUND_NUM未満の値を使用してください
	２：敵がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetGroundX(int groundEnemiesId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetX();
	}
	return -1;
}

/******************************************************************
関数名：GetGroundY

機能：引数で指定したIDの地上敵のY座標を返します

引数：	int型	groundEnemiesId	(i)	地上敵のID

返却値：
	成功：Y座標
	失敗：-1
注意事項：
	１：groundEnemiesIdにはGROUND_NUM未満の値を使用してください
	２：敵がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetGroundY(int groundEnemiesId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetY();
	}
	return -1;
}

/******************************************************************
関数名：GetGroundStatus

機能：引数で指定したIDの地上敵の状態を返します

引数：	int型	aerialEnemiesId	(i)	地上敵のID

返却値：
	int型
		成功：状態
		失敗：-1
注意事項：
	１：aerialEnemiesIdにはAERIAL_NUM未満の値を使用してください
	２：不正なIDの場合-1を返します
*********************************************************************/
int EnemyManager::GetGroundStatus(int groundEnemiesId) {
	// 引数が正しいIDの場合
	if (groundEnemiesId < GROUND_NUM) {
		return m_GroundEnemies[groundEnemiesId]->GetStatus();
	}
	return -1;
}

/******************************************************************
関数名：GetGroundIsCollision

機能：引数で指定したIDの地上敵の接触判定の有無を返します

引数：	int型	groundEnemiesId	(i)	空中敵のID

返却値：
	bool型
		成功：判定あり：true　判定無し：false
		失敗：false
注意事項：
	１：groundEnemiesIdにはGROUND_NUM未満の値を使用してください
	２：不正なIDの場合falseを返します
*********************************************************************/
bool EnemyManager::GetGroundIsCollision(int groundEnemiesId) {
	if (groundEnemiesId < GROUND_NUM) {
		return m_GroundEnemies[groundEnemiesId]->GetIsCollision();
	}
	return false;
}

/******************************************************************
関数名：GetBulletX

機能：引数で指定したIDの敵通常弾のX座標を返します

引数：	int型	sparioId	(i)	弾のID

返却値：
	成功：X座標
	失敗：-1
注意事項：
	１：sparioIdにはSPARIO_NUM未満の値を使用してください
	２：弾がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetBulletX(int bulletId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (bulletId < SPARIO_NUM && m_spario[bulletId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_spario[bulletId]->GetX();
	}
	return -1;
}

/******************************************************************
関数名：GetBulletY

機能：引数で指定したIDの敵通常弾のX座標を返します

引数：	int型	sparioId	(i)	弾のID

返却値：
	成功：Y座標
	失敗：-1
注意事項：
	１：sparioIdにはSPARIO_NUM未満の値を使用してください
	２：弾がアクティブでない、または不正なIDの場合-1を返します
*********************************************************************/
float EnemyManager::GetBulletY(int bulletId) {
	// 引数が正しいID、かつ対象がアクティブの場合
	if (bulletId < SPARIO_NUM && m_spario[bulletId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_spario[bulletId]->GetY();
	}
	return -1;
}

int EnemyManager::GetAerialLevel() {
	return m_AerialLevel;
}