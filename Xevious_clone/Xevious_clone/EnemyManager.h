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

	// �n��G�e�[�u���\����
	typedef struct{
		int characterID;
		int type;
		int brokenType;
		float offsetX;
		float offsetY;
	} GROUND_TABLE;

	// �󒆓G�e�[�u���\����
	typedef struct {
		int characterID;
		int type;
		int num;
	} AERIAL_TABLE;

	// �n��G�z�u���\����
	typedef struct {
		int tableID;
		int areaX;
		int areaY;
	} ENEMY_MAP;

	// �G�q�b�g���\����
	typedef struct {
		bool aerial[AERIAL_NUM];
		bool ground[GROUND_NUM];
	}ENEMY_HIT;

	EnemyManager(ScoreManager* pScoreManager, AreaManager* pAreaManager, SoundManager* pSoundManager, UI* pUI);	// �R���X�g���N�^�錾
	~EnemyManager(); // �f�X�g���N�^�錾

	void Init();	// �������֘A
	void Restart();

	// ����
	float RandomX(float playerX, bool isDistance);
	int GroundGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, Enemy* pParentObject);		// �n��G����
	void AerialGenerater(float playerX, float playerY);	// �󒆓G����
	void BulletGenerater(float x, float y, float playerX, float playerY);	// �e����

	int AreaDataCheck(float playerX);

	// �v�Z
	void Calc(int frameCount);	// �S�̂Ɋւ�镨�̌v�Z
	void GroundCalc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove);	// �n�㕨�v�Z
	void AerialCalc(float playerX, float playerY, float groundMove);	// �󒆓G�v�Z
	void SparioCalc();	// �e�v�Z

	// �`��
	void GroundDraw();	// �n�㕨�`��
	void AerialDraw();	// �󒆓G�`��
	void SparioDraw();	// �e�`��

	// �Z�b�^�[
	void SetIsAerialGenerate();	// �󒆓G�o���t���O
	void SetEnemyHit(ENEMY_HIT enemyHit);

	// �Q�b�^�[
	float GetAerialX(int aerialEnemiesId);
	float GetAerialY(int aerialEnemiesId);
	int GetAerialStatus(int aerialEnemiesId);
	bool GetAerialIsCollision(int aerialEnemiesId);

	int GetGroundCharcterId(int groundEnemiesId);	// �L�����N�^ID
	float GetGroundX(int groundEnemiesId);
	float GetGroundY(int groundEnemiesId);
	int GetGroundStatus(int groundEnemiesId);
	bool GetGroundIsCollision(int groundEnemiesId);

	float GetBulletX(int bulletId);
	float GetBulletY(int bulletId);
	int GetBulletStatus(int bulletId);

	int GetAerialLevel();

private:
	int GroundAreaDataSet();				// �n��G����z�u
	int AerialPatternSet(int patternID);	// �󒆓G���z�u
	int AerialGenerateStart();				// ���x���Œ�l���Z�{�󒆓G���z�u�{�󒆓G�����J�n
	int LevelUpByScore();					// �X�R�A�ˑ��̓G���x�����Z
	int LoadLineData();						// ���C��������񃍁[�h
	int LoadAreaData();						// �G���A��񃍁[�h

	// �{�U���O������������
	int BozaLogramGenerater(float x, float y, int characterId, int type, int brokenType, float playerX, int i);

	// ��ʊO�`�F�b�J�[
	bool OffScreenChecker(float x, float y);
	bool OffScreenChecker2(float x, float y);


	static constexpr int LEVEL_ADD_VALUE[LEVEL_NUM] = { 0, 2, 6, 16 };		// ���x���Œ���Z�l
	static constexpr int LEVEL_SUBTRAC_VALUE[LEVEL_NUM] = { 24, 16, 8, 0 };	// ���x���Œ茸�Z�l

	// �e�[�u����萔�Ƃ��Đ錾	�ЂƂ܂��G���A1�ɕK�v�ȕ�����
	const GROUND_TABLE GROUND_ENEMY_TABLE[ENEMY_TABLE_NUM] = {
		static_cast<int>(EnemyId::DUMMY), 0, 0, 0, 0,												// 0 �ŏ��̓_�~�[��z�u���Ă���
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
		{static_cast<int>(EnemyId::JARA), 0, 5},	// �ЂƂ܂��G������Ă���45�܂�

	};

	// ���C�������萔
	static constexpr int APP_STOP					 = 1;
	static constexpr int APP_START_LEVEL			 = 2;
	static constexpr int APP_START_PATTERN			 = 3;
	static constexpr int LEVEL_SCORE_UPDATE			 = 4;
	static constexpr int APP_ALONE_ENEMY			 = 5;
	static constexpr int SHOT_INTERVAL_MAX_CHANGE	 = 6;

	// �ϐ��Q
	bool m_IsAerialGenerate;				// �󒆓G�o���t���O
	int m_AerialLevel;						// ���݂̋󒆓G���x��
	int m_AppAerialPattern[AERIAL_NUM];		// �o���G�z��
	int m_AppAerialTypePattern[AERIAL_NUM];	// �o���G�^�C�v�z��

	ENEMY_HIT m_enemyHit;					// �q�b�g���
	IMAGE_HANDLE_STRUCT m_imageHandle;		// �摜�n���h���z��

	int m_enemyDataNum = 0;					// ���[�h�̍ۂ̃G�l�~�[�f�[�^�̐�
	ENEMY_MAP m_enemyData[AREA_DATA_GROUND_MAX]; // �G�z�u���Z�b�g�p�\���� �{���͂��̍\���̔z����o�C�i���f�[�^�ɂ��ă��[�h���� �z�񐔂͉���64
	int m_GroundAreaData[AreaManager::LINE_MAX][AreaManager::AREA_CHARACTER_WIDTH];	// ���ۂɎg�p�����G�z�u�f�[�^�@�����̒l��ǂݎ��A����ID�̓G�e�[�u���̏��œG��z�u����

	int lineProcessData[AreaManager::LINE_MAX][LINE_PROCESS_NUM][LINE_PROCESS_NUM];	// ���C���������߃f�[�^

	int m_FrameCount;		// �o�߃t���[��
	bool m_IsIncrement;		// �G�J���[�̒l�𑝂₷�����炷��
	int m_EnemyColor;		// ���݂̓G�J���[
	int m_CraterColor;		// ���݂̃N���[�^�[�̃J���[
	int m_BulletColor;		// ���݂̒e�J���[
	int m_GiddosparioColor;	// ���݂̃M�h�X�p���I�̃J���[

	int m_shotIntervalMax;	// �n��G�̎ˌ��Ԋu�ő�l
	int m_difficulty;		// ��Փx
	int m_sortie;			// �\���o���E�o����

	ScoreManager* m_pScoreManager;		// �X�R�A�}�l�[�W���|�C���^
	AreaManager* m_pAreaManager;		// �G���A�}�l�[�W���|�C���^
	SoundManager* m_pSoundManager;		// �T�E���h�}�l�[�W���|�C���^
	UI* m_pUI;							// UI�|�C���^

	// �����֘A
	std::default_random_engine m_Seed;					// �V�[�h
	std::uniform_int_distribution<int> m_RandEnemyX;	// ���������I�u�W�F�N�g

	// �I�u�W�F�N�g�̃|�C���^�錾
	Enemy* m_GroundEnemies[GROUND_NUM];		// �n�㕨
	Enemy* m_Bacuras[BACURA_NUM];			// �o�L����
	Enemy* m_AerialEnemies[AERIAL_NUM];		// �󒆓G
	Enemy* m_spario[SPARIO_NUM];			// �ʏ�e�i�X�p���I�j

};