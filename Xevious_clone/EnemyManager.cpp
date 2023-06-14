#include "EnemyManager.h"

// �R���X�g���N�^
EnemyManager::EnemyManager(ScoreManager* pScoreManagerP, AreaManager* pAreaManagerP, SoundManager* pSoundManager, UI* pUI) {
	int i;
	char imageName[IMAGE_NAME_BUF];					// �t�@�C�����ϐ�
	char bigImageName[IMAGE_NAME_BUF];				// �t�@�C�����ϐ�
	memset(imageName, 0x00, sizeof(imageName));		// ������
	memset(bigImageName, 0x00, sizeof(imageName));	// ������

	memset(m_AppAerialPattern, 0, sizeof(m_AppAerialPattern));	// �󒆓G�o���p�^�[��������
	memset(&m_enemyHit, false, sizeof(ENEMY_HIT));				// �q�b�g��񏉊���
	m_pScoreManager = pScoreManagerP;							// �X�R�A�}�l�[�W���|�C���^
	m_pAreaManager = pAreaManagerP;								// �G���A�}�l�[�W���|�C���^
	m_pSoundManager = pSoundManager;							// �T�E���h�}�l�[�W���|�C���^
	m_pUI = pUI;												// UI�|�C���^

	memset(m_enemyData, 0x00, sizeof(m_enemyData));			// �G���A�z�u��񏉊���
	memset(lineProcessData, 0x00, sizeof(lineProcessData));	// ���C���������߃f�[�^������

	// �摜�ǂݍ���
	for (i = 0; i < ENEMY_COLOR_NUM; i++) {	// �J���[�p�^�[�����ʉ摜�̕����摜�z��Ɋ��蓖�Ă郋�[�v
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
			LITTLE_SIZE, LITTLE_SIZE, m_imageHandle.little[i], FALSE);	// ���^�G�摜	�S�̂�80�����@��8�����@�c10����

		LoadDivGraph(bigImageName, BIG_IMG_DIVSION_NUM,
			BIG_IMG_DIVSION_X, BIG_IMG_DIVSION_Y,
			BIG_SIZE, BIG_SIZE, m_imageHandle.big[i], FALSE);			// ��^�G�摜
	}
	LoadDivGraph("img/Xevious_OBJ_enemyBroken.png", BROKEN_IMG_DIVSION_NUM,
		BROKEN_IMG_DIVSION_X, BROKEN_IMG_DIVSION_Y,
		BIG_SIZE, BIG_SIZE, m_imageHandle.broken, FALSE);	// �j��摜	�S�̂�14�����@��7�����@�c2����

	LoadDivGraph("img/Xevious_OBJ_bullet1.png", SPARIO_IMG_DIVSION_NUM,
		SPARIO_IMG_DIVSION_X, SPARIO_IMG_DIVSION_Y,
		LITTLE_SIZE, LITTLE_SIZE, m_imageHandle.spario, FALSE);	// �e�摜

	LoadDivGraph("img/Xevious_OBJ_bullet2.png", ZAKATO_IMG_DIVSION_NUM,
		ZAKATO_IMG_DIVSION_X, ZAKATO_IMG_DIVSION_Y,
		BIG_SIZE, BIG_SIZE, m_imageHandle.zakato, FALSE);	// �U�J�[�g�n�o���摜	�S�̂�30�����@��5�����@�c6����

	m_FrameCount = 0;			// �o�߃t���[��
	m_IsIncrement = false;		// �G�_�ŗp�t���O�ϐ�
	m_EnemyColor = 0;			// ���݂̓G�J���[
	m_CraterColor = 0;			// ���݂̃N���[�^�[�J���[
	m_BulletColor = 0;			// ���݂̒e�J���[
	m_GiddosparioColor = 0;		// ���݂̃M�h�X�p���I�̃J���[
	m_difficulty = 1;			// ��Փx�@��ň�������ݒ�ɕύX�H�@1:�W��

	// X���W�p�����_��������
	std::random_device rd;
	std::default_random_engine tmpRandSeed(rd());
	m_Seed = tmpRandSeed;
	std::uniform_int_distribution<int> tmpRand(CARACTER_X_MIN, CARACTER_X_MAX);
	m_RandEnemyX = tmpRand;

	// �_�~�[�I�u�W�F�N�g����
	for (i = 0; i < GROUND_NUM; i++) {
		m_GroundEnemies[i] = new Enemy();
	}
	for (i = 0; i < AERIAL_NUM; i++) {
		m_AerialEnemies[i] = new Enemy();
	}
	for (i = 0; i < SPARIO_NUM; i++) {
		m_spario[i] = new Enemy();
	}

	Init();			// ���X�^�[�g���Ɠ�������
}

// �f�X�g���N�^
EnemyManager::~EnemyManager() {
	int i;

	// �I�u�W�F�N�g�̃����������
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
	
	LoadAreaData();											// �G���A�z�u�������[�h
	GroundAreaDataSet();									// �f�[�^�����ɒn�㕨�����蓖��
	LoadLineData();											// ���C�������������[�h

	// �I�u�W�F�N�g��S�Ĕ񑶍݁A�H�炢���薳����
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

	// �B�����b�Z�[�W�I�u�W�F�N�g����
	GroundGenerater(TEMP_OFFSCREEN_WIDTH - 4, 0, static_cast<int>(EnemyId::HIDDEN_MESSAGE), 0, 0, 0, NULL);

	m_shotIntervalMax = 255;	// �n��G�̎ˌ��Ԋu�ő�l
	m_sortie = 0;				// �o����

	m_IsAerialGenerate = false;							// �G�����t���O
	m_AerialLevel = 0;									// �󒆓G���x��
	m_sortie++;											// �o���񐔂����Z
}

void EnemyManager::Restart() {
	int i;

	LoadAreaData();											// �G���A�z�u�������[�h
	GroundAreaDataSet();									// �f�[�^�����ɒn�㕨�����蓖��
	LoadLineData();											// ���C�������������[�h

	// �I�u�W�F�N�g��S�Ĕ񑶍݁A�H�炢���薳����
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

	// �B�����b�Z�[�W�I�u�W�F�N�g����
	if (m_pAreaManager->GetAria() == 0) {
		GroundGenerater(TEMP_OFFSCREEN_WIDTH - 4, 0, static_cast<int>(EnemyId::HIDDEN_MESSAGE), 0, 0, 0, NULL);
	}

	m_shotIntervalMax = 255;	// �n��G�̎ˌ��Ԋu�ő�l
	m_sortie = 0;				// �o����

	m_IsAerialGenerate = false;							// �G������~
	m_AerialLevel -= LEVEL_SUBTRAC_VALUE[m_difficulty];	// ��Փx�ɉ����ă��x���_�E��
	if (m_AerialLevel < 0) {							// 0������������0�ɕ␳
		m_AerialLevel = 0;
	}
	m_sortie++;											// �o���񐔂����Z
}

// Set�֐�
void EnemyManager::SetIsAerialGenerate() {	// �󒆓G�o���t���OON�֐�
	m_IsAerialGenerate = true;
}

void EnemyManager::SetEnemyHit(ENEMY_HIT enemyHit) {
	m_enemyHit = enemyHit;
}

// �G���A�z�u�f�[�^�����[�h���܂��@���A����͉��ɃG���A1�̃f�[�^���x�^�������Ă��邾��
int EnemyManager::LoadAreaData() {
	memset(m_enemyData, 0x00, sizeof(m_enemyData));			// �G�z�u��񏉊���

	m_enemyData[0] = { 2,16,44 };							// ���̃G���A1�n��G���@�{���̓��[�h����
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

// ���C�������f�[�^�����[�h���܂��@���A����͉��ɃG���A1�̃f�[�^���x�^�������Ă��邾��
int EnemyManager::LoadLineData() {
	memset(lineProcessData, 0x00, sizeof(lineProcessData));	// ���C���������߃f�[�^������

	lineProcessData[0][0][0] = SHOT_INTERVAL_MAX_CHANGE;	// ���̃G���A1���C�������f�[�^�@�{���̓��[�h����
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
* ���O�FGroundAreaDataSet
*
* �@�\�Fm_GroundAreaData�����������Am_enemyData�����ɒn�㕨���e�[�u���Ɋ��蓖�Ă܂�
*
* �����F�Ȃ�
*
* �߂�l�F�z�u�����n�㕨�̐�
*/
int EnemyManager::GroundAreaDataSet() {
	memset(m_GroundAreaData, 0x00, sizeof(m_GroundAreaData));	// �n��G�G���A��񏉊���
	int i;

	for (i = 0; i < AREA_DATA_GROUND_MAX; i++) {
		if (m_enemyData[i].tableID == 0) {	// ID��0�̃f�[�^���o�Ă����炻���ŏI��
			break;
		}
		m_GroundAreaData[m_enemyData[i].areaY][m_enemyData[i].areaX] = m_enemyData[i].tableID;	// ���蓖��
	}

	return i;
}

/*
* ���O�FAerialPatternSet
* 
* �@�\�F�����Ŏw�肳�ꂽ�l��ID�̓G�p�^�[�����Z�b�g���܂�
* 
* �����F�G�p�^�[��ID
* 
* �߂�l�F���������G�̐�
*/
int EnemyManager::AerialPatternSet(int level) {

	memset(m_AppAerialPattern, 0x00, sizeof(m_AppAerialPattern));			// ������
	memset(m_AppAerialTypePattern, 0x00, sizeof(m_AppAerialTypePattern));	// ������

	int patternID = 0;

	for (int j = 0; j < AERIAL_TABLE_PATTERN_NUM; j++) {								// �e�[�u����ɂ����ނ̓G
		for (int i = 0; i < AERIAL_ENEMY_TABLE[level][j].num; i++) {					// ���̓G�̔z�u�������[�v 
			m_AppAerialPattern[patternID] = AERIAL_ENEMY_TABLE[level][j].characterID;	// ID���Z�b�g
			m_AppAerialTypePattern[patternID] = AERIAL_ENEMY_TABLE[level][j].type;		// �^�C�v���Z�b�g
			patternID++;																// �z�u�ꏊ���C���N�������g
		}
	} 

	return patternID;
}

/*
* ���O�FAerialPatternSet
* 
* �@�\�F�󒆓G�̃��x�����Փx�ɉ����ďグ�A�����p�^�[�����Z�b�g���A�󒆓G�o���t���O��true�ɂ��܂�
* 
* �����F�Ȃ�
* 
* �߂�l�F���������G�̐�
*/
int EnemyManager::AerialGenerateStart() {
	int ret;
	m_AerialLevel += LEVEL_ADD_VALUE[m_difficulty];	// ��Փx�ˑ��Ń��x�����Œ�l���Z

	if (m_AerialLevel >= 46) {	// �{����128�ȏ�̎���64���Z���鏈���@����͓G�s���̂��ߓK���ɖ���Ȓl
		m_AerialLevel -= 32;
	}

	ret = AerialPatternSet(m_AerialLevel);	// �p�^�[�����Z�b�g
	
	m_IsAerialGenerate = true;				// �󒆓G�o���J�n
	return ret;								// ���������G�̐�
}

/*
* ���O�FLevelUpByScore
* 
* �@�\�F�X�R�A�ɉ����ċ󒆓G�o�����x�������Z���܂�
* 
* �����F�Ȃ�
* 
* �߂�l�F���x���ɉ��Z�����l
*/

int EnemyManager::LevelUpByScore() {
	int n = (m_pScoreManager->GetScore() / 100);					// ���݃X�R�A��100�Ŋ���
	n = n + 0x6 * (n / 10) + 0x60 * (n / 100) + 0x600 * (n / 1000);	// ���̒l��BCD�ɕϊ�

	if (m_sortie != 0) {	// �O�̂���0���Z���
		n /= m_sortie;		// �o�����Ŋ���
	}

	if (n > 16) {			// �ő�16
		n = 16;
	}

	m_AerialLevel += n;		// ���x���ɉ��Z

	return n;
}

// �S�̂Ɋւ�镨�̌v�Z
void EnemyManager::Calc(int frameCount) {

	// �e�J���[�I��
	if (frameCount % 2 == 0) {	// 2�t���[����
		m_BulletColor++;
		m_GiddosparioColor++;

		if (m_BulletColor == 4) {
			m_BulletColor = 0;
		}

		if (m_GiddosparioColor == 16) {
			m_GiddosparioColor = 0;
		}
	}

	// �n��N���[�^�[�J���[�I��
	if (frameCount % 4 == 0) {
		m_CraterColor++;

		if (m_CraterColor == 2) {
			m_CraterColor = 0;
		}
	}

	// �G�_�ŃJ���[�I��
	if (frameCount % 8 == 0) {	// 8�t���[����
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

// ��Ńo�L�����ɂ��Ή��\��
/******************************************************************
�֐����FRandomX

�@�\�F�G�o��X���W�������_���Ɍ��肷��֐��ł�

�����F	bool�^	isDistance	(i)	false	: �S�̂��烉���_��
								true	: �v���C���[����8�L�����N�^�ȏ㗣�ꂽ�ʒu�Ń����_��

�ԋp�l�F X���W

���ӎ����F�L�����N�^�P�ʂ̕\����ʓ���X���W1�`25����I������A���m��X���W�ɕϊ����ĕԂ��܂�
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

// �n�㕨����
int EnemyManager::GroundGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, Enemy* pParentObject) {
	if (characterId == static_cast<int>(EnemyId::SPFLAG)) {		// SP�t���b�O�͐�p�g
		delete m_GroundEnemies[0];								// ���݂̃I�u�W�F�N�g���폜
		m_GroundEnemies[0] = new SPFlag(RandomX(playerX, true), y, &m_imageHandle, m_pScoreManager);
		return 0;
	}

	if (characterId == static_cast<int>(EnemyId::HIDDEN_MESSAGE)) {		// SP�t���b�O�͐�p�g
		delete m_GroundEnemies[0];										// ���݂̃I�u�W�F�N�g���폜
		m_GroundEnemies[0] = new HiddenMessage(x, y, m_pScoreManager, m_pUI);
		return 0;
	}

	int i = -1;	// i����U-1�ŏ��������鎖�ŁA�Ȃɂ����Ȃ������ꍇ��-1��Ԃ�
	if (characterId != 0) {	// 0�͂Ȃɂ����Ȃ��@�_�~�[��ID�ƈ�v���Ă���̂ŁA�_�~�[�𐶐��������ꍇ�͒��ڍs����
		for (i = 2; i < GROUND_NUM; i++) {						// �I�u�W�F�N�g�̏�Ԃ��`�F�b�N�@�ŏ��̓�̓J�[�\�����������Ȃ����iSP�t���b�O���j��p�g
			if (m_GroundEnemies[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT)) {	// �ΏۃI�u�W�F�N�g�g���񑶍�
				delete m_GroundEnemies[i];							// ���݂̃I�u�W�F�N�g���폜
				switch (characterId)						// �����ŃL�����N�^ID�ŕ��򂵂Đ���
				{
				case static_cast<int>(EnemyId::BARRA):		// �o�[��
					m_GroundEnemies[i] = new Barra(x, y, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::ZOLBAK):		// �]���o�N				
					m_GroundEnemies[i] = new Zolbak(x, y, &m_imageHandle, m_pScoreManager, &m_AerialLevel);
					break;

				case static_cast<int>(EnemyId::GROBDA):	// �O���u�_�[A
					m_GroundEnemies[i] = new Grobda(x, y, type, brokenType, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::LOGRAM):		// ���O����
					m_GroundEnemies[i] = new Logram(x, y, m_shotIntervalMax, &m_imageHandle, m_pScoreManager, pParentObject);
					break;

				case static_cast<int>(EnemyId::BOZA_LOGRAM):// �{�U���O����
					BozaLogramGenerater(x, y, characterId, type, brokenType, playerX, i);	// ��p�����@�֐��ɐ؂�o���Ă���
					break;

				case static_cast<int>(EnemyId::DEROTA):		// �f���[�^
					m_GroundEnemies[i] = new Derota(x, y, m_shotIntervalMax, &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::SOL):		// �\��
					m_GroundEnemies[i] = new Sol(x, y, &m_imageHandle, m_pScoreManager);
					break;

				default:									// �ǂ�ɂ���v���Ȃ�������A�|�C���^����ɂȂ��Ă���󋵂�������邽�߃_�~�[��z�u
					m_GroundEnemies[i] = new Enemy();
					break;
				}
				break;
			}
		}
	}

	return i != GROUND_NUM ? i : -1;	// �����Ȃ琶�����ꂽID�A���s�Ȃ�-1��Ԃ�
}

// �{�U���O���������@�R�����g�����Ȃ���c
int EnemyManager::BozaLogramGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, int i) {
	if (i >= 0 && i < GROUND_NUM) {									// i���s���Ȓl�łȂ����`�F�b�N
		int groundID[BozaLogram::BOZA_LOGRAM_NUM] = { 0x00 };		// �{�̂ƃ��O������ID
		Enemy** tempPP[BozaLogram::BOZA_LOGRAM_NUM] = { NULL };		// �i�[��������_�u���|�C���^

		m_GroundEnemies[i] = new Enemy();									// �_�~�[��������
		m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::ACTIVE));	// ���u���̃_�~�[���A�N�e�B�u�ɂ��A�����ʒu�ɐ����s�ɂ���

		// ���������ꏊ��ID��z��ɓ���Ă����@�ŏ��ɖ{�́A�Ō�ɏ�̃��O�����ł��邱��
		groundID[0] = i;
		groundID[1] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET, y + BozaLogram::BOZA_LOGRAM_OFFSET * 2, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);	// ���O������
		groundID[2] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET * 2, y + BozaLogram::BOZA_LOGRAM_OFFSET, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);	// ���O������
		groundID[3] = GroundGenerater(x, y + BozaLogram::BOZA_LOGRAM_OFFSET, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);											// ���O�����E
		groundID[4] = GroundGenerater(x + BozaLogram::BOZA_LOGRAM_OFFSET, y, static_cast<int>(EnemyId::LOGRAM), 0, 0, playerX, NULL);											// ���O������

		for (int j = 0; j < BozaLogram::BOZA_LOGRAM_NUM; j++) {		// ID���`�F�b�N���A�����ł��Ă���ꍇ�͐�����̃|�C���^���_�u���|�C���^�z��ɓ����
			if (groundID[j] != -1) {
				tempPP[j] = &m_GroundEnemies[groundID[j]];
			}
		}

		delete m_GroundEnemies[i];									// ���u�����Ă����_�~�[�I�u�W�F�N�g���폜
		m_GroundEnemies[i] = new BozaLogram(x + BozaLogram::BOZA_LOGRAM_OFFSET, y + BozaLogram::BOZA_LOGRAM_OFFSET, &m_imageHandle, m_pScoreManager, tempPP);	// �{�̂𐶐�

		for (int j = 1; j < BozaLogram::BOZA_LOGRAM_NUM; j++) {
			if (groundID[j] != -1) {
				m_GroundEnemies[groundID[j]]->SetPParentObject(m_GroundEnemies[i]);	// �q���O�����Ƀ{�U���O�����̃|�C���^���Z�b�g
			}
		}

		return 0;
	}
	return -1;

}

// �󒆓G����
void EnemyManager::AerialGenerater(float playerX, float playerY) {

	if (m_IsAerialGenerate) {

		for (int i = 0; i < AERIAL_NUM; i++) {						// �I�u�W�F�N�g�̏�Ԃ��`�F�b�N
			// �ΏۃI�u�W�F�N�g�g���񑶍݂ŁA�����Ώۂ��_�~�[����Ȃ��A�܂��͐����Ώۂ��_�~�[�������悪�_�~�[����Ȃ�
			if (m_AerialEnemies[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT) &&

				(m_AppAerialPattern[i] != static_cast<int>(EnemyId::DUMMY) ||
					m_AppAerialPattern[i] == static_cast<int>(EnemyId::DUMMY) && m_AerialEnemies[i]->GetCharcterId() != static_cast<int>(EnemyId::DUMMY))) {

				delete m_AerialEnemies[i];					// ���݂̃I�u�W�F�N�g���폜
				switch (m_AppAerialPattern[i])				// �����ŃL�����N�^ID�ŕ��򂵂Đ���
				{
				case static_cast<int>(EnemyId::TOROID):		// �g�[���C�h
					m_AerialEnemies[i] = new Toroid(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::TORKAN):		// �^���P��
					m_AerialEnemies[i] = new Torkan(RandomX(playerX, false), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::ZOSHI):		// �]�V�[
					if (m_AppAerialTypePattern[i] == 2) {	// �o�b�N�]�V�[��������
						m_AerialEnemies[i] = new Zoshi(RandomX(playerX, true), TEMP_SCREEN_HEIGHT, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);

					}
					else {									// ����ȊO
						m_AerialEnemies[i] = new Zoshi(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					}
					break;

				case static_cast<int>(EnemyId::JARA):		// �W�A��
					m_AerialEnemies[i] = new Jara(RandomX(playerX, true), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				case static_cast<int>(EnemyId::GIDDO_SPARIO):	// �M�h�X�p���I
					m_AerialEnemies[i] = new Giddospario(RandomX(playerX, false), 0, playerX, playerY, m_AppAerialTypePattern[i], &m_imageHandle, m_pScoreManager);
					break;

				default:	// �ǂ�ɂ���v���Ȃ�������ꉞ�_�~�[
					m_AerialEnemies[i] = new Enemy();
					break;
				}
			}
		}
	}
}

// �ʏ�e����
void EnemyManager::BulletGenerater(float x, float y, float playerX, float playerY) {
	for (int i = 0; i < SPARIO_NUM; i++) {												// �I�u�W�F�N�g�̏�Ԃ��`�F�b�N
		if (m_spario[i]->GetStatus() == static_cast<int>(StatusId::NON_EXISTENT)) {		// �ΏۃI�u�W�F�N�g�g���񑶍�
			delete m_spario[i];															// ���݂̃I�u�W�F�N�g���폜
			m_spario[i] = new Spario(x, y, playerX, playerY, 0, &m_imageHandle);		// �^�C�v�ݒ�͉�
			break;
		}
	}
}

/*
* ���O�FAreaDataCheck
*
* �@�\�F�z�u�����`�F�b�N���A�G�𐶐��֘A�������s���܂�
*
* �����F(i) int playerX
*
* �߂�l�F����0�̂�
*/
int EnemyManager::AreaDataCheck(float playerX) {
	int line = m_pAreaManager->GetLine();			// ���݂̃��C��
	int y = m_pAreaManager->GetGroundEnemyLine();	// ���݂̒n��G���C��
	int id = 0;										// ��������G�e�[�u��ID
	GROUND_TABLE tempT;								// �Z���\�L���邽�߂̉��G�f�[�^

	if (!m_pAreaManager->GetIsStart()) {			// �J�n���łȂ���

		if (line == 0) {
			GroundAreaDataSet();	// �f�[�^�����ɒn�㕨�����蓖��
			LoadLineData();			// 

		}

		for (int i = 0; i < LINE_PROCESS_NUM; i++) {			// ���C������
			switch (lineProcessData[line][i][0])
			{
			case APP_STOP:										// �󒆓G������~
				m_IsAerialGenerate = false;						// �����t���O��false��
				break;

			case APP_START_LEVEL:								// ���x���ɉ������󒆓G�����J�n
				AerialGenerateStart();							// �����J�n
				break;

			case APP_START_PATTERN:								// �Œ�p�^�[���ł̋󒆓G�����J�n
				AerialPatternSet(lineProcessData[line][0][1]);	// �p�^�[�����Z�b�g
				m_IsAerialGenerate = true;						// �����t���O��true��
				break;

			case LEVEL_SCORE_UPDATE:							// �X�R�A�ˑ��Ń��x�����Z
				LevelUpByScore();
				break;

			case APP_ALONE_ENEMY:								// �G�P�̐���
				// ������
				break;

			case SHOT_INTERVAL_MAX_CHANGE:						// �n��G�ˌ��Ԋu�ύX
				m_shotIntervalMax = lineProcessData[line][0][1];
				break;

			default:
				break;
			}
			lineProcessData[line][i][0] = 0;

		}
		
		for (int i = AreaManager::AREA_CHARACTER_WIDTH - 1; i >= 0; i--) {
			id = m_GroundAreaData[y][i];		// �GID���擾
			if (id != 0) {						// ID��0�łȂ���
				tempT = GROUND_ENEMY_TABLE[id];	// ����ID�̓G�����擾
				GroundGenerater(static_cast<float>(i * CHARACTER_SIZE) + tempT.offsetX, tempT.offsetY,
					tempT.characterID, tempT.type, tempT.brokenType, playerX, NULL);	// �Ώ�ID�̏��Ő���
				m_GroundAreaData[y][i] = 0;		// ���������炻����ID�����
				return 0;						// 1�t���[���Ɉ�񂵂��������Ȃ�
			}
		}
	}

	return 0;
}

// �n�㕨�v�Z
void EnemyManager::GroundCalc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove) {
	float tempX = 0;	// �N���[�^�[�����p���W�ێ��ϐ�
	float tempY = 0;

	for (int i = 0; i < GROUND_NUM; i++) {

		// ��{����
		if (m_GroundEnemies[i]->GetStatus() != static_cast<int>(StatusId::DESTROYED)) {
			m_GroundEnemies[i]->Calc(playerX, playerY, cursorX, cursorY, landingPointX, landingPointY, groundMove, m_enemyHit.ground[i]);	// ��{�̌v�Z

			if (m_GroundEnemies[i]->GetIsShot()) {	// �V���b�g�t���O�������Ă�����e�𐶐�����false��
				m_GroundEnemies[i]->SetIsShotFalse();
				BulletGenerater(m_GroundEnemies[i]->GetX(), m_GroundEnemies[i]->GetY(), playerX, playerY);
			}

		}
		else {
			tempX = m_GroundEnemies[i]->GetX();					// ���W��ێ�
			tempY = m_GroundEnemies[i]->GetY();
			delete m_GroundEnemies[i];							// ���݂̃I�u�W�F�N�g���폜
			m_GroundEnemies[i] = new Crater(tempX, tempY, &m_imageHandle);	// �N���[�^�[����
		}

		// ��ȊO�̉�ʊO�ŏ���
		if (OffScreenChecker2(m_GroundEnemies[i]->GetX(), m_GroundEnemies[i]->GetY())) {
			m_GroundEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// �󒆓G�v�Z
void EnemyManager::AerialCalc(float playerX, float playerY, float groundMove) {
	for (int i = 0; i < AERIAL_NUM; i++) {

		m_AerialEnemies[i]->Calc(playerX, playerY, groundMove, m_enemyHit.aerial[i]);

		if (m_AerialEnemies[i]->GetIsShot()) {	// �V���b�g�t���O�������Ă�����e����
			m_AerialEnemies[i]->SetIsShotFalse();	// �V���b�g�t���O��false��
			BulletGenerater(m_AerialEnemies[i]->GetX(), m_AerialEnemies[i]->GetY(), playerX, playerY);	// �e����
		}

		// ��ʊO�ŏ���
		if (OffScreenChecker(m_AerialEnemies[i]->GetX(), m_AerialEnemies[i]->GetY())) {
			m_AerialEnemies[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// �e�v�Z
void EnemyManager::SparioCalc() {
	for (int i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->Calc();

		// ��ʊO�ŏ���
		if (OffScreenChecker(m_spario[i]->GetX(), m_spario[i]->GetY())) {
			m_spario[i]->SetStatus(static_cast<int>(StatusId::NON_EXISTENT));
		}
	}
}

// �n�㕨�`��
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

// �e�`��
void EnemyManager::SparioDraw() {
	for (int i = 0; i < SPARIO_NUM; i++) {
		m_spario[i]->Draw(m_BulletColor);
	}
}

// �󒆓G�`��
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

// ��ʊO�`�F�b�N�@������x, y���W����ʊO��������true ��ʓ���������false��Ԃ��܂�
// �󒆓G�p�@�n��G��OffScreenChecker2���g�p���Ă�������
bool EnemyManager::OffScreenChecker(float x, float y) {
	if (y > TEMP_SCREEN_HEIGHT || x > TEMP_SCREEN_WIDTH ||
		x <= 0 || y < 0) {
		return true;
	}
	return false;
}

// ��ʊO�`�F�b�N2�@��ʏ�����������O�����ŉ�ʊO��������true ��ʓ���������false��Ԃ��܂�
// �n��G�p�@�󒆓G��OffScreenChecker���g�p���Ă�������
bool EnemyManager::OffScreenChecker2(float x, float y) {
	if (y > TEMP_SCREEN_HEIGHT || x > TEMP_SCREEN_WIDTH ||
		x <= 0) {
		return true;
	}
	return false;
}

/******************************************************************
�֐����FGetAerialX

�@�\�F�����Ŏw�肵��ID�̋󒆓G��X���W��Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	�����FX���W
	���s�F-1
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�G���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetAerialX(int aerialEnemiesId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (aerialEnemiesId < AERIAL_NUM && m_AerialEnemies[aerialEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_AerialEnemies[aerialEnemiesId]->GetX();
	}
	return -1;
}

/******************************************************************
�֐����FGetAerialY

�@�\�F�����Ŏw�肵��ID�̋󒆓G��Y���W��Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	�����FY���W
	���s�F-1
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�G���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetAerialY(int aerialEnemiesId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (aerialEnemiesId < AERIAL_NUM && m_AerialEnemies[aerialEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_AerialEnemies[aerialEnemiesId]->GetY();
	}
	return -1;
}

/******************************************************************
�֐����FGetAerialStatus

�@�\�F�����Ŏw�肵��ID�̋󒆓G�̏�Ԃ�Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	int�^
		�����F���
		���s�F-1
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
int EnemyManager::GetAerialStatus(int aerialEnemiesId) {
	// ������������ID�̏ꍇ
	if (aerialEnemiesId < AERIAL_NUM) {
		return m_AerialEnemies[aerialEnemiesId]->GetStatus();
	}
	return -1;
}

/******************************************************************
�֐����FGetAerialStatus

�@�\�F�����Ŏw�肵��ID�̋󒆓G�̏�Ԃ�Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	int�^
		�����F���
		���s�F-1
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
int EnemyManager::GetBulletStatus(int bulletId) {
	// ������������ID�̏ꍇ
	if (bulletId < SPARIO_NUM) {
		return m_spario[bulletId]->GetStatus();
	}
	return -1;
}

/******************************************************************
�֐����FGetAerialIsCollision

�@�\�F�����Ŏw�肵��ID�̋󒆓G�̐ڐG����̗L����Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	bool�^
		�����F���肠��Ftrue�@���薳���Ffalse
		���s�Ffalse
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇfalse��Ԃ��܂�
*********************************************************************/
bool EnemyManager::GetAerialIsCollision(int aerialEnemiesId) {
	if (aerialEnemiesId < AERIAL_NUM) {
		return m_AerialEnemies[aerialEnemiesId]->GetIsCollision();
	}
	return false;
}

/******************************************************************
�֐����FGetGroundCharcterId

�@�\�F�����Ŏw�肵��ID�̒n��G�̃L�����N�^ID��Ԃ��܂�

�����F	int�^	groundEnemiesId	(i)	�n��G��ID

�ԋp�l�F
	�����F�L�����N�^ID
	���s�F-1
���ӎ����F
	�P�FgroundEnemiesId�ɂ�GROUND_NUM�����̒l���g�p���Ă�������
	�Q�F�G���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
int EnemyManager::GetGroundCharcterId(int groundEnemiesId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetCharcterId();
	}
	return -1;
}

/******************************************************************
�֐����FGetGroundX

�@�\�F�����Ŏw�肵��ID�̒n��G��X���W��Ԃ��܂�

�����F	int�^	groundEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	�����FX���W
	���s�F-1
���ӎ����F
	�P�FgroundEnemiesId�ɂ�GROUND_NUM�����̒l���g�p���Ă�������
	�Q�F�G���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetGroundX(int groundEnemiesId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetX();
	}
	return -1;
}

/******************************************************************
�֐����FGetGroundY

�@�\�F�����Ŏw�肵��ID�̒n��G��Y���W��Ԃ��܂�

�����F	int�^	groundEnemiesId	(i)	�n��G��ID

�ԋp�l�F
	�����FY���W
	���s�F-1
���ӎ����F
	�P�FgroundEnemiesId�ɂ�GROUND_NUM�����̒l���g�p���Ă�������
	�Q�F�G���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetGroundY(int groundEnemiesId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (groundEnemiesId < GROUND_NUM && m_GroundEnemies[groundEnemiesId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_GroundEnemies[groundEnemiesId]->GetY();
	}
	return -1;
}

/******************************************************************
�֐����FGetGroundStatus

�@�\�F�����Ŏw�肵��ID�̒n��G�̏�Ԃ�Ԃ��܂�

�����F	int�^	aerialEnemiesId	(i)	�n��G��ID

�ԋp�l�F
	int�^
		�����F���
		���s�F-1
���ӎ����F
	�P�FaerialEnemiesId�ɂ�AERIAL_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
int EnemyManager::GetGroundStatus(int groundEnemiesId) {
	// ������������ID�̏ꍇ
	if (groundEnemiesId < GROUND_NUM) {
		return m_GroundEnemies[groundEnemiesId]->GetStatus();
	}
	return -1;
}

/******************************************************************
�֐����FGetGroundIsCollision

�@�\�F�����Ŏw�肵��ID�̒n��G�̐ڐG����̗L����Ԃ��܂�

�����F	int�^	groundEnemiesId	(i)	�󒆓G��ID

�ԋp�l�F
	bool�^
		�����F���肠��Ftrue�@���薳���Ffalse
		���s�Ffalse
���ӎ����F
	�P�FgroundEnemiesId�ɂ�GROUND_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇfalse��Ԃ��܂�
*********************************************************************/
bool EnemyManager::GetGroundIsCollision(int groundEnemiesId) {
	if (groundEnemiesId < GROUND_NUM) {
		return m_GroundEnemies[groundEnemiesId]->GetIsCollision();
	}
	return false;
}

/******************************************************************
�֐����FGetBulletX

�@�\�F�����Ŏw�肵��ID�̓G�ʏ�e��X���W��Ԃ��܂�

�����F	int�^	sparioId	(i)	�e��ID

�ԋp�l�F
	�����FX���W
	���s�F-1
���ӎ����F
	�P�FsparioId�ɂ�SPARIO_NUM�����̒l���g�p���Ă�������
	�Q�F�e���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetBulletX(int bulletId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (bulletId < SPARIO_NUM && m_spario[bulletId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_spario[bulletId]->GetX();
	}
	return -1;
}

/******************************************************************
�֐����FGetBulletY

�@�\�F�����Ŏw�肵��ID�̓G�ʏ�e��X���W��Ԃ��܂�

�����F	int�^	sparioId	(i)	�e��ID

�ԋp�l�F
	�����FY���W
	���s�F-1
���ӎ����F
	�P�FsparioId�ɂ�SPARIO_NUM�����̒l���g�p���Ă�������
	�Q�F�e���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ-1��Ԃ��܂�
*********************************************************************/
float EnemyManager::GetBulletY(int bulletId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (bulletId < SPARIO_NUM && m_spario[bulletId]->GetStatus() == static_cast<int>(StatusId::ACTIVE)) {
		return m_spario[bulletId]->GetY();
	}
	return -1;
}

int EnemyManager::GetAerialLevel() {
	return m_AerialLevel;
}