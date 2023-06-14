#include "GameManager.h"

GameManager::GameManager(bool isDebug) {
	m_isDebug = isDebug;	// �Q�[�����f�o�b�O���[�h�t���O
	//m_isDebug = true;		// �W���p
	m_isResetFirst = true;	// ���Z�b�g�L�[����t���O

	// �`��\�ȉ���ʂ��쐬
	SetDrawValidGraphCreateFlag(TRUE);	// �`��\�ȉ摜�̃n���h�����쐬���郂�[�h�ɕύX

	// �����2�t���[���`��x�����Č����邽�߁A3����ʂ�p��
	for (int i = 0; i < TEMP_SCREEN_NUM; ++i) {
		tempScreen[i] = MakeGraph(TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH);	// ����ʍ쐬�AtempScreen�Ƀn���h�����Z�b�g
	}
	uiScreen = MakeScreen(TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT, true);

	SetDrawValidGraphCreateFlag(FALSE);		// ���[�h����

	mode = static_cast<int>(ModeId::TITLE);	// ���݂̃��[�h

	frameCount = 0;							// �t���[���J�E���g
	missFrameCount = MISS_FRAME;			// �~�X�t���[���J�E���g
	isRestart = false;						// ���X�^�[�g�t���O
	restartFrameCount = RESTART_FRAME;		// ���X�^�[�g�t���[���J�E���g
	isGameOver = false;						// �Q�[���I�[�o�[�\���t���O
	gameOverCount = GAME_OVER_FRAME;		// �Q�[���I�[�o�[�\���t���[���J�E���g

	memset(keyBuf, 0x00, sizeof(keyBuf));	// �L�[���͗p�󂯎��p�z�񏉊���
	padBuf = 0;								// �p�b�h���͎󂯎��p

	isStartBgmAfter = false;				// �J�n��BGM�I����t���O

	debuger = new Debug(m_isDebug);			// �f�o�b�K�[�̎��̐錾
	soundManager = new SoundManager;		// �T�E���h�}�l�[�W���̎��̐錾
	areaManager = new AreaManager;			// �G���A�}�l�[�W���̎��̐錾
	player = new Player(soundManager);		// �v���C���[�̎��̐錾
	backGround = new BackGround;			// �o�b�N�O���E���h�̎��̐錾
	scoreManager = new ScoreManager(player, soundManager);							// �X�R�A�}�l�[�W���̎��̐錾
	bulletManager = new BulletManager(soundManager);								// �o���b�g�}�l�[�W���̎��̐錾
	ui = new UI(uiScreen, scoreManager, player, &padBuf);							// UI�̎��̐錾
	enemyManager = new EnemyManager(scoreManager, areaManager, soundManager, ui);	// �G�l�~�[�}�l�[�W���̎��̐錾
	hitChecker = new HitChecker(enemyManager, bulletManager, player, soundManager);	// �q�b�g�`�F�b�J�[�̎��̐錾

	TitleInit();
}

GameManager::~GameManager() {
	// �I�u�W�F�N�g�̃����������
	delete debuger;			// �f�o�b�K�[
	delete soundManager;	// �T�E���h�}�l�[�W��
	delete areaManager;		// �G���A�}�l�[�W��
	delete player;			// �v���C���[
	delete backGround;		// �o�b�N�O���E���h
	delete scoreManager;	// �X�R�A�}�l�[�W��
	delete bulletManager;	// �o���b�g�}�l�[�W��
	delete enemyManager;	// �G�l�~�[�}�l�[�W��
	delete hitChecker;		// �q�b�g�`�F�b�J�[
	delete ui;				// UI
}

// ���C�����[�v�֐�
int GameManager::MainLoop() {
	//while (ProcessMessage() == 0 && !(keyBuf[KEY_INPUT_ESCAPE] || padBuf & PAD_INPUT_9))	// �p�b�h����I���ł���o�[�W����
	while (ProcessMessage() == 0 && !(keyBuf[KEY_INPUT_ESCAPE]))							// �p�b�h����I���ł��Ȃ��o�[�W����
	{

		// ���݂̃J�E���g���擾����
		firstTime = GetNowHiPerformanceCount();


		// �L�[���̓`�F�b�N
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
			m_isResetFirst = true;	// ���Z�b�g�L�[����t���O��true��
		}

		// �v�Z
		soundManager->Calc();

		if (debuger->CalcStop(keyBuf, padBuf)) {
			switch (mode)
			{
			case static_cast<int>(ModeId::TITLE):
				TitleCalc();		// �v�Z
				break;

			case static_cast<int>(ModeId::MAIN_GAME):
				MainGameCalc();		// �v�Z
				break;

			case static_cast<int>(ModeId::NAME_ENTRY):
				NameEntryCalc();	// �v�Z
				break;

			default:
				return -1;
				break;
			}

			// �t���[���J�E���g���C���N�������g
			frameCount++;
		}

		// �`��
		// �`��������ʂɂ��ăN���A
		SetDrawScreen(tempScreen[frameCount % 3]);
		ClearDrawScreen();

		// ���ɕ\���̈�Ɖ�ʊO�̈��F����
		DrawBox(0, 0, TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH, GetColor(0, 255, 0), TRUE);
		DrawBox(TEMP_OFFSCREEN_HEIGHT, TEMP_OFFSCREEN_WIDTH, TEMP_SCREEN_HEIGHT, TEMP_SCREEN_WIDTH, GetColor(0, 0, 0), TRUE);

		switch (mode)
		{
		case static_cast<int>(ModeId::TITLE):
			TitleDraw();		// �`��
			break;

		case static_cast<int>(ModeId::MAIN_GAME):
			MainGameDraw();		// �`��
			break;

		case static_cast<int>(ModeId::NAME_ENTRY):
			NameEntryDraw();	// �`��
			break;

		default:
			return -1;
			break;
		}

		// �`���𗠉�ʂɕύX
		SetDrawScreen(DX_SCREEN_BACK);

		/*
		* ����ʂ̓��e��90�x��]�A2�{�Ɋg�債�āA��ʊO�̈悪�͂ݏo���悤�ɗ���ʂɕ`��
		* DrawRotaGraph()�֐��͕`���̒��S���W���w�肷������̂��߁A������ƕ�����Â炢���ɂȂ��Ă���
		* X���W��TEMP_SCREEN_WIDTH * 2 / 2�@�`����X���W�Ȃ̂ō������_�@�����TEMP_OFFSCREEN_WIDTH�͍l�����Ȃ��Ă���ʊO�̈�̓J�b�g�����
		* Y���W��(TEMP_SCREEN_HEIGHT * 2 / 2) - (TEMP_OFFSCREEN_HEIGHT * 2)
		* �`����Y���W�Ȃ̂ŏオ���_�@�����TEMP_OFFSCREEN_HEIGHT���l������K�v������A���̕��Y�����ĉ�ʊO�̈���J�b�g���Ă���
		*/
		DrawRotaGraph(TEMP_SCREEN_WIDTH, TEMP_SCREEN_HEIGHT - (TEMP_OFFSCREEN_HEIGHT + TEMP_OFFSCREEN_HEIGHT), 2.0f, PI / 2, tempScreen[(frameCount + 1) % TEMP_SCREEN_NUM], TRUE);

		// UI��ʂ͉��ɂ��Ă��Ȃ��̂ŉ�]�����@����ł�UI��ʂ�3�h�b�g�E�ɃY���Ă��邽�߁A�g�啪���݂�6�h�b�g�E�ɃY����
		DrawRotaGraph(TEMP_SCREEN_WIDTH + 6, TEMP_SCREEN_HEIGHT - (TEMP_OFFSCREEN_HEIGHT + TEMP_OFFSCREEN_HEIGHT), 2.0f, 0, uiScreen, TRUE);
		// DrawExtendGraph��
		//DrawExtendGraph(4 , -TEMP_OFFSCREEN_HEIGHT * 2, 4 + TEMP_SCREEN_WIDTH * 2, TEMP_SCREEN_HEIGHT * 2 - TEMP_OFFSCREEN_HEIGHT * 2, uiScreen, TRUE);

		// �f�o�b�O�E�C���h�E�ɕ`��
		debuger->DrawDebugWindow(tempScreen[frameCount % TEMP_SCREEN_NUM], player, scoreManager->GetScore(), areaManager, enemyManager->GetAerialLevel());

		// ����ʂ̓��e��\��ʂɔ��f
		ScreenFlip();

		// ����̃t���[�����[�g�ƍ��킹�邽�߁A16.5�~���b(��b��60.6�t���[���̎���1�t���[��������̌o�ߎ���)�o�߂���܂ł����ő҂�
		while (GetNowHiPerformanceCount() - firstTime < MICROSECONDS_PER_FRAME);
	}

	return 0;
}

int GameManager::TitleInit() {
	mode = static_cast<int>(ModeId::TITLE);	// ���[�h���^�C�g����

	soundManager->NoisePlay(static_cast<int>(NoiseSeId::CreditSound));	// ����1�N���W�b�g�����Ԃ݂̂Ȃ̂ŊJ���N���W�b�g��
	return 0;
}

int GameManager::MainGameInit() {
	mode = static_cast<int>(ModeId::MAIN_GAME);	// ���[�h�����C���Q�[����

	// ������
	player->Init();
	areaManager->Init();
	enemyManager->Init();
	backGround->Init();
	scoreManager->Init();
	ui->Init();

	isStartBgmAfter = false;	// �J�n��BGM�I����t���O
	soundManager->BgmPlay(static_cast<int>(BgmId::Game_Start), false);	// BGM���Đ�

	return 0;
}

int GameManager::NameEntryInit() {
	mode = static_cast<int>(ModeId::NAME_ENTRY);	// ���[�h���l�[���G���g���[��

	int ret = ui->NameEntryInit();

	switch (ret)
	{
	case 0:																		// ���
		soundManager->BgmPlay(static_cast<int>(BgmId::Name_Entry_01), true);	// BGM�Đ�
		break;

	case 1:																		// ��ʁ`�܈�
	case 2:
	case 3:
	case 4:
		soundManager->BgmPlay(static_cast<int>(BgmId::Name_Entry_02), true);	// BGM�Đ�
		break;

	case -1:																	// �����N�O
		isGameOver = true;
		break;

	default:
		break;
	}

	return ret;
}

int GameManager::TitleCalc() {
	ui->Calc(mode);

	if (keyBuf[KEY_INPUT_S] || padBuf & PAD_INPUT_5) {	// �Ƃ肠��������S�L�[�ŃX�^�[�g
		MainGameInit(); // ���C���Q�[�����[�h�p������
	}

	return 0;
}

// ���C���Q�[���v�Z�֐�
int GameManager::MainGameCalc() {
	if (!isRestart) {																// ���X�^�[�g���łȂ�

		if (player->GetOnActive()) {
			// �J�nBGM���I������烁�C��BGM���Đ��J�n����
			if (!(isStartBgmAfter || soundManager->GameStartBgmCheck())) {
				isStartBgmAfter = true;
				soundManager->BgmPlay(static_cast<int>(BgmId::BGM_loop), true);
			}
			// �e��������
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

		// �G�l�~�[�}�l�[�W�����ōs���S�̂Ɋւ��v�Z
		enemyManager->Calc(frameCount);

		// �󒆓G��������
		enemyManager->AreaDataCheck(player->GetX());
		enemyManager->AerialGenerater(player->GetX(), player->GetY());

		// �q�b�g�`�F�b�N
		hitChecker->Calc();

		// �v�Z�n
		areaManager->Calc(backGround->GetSpeed());															// �G���A
		backGround->Calc(areaManager->GetAria(), areaManager->GetLine(), areaManager->GetIsStart());		// �w�i
		enemyManager->SparioCalc();																			// �G�e
		enemyManager->GroundCalc(player->GetX(), player->GetY(), bulletManager->GetCursorX(), bulletManager->GetCursorY(),
			bulletManager->GetLandingPointX(), bulletManager->GetLandingPointY(), backGround->GetSpeed());	// �n�㕨
		bulletManager->Calc(frameCount);																	// �U�b�p�[
		player->Calc(keyBuf, padBuf, frameCount);															// �v���C���[
		bulletManager->BlasterCalc();																		// �u���X�^�[
		if (player->GetOnActive()) {
			bulletManager->CursorCalc(player->GetX(), player->GetY(), frameCount);							// �J�[�\��
		}
		bulletManager->LandingPointCalc();																	// ���e�J�[�\��
		enemyManager->AerialCalc(player->GetX(), player->GetY(), backGround->GetSpeed());					// �󒆓G
		ui->Calc(mode);

	}
	else {																			// ���X�^�[�g���ł���

		restartFrameCount--;														// ���X�^�[�g�t���[���J�E���g���f�N�������g
		if (restartFrameCount == 0) {												// �t���[���J�E���g��0�̎�
			isRestart = false;														// ���X�^�[�g�t���O��false��
			restartFrameCount = RESTART_FRAME;										// ���X�^�[�g�t���[��������

			if (player->GetStock() >= 0) {											// �X�g�b�N���܂�����
				isStartBgmAfter = false;											// �J�n��BGM�I����t���O������
				soundManager->BgmPlay(static_cast<int>(BgmId::Game_Start), false);	// BGM���Đ�����
				ui->Init();															// UI������

			}
			else {																	// �X�g�b�N������
				NameEntryInit();													// �l�[���G���g���[�ֈڍs
			}
		}
	}

	return 0;
}

int GameManager::NameEntryCalc() {
		
	if (isGameOver) {							// �Q�[���I�[�o�[�t���O��true
		soundManager->BgmStop();
		gameOverCount--;						// �Q�[���I�[�o�[�t���[���J�E���g���f�N�������g
		if (gameOverCount == 0) {				// �Q�[���I�[�o�[�t���[���J�E���g��0��������
			isGameOver = false;					// ������
			gameOverCount = GAME_OVER_FRAME;	// ������
			TitleInit();						// �^�C�g���֖߂�
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

	// �`����UI��ʂɂ��ăN���A
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI�`��


	return 0;
}

// ���C���Q�[���`��֐�
int GameManager::MainGameDraw() {

	// �`��n
	backGround->Draw();						// �w�i
	if (!isRestart) {
		enemyManager->GroundDraw();			// �n�㕨
		bulletManager->LandingPointDraw();	// ���e�J�[�\��
		bulletManager->BlasterDraw();		// �u���X�^�[
		bulletManager->CursorDraw();		// �J�[�\��
		player->Draw();						// �v���C���[
		bulletManager->ZapperDraw();		// �U�b�p�[
		enemyManager->SparioDraw();			// �G�e
		enemyManager->AerialDraw();			// �󒆓G
	}

	// �`����UI��ʂɂ��ăN���A
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI

	return 0;
}

// �l�[���G���g���[�`��֐�
int GameManager::NameEntryDraw() {
	backGround->Draw();							// �w�i

	// �`����UI��ʂɂ��ăN���A
	SetDrawScreen(uiScreen);
	ClearDrawScreen();
	ui->Draw(mode, frameCount, isGameOver);		// UI

	return 0;
}