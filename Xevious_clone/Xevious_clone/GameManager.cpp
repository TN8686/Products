#include "GameManager.h"

GameManager::GameManager(bool isDebug) {
	m_isDebug = isDebug;	// ゲーム内デバッグモードフラグ
	//m_isDebug = true;		// 展示用
	m_isResetFirst = true;	// リセットキー初回フラグ

	// 描画可能な仮画面を作成
	SetDrawValidGraphCreateFlag(TRUE);	// 描画可能な画像のハンドルを作成するモードに変更

	// 原作の2フレーム描画遅延を再現するため、3つ仮画面を用意
	for (int i = 0; i < TEMP_SCREEN_NUM; ++i) {
		tempScreen[i] = MakeGraph(TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH);	// 仮画面作成、tempScreenにハンドルをセット
	}
	uiScreen = MakeScreen(TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT, true);

	SetDrawValidGraphCreateFlag(FALSE);		// モード解除

	mode = static_cast<int>(ModeId::TITLE);	// 現在のモード

	frameCount = 0;							// フレームカウント
	missFrameCount = MISS_FRAME;			// ミスフレームカウント
	isRestart = false;						// リスタートフラグ
	restartFrameCount = RESTART_FRAME;		// リスタートフレームカウント
	isGameOver = false;						// ゲームオーバー表示フラグ
	gameOverCount = GAME_OVER_FRAME;		// ゲームオーバー表示フレームカウント

	memset(keyBuf, 0x00, sizeof(keyBuf));	// キー入力用受け取り用配列初期化
	padBuf = 0;								// パッド入力受け取り用

	isStartBgmAfter = false;				// 開始時BGM終了後フラグ

	debuger = new Debug(m_isDebug);			// デバッガーの実体宣言
	soundManager = new SoundManager;		// サウンドマネージャの実体宣言
	areaManager = new AreaManager;			// エリアマネージャの実体宣言
	player = new Player(soundManager);		// プレイヤーの実体宣言
	backGround = new BackGround;			// バックグラウンドの実体宣言
	scoreManager = new ScoreManager(player, soundManager);							// スコアマネージャの実体宣言
	bulletManager = new BulletManager(soundManager);								// バレットマネージャの実体宣言
	ui = new UI(uiScreen, scoreManager, player, &padBuf);							// UIの実体宣言
	enemyManager = new EnemyManager(scoreManager, areaManager, soundManager, ui);	// エネミーマネージャの実体宣言
	hitChecker = new HitChecker(enemyManager, bulletManager, player, soundManager);	// ヒットチェッカーの実体宣言

	TitleInit();
}

GameManager::~GameManager() {
	// オブジェクトのメモリを解放
	delete debuger;			// デバッガー
	delete soundManager;	// サウンドマネージャ
	delete areaManager;		// エリアマネージャ
	delete player;			// プレイヤー
	delete backGround;		// バックグラウンド
	delete scoreManager;	// スコアマネージャ
	delete bulletManager;	// バレットマネージャ
	delete enemyManager;	// エネミーマネージャ
	delete hitChecker;		// ヒットチェッカー
	delete ui;				// UI
}

// メインループ関数
int GameManager::MainLoop() {
	//while (ProcessMessage() == 0 && !(keyBuf[KEY_INPUT_ESCAPE] || padBuf & PAD_INPUT_9))	// パッドから終了できるバージョン
	while (ProcessMessage() == 0 && !(keyBuf[KEY_INPUT_ESCAPE]))							// パッドから終了できないバージョン
	{

		// 現在のカウントを取得する
		firstTime = GetNowHiPerformanceCount();


		// キー入力チェック
		GetHitKeyStateAll(keyBuf);
		padBuf = GetJoypadInputState(DX_INPUT_KEY_PAD1);

		if (m_isDebug && keyBuf[KEY_INPUT_R]) {
			if (m_isResetFirst) {
				m_isResetFirst = false;
				soundManager->BgmStop();
				soundManager->AndorgenesisSeStop();
				TitleInit();
			}
		}else {
			m_isResetFirst = true;	// リセットキー初回フラグをtrueに
		}

		// 計算
		soundManager->Calc();

		if (debuger->CalcStop(keyBuf, padBuf)) {
			switch (mode)
			{
			case static_cast<int>(ModeId::TITLE):
				TitleCalc();		// 計算
				break;

			case static_cast<int>(ModeId::MAIN_GAME):
				MainGameCalc();		// 計算
				break;

			case static_cast<int>(ModeId::NAME_ENTRY):
				NameEntryCalc();	// 計算
				break;

			default:
				return -1;
				break;
			}

			// フレームカウントをインクリメント
			frameCount++;
		}

		// 描画
		// 描画先を仮画面にしてクリア
		SetDrawScreen(tempScreen[frameCount % 3]);
		ClearDrawScreen();

		// 仮に表示領域と画面外領域を色分け
		DrawBox(0, 0, TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH, GetColor(0, 255, 0), TRUE);
		DrawBox(TEMP_OFFSCREEN_HEIGHT, TEMP_OFFSCREEN_WIDTH, TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH, GetColor(0, 0, 0), TRUE);

		switch (mode)
		{
		case static_cast<int>(ModeId::TITLE):
			TitleDraw();		// 描画
			break;

		case static_cast<int>(ModeId::MAIN_GAME):
			MainGameDraw();		// 描画
			break;

		case static_cast<int>(ModeId::NAME_ENTRY):
			NameEntryDraw();	// 描画
			break;

		default:
			return -1;
			break;
		}

		// 描画先を裏画面に変更
		SetDrawScreen(DX_SCREEN_BACK);

		/*
		* 仮画面の内容を90度回転、2倍に拡大して、画面外領域がはみ出すように裏画面に描画
		* DrawRotaGraph()関数は描画先の中心座標を指定する方式のため、ちょっと分かりづらい式になっている
		* X座標はTEMP_SCREEN_WIDTH * 2 / 2　描画先のX座標なので左が原点　よってTEMP_OFFSCREEN_WIDTHは考慮しなくても画面外領域はカットされる
		* Y座標は(TEMP_SCREEN_HEIGHT * 2 / 2) - (TEMP_OFFSCREEN_HEIGHT * 2)
		* 描画先のY座標なので上が原点　よってTEMP_OFFSCREEN_HEIGHTを考慮する必要があり、その分ズラして画面外領域をカットしている
		*/
		DrawRotaGraph(TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT - (TEMP_OFFSCREEN_HEIGHT + TEMP_OFFSCREEN_HEIGHT), 2.0f, PI / 2, tempScreen[(frameCount + 1) % TEMP_SCREEN_NUM], TRUE);

		// UI画面は横にしていないので回転せず　原作ではUI画面が3ドット右にズレているため、拡大分込みで6ドット右にズラす
		DrawRotaGraph(TEMP_SCREEN_WIDTH + 6, TEMP_SCREEN_HEIGHT - (TEMP_OFFSCREEN_HEIGHT + TEMP_OFFSCREEN_HEIGHT), 2.0f, 0, uiScreen, TRUE);
		// DrawExtendGraph版
		//DrawExtendGraph(4 , -TEMP_OFFSCREEN_HEIGHT * 2, 4 + TEMP_SCREEN_WIDTH * 2, TEMP_SCREEN_HEIGHT * 2 - TEMP_OFFSCREEN_HEIGHT * 2, uiScreen, TRUE);

		// デバッグウインドウに描画
		debuger->DrawDebugWindow(tempScreen[frameCount % TEMP_SCREEN_NUM], player, scoreManager->GetScore(), areaManager, enemyManager->GetAerialLevel());

		// 裏画面の内容を表画面に反映
		ScreenFlip();

		// 原作のフレームレートと合わせるため、16.5ミリ秒(約秒間60.6フレームの時の1フレームあたりの経過時間)経過するまでここで待つ
		while (GetNowHiPerformanceCount() - firstTime < MICROSECONDS_PER_FRAME);
	}

	return 0;
}

int GameManager::TitleInit() {
	mode = static_cast<int>(ModeId::TITLE);	// モードをタイトルに

	soundManager->NoisePlay(static_cast<int>(NoiseSeId::CreditSound));	// 現状1クレジット入り状態のみなので開幕クレジット音
	return 0;
}

int GameManager::MainGameInit() {
	mode = static_cast<int>(ModeId::MAIN_GAME);	// モードをメインゲームに

	// 初期化
	player->Init();
	areaManager->Init();
	enemyManager->Init();
	backGround->Init();
	scoreManager->Init();
	ui->Init();

	isStartBgmAfter = false;	// 開始時BGM終了後フラグ
	soundManager->BgmPlay(static_cast<int>(BgmId::Game_Start), false);	// BGMを再生

	return 0;
}

int GameManager::NameEntryInit() {
	mode = static_cast<int>(ModeId::NAME_ENTRY);	// モードをネームエントリーに

	int ret = ui->NameEntryInit();

	switch (ret)
	{
	case 0:																		// 一位
		soundManager->BgmPlay(static_cast<int>(BgmId::Name_Entry_01), true);	// BGM再生
		break;

	case 1:																		// 二位～五位
	case 2:
	case 3:
	case 4:
		soundManager->BgmPlay(static_cast<int>(BgmId::Name_Entry_02), true);	// BGM再生
		break;

	case -1:																	// ランク外
		isGameOver = true;
		break;

	default:
		break;
	}

	return ret;
}

int GameManager::TitleCalc() {
	ui->Calc(mode);

	if (keyBuf[KEY_INPUT_S] || padBuf & PAD_INPUT_5) {	// とりあえず仮にSキーでスタート
		MainGameInit(); // メインゲームモード用初期化
	}

	return 0;
}

// メインゲーム計算関数
int GameManager::MainGameCalc() {
	if (!isRestart) {																// リスタート中でない

		if (player->GetOnActive()) {
			// 開始BGMが終わったらメインBGMを再生開始する
			if (!(isStartBgmAfter || soundManager->GameStartBgmCheck())) {
				isStartBgmAfter = true;
				soundManager->BgmPlay(static_cast<int>(BgmId::BGM_loop), true);
			}
			// 弾生成処理
			//bulletManager->CreateZapper(player->GetX(), player->GetY(), keyBuf[KEY_INPUT_Z]);
			//bulletManager->CreateBlaster(player->GetX(), player->GetY(), keyBuf[KEY_INPUT_X]);
			bulletManager->CreateZapper(player->GetX(), player->GetY(), padBuf & PAD_INPUT_1 + PAD_INPUT_3);
			bulletManager->CreateBlaster(player->GetX(), player->GetY(), padBuf & PAD_INPUT_2 + PAD_INPUT_4);
		}
		else {
			missFrameCount--;
			if (missFrameCount == 0) {
				soundManager->AndorgenesisSeStop();
				player->Restart();
				areaManager->Restart();
				enemyManager->Restart();
				backGround->Init();
				missFrameCount = MISS_FRAME;
				isRestart = true;
			}
		}

		// エネミーマネージャ側で行う全体に関わる計算
		enemyManager->Calc(frameCount);

		// 空中敵生成処理
		enemyManager->AreaDataCheck(player->GetX());
		enemyManager->AerialGenerater(player->GetX(), player->GetY());

		// ヒットチェック
		hitChecker->Calc();

		// 計算系
		areaManager->Calc(backGround->GetSpeed());															// エリア
		backGround->Calc(areaManager->GetAria(), areaManager->GetLine(), areaManager->GetIsStart());		// 背景
		enemyManager->SparioCalc();																			// 敵弾
		enemyManager->GroundCalc(player->GetX(), player->GetY(), bulletManager->GetCursorX(), bulletManager->GetCursorY(),
			bulletManager->GetLandingPointX(), bulletManager->GetLandingPointY(), backGround->GetSpeed());	// 地上物
		bulletManager->Calc(frameCount);																	// ザッパー
		player->Calc(keyBuf, padBuf, frameCount);															// プレイヤー
		bulletManager->BlasterCalc();																		// ブラスター
		if (player->GetOnActive()) {
			bulletManager->CursorCalc(player->GetX(), player->GetY(), frameCount);							// カーソル
		}
		bulletManager->LandingPointCalc();																	// 着弾カーソル
		enemyManager->AerialCalc(player->GetX(), player->GetY(), backGround->GetSpeed());					// 空中敵
		ui->Calc(mode);

	}
	else {																			// リスタート中である

		restartFrameCount--;														// リスタートフレームカウントをデクリメント
		if (restartFrameCount == 0) {												// フレームカウントが0の時
			isRestart = false;														// リスタートフラグをfalseに
			restartFrameCount = RESTART_FRAME;										// リスタートフレーム初期化

			if (player->GetStock() >= 0) {											// ストックがまだある
				isStartBgmAfter = false;											// 開始時BGM終了後フラグ初期化
				soundManager->BgmPlay(static_cast<int>(BgmId::Game_Start), false);	// BGMを再生する
				ui->Init();															// UI初期化

			}
			else {																	// ストックが無い
				NameEntryInit();													// ネームエントリーへ移行
			}
		}
	}

	return 0;
}

int GameManager::NameEntryCalc() {
		
	if (isGameOver) {							// ゲームオーバーフラグがtrue
		soundManager->BgmStop();
		gameOverCount--;						// ゲームオーバーフレームカウントをデクリメント
		if (gameOverCount == 0) {				// ゲームオーバーフレームカウントが0だったら
			isGameOver = false;					// 初期化
			gameOverCount = GAME_OVER_FRAME;	// 初期化
			TitleInit();						// タイトルへ戻る
		}
	}
	else {
		if (ui->Calc(mode) == 1) {
			isGameOver = true;
		}
	}

	return 0;
}

int GameManager::TitleDraw() {

	// 描画先をUI画面にしてクリア
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI描画


	return 0;
}

// メインゲーム描画関数
int GameManager::MainGameDraw() {

	// 描画系
	backGround->Draw();						// 背景
	if (!isRestart) {
		enemyManager->GroundDraw();			// 地上物
		bulletManager->LandingPointDraw();	// 着弾カーソル
		bulletManager->BlasterDraw();		// ブラスター
		bulletManager->CursorDraw();		// カーソル
		player->Draw();						// プレイヤー
		bulletManager->ZapperDraw();		// ザッパー
		enemyManager->SparioDraw();			// 敵弾
		enemyManager->AerialDraw();			// 空中敵
	}

	// 描画先をUI画面にしてクリア
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI

	return 0;
}

// ネームエントリー描画関数
int GameManager::NameEntryDraw() {
	backGround->Draw();							// 背景

	// 描画先をUI画面にしてクリア
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI

	return 0;
}