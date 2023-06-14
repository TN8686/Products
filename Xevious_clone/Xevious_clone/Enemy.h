#pragma once

#include <math.h>
#include "Xevious_clone.h"
#include "EnemyManagerConst.h"
#include "ScoreManager.h"

using namespace EnemyManagerConst;

// �G�摜�n���h������Z�߂ň����\����
typedef struct {
	int little[ENEMY_COLOR_NUM][LITTLE_IMG_DIVSION_NUM];		// �G�摜�n���h���z��
	int big[ENEMY_COLOR_NUM][BIG_IMG_DIVSION_NUM];				// ��^�G�摜�n���h���z��
	int broken[BROKEN_IMG_DIVSION_NUM];							// �j��摜�n���h���z��
	int spario[SPARIO_IMG_DIVSION_NUM];							// �e�摜�n���h���z��
	int zakato[ZAKATO_IMG_DIVSION_NUM];							// �U�J�[�g�o���摜�n���h���z��
	int andorGenesisBody[ANDOR_GENESIS_BODY_IMG_DIVSION_NUM];	// �A���h�A�W�F�l�V�X�{�̉摜�n���h���z��
	int andorGenesisParts[ANDOR_GENESIS_PARTS_IMG_DIVSION_NUM];	// �A���h�A�W�F�l�V�X�p�[�c�摜�n���h���z��
} IMAGE_HANDLE_STRUCT;

// ��Ԓ萔	#define�ł͂Ȃ�enum�ň����悤�ɂ��܂���
enum class StatusId : int {
	NON_EXISTENT,
	ACTIVE,
	BROKEN,
	DESTROYED
};

// �j��^�C�vID
enum class BrokenTypeId : int {
	AERIAL,
	GROUND_A,
	GROUND_B
};

// �GID�@�ォ�珇��0����1�������Ă���@���g���ӎ�����K�v�͖���
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
	ZEPNITE,	// �E���@�������Ԃ�s��
	CASNITE,	// �����@�������Ԃ�s��
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
	int m_CharcterId;						// �G�̎��ID
	int m_Status;							// ��ԁ@0:�񑶍݁@1:��ʊO�ҋ@�@2:���쒆�@3:�j��A�j���[�V�������@4:�j��I��
	float m_X;								// X���W
	float m_Y;								// Y���W
	float m_SpeedX;							// X�ړ����x
	float m_SpeedY;							// Y�ړ����x
	float m_AccelerationX;					// X�����x
	float m_AccelerationY;					// Y�����x
	int m_Width;							// ��
	int m_Height;							// ����
	int m_Score;							// �X�R�A
	int m_ShotInterval;						// �e�𕡐����ꍇ�̎ˌ��Ԋu�@���ۂ̏����ł�8�{���Ĉ���
	bool m_IsShot;							// �V���b�g�t���O
	bool m_isCollision;						// �H�炢����t���O
	int m_Type;								// �����G�ŕ����s���p�^�[��������G�p
	int m_BrokenType;						// �j��A�j���[�V�����^�C�v�@0:�ʏ�
	IMAGE_HANDLE_STRUCT* m_imageHandleP;	// �摜�n���h���|�C���^
	ScoreManager* m_scoreManagerP;			// �X�R�A�}�l�[�W���|�C���^
	Enemy* m_pParentObject;					// �e�I�u�W�F�N�g�|�C���^

	double m_Angle;							// �p�x�v�Z
	bool m_IsFlip;							// ���]�t���O
	int m_BrokenImageAnimation;				// �j��A�j���[�V�������Z
	int m_FCount;							// �t���[���J�E���g

	static constexpr float M_SPEED_TABLE[4] = { 1.5, 2, 3, 4 };	// ���x�e�[�u��
	static constexpr int NOT_SHOT_LINE = TEMP_SCREEN_HEIGHT - 88;	// �ˌ��s���C��

	static constexpr float RAD_32 = 0.19634954f;	// 32�����̃��W�A���l
	static constexpr float RAD_4 = 1.57079632f;		// 4�����̃��W�A���l

	// �V���b�g�֐��@�ĂԂƂ��̃t���[�������e������	�i���ۂ̓t���O��true�ɂ��Ă邾���@false�ɂ���̂̓}�l�[�W��������ɊǗ��j
	void m_Shot();

public:
	Enemy();
	Enemy(int charcterId, float x, float y, int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP);	// �R���X�g���N�^�錾

	// Get�֐�
	int GetCharcterId();	// �L�����N�^ID
	int GetStatus();		// ���
	float GetX();			// X���W
	float GetY();			// Y���W
	int GetWidth();			// ��
	int GetHeight();		// ����
	bool GetIsShot();		// �V���b�g�t���O
	bool GetIsCollision();	// �����蔻��t���O
	int GetScore();			// �X�R�A

	// Set�֐�
	void SetStatus(int status);
	void SetIsCollision(bool isCollision);
	void SetPParentObject(Enemy* pParentObject);
	void SetIsShotFalse();

	float Angle(float x1, float y1, float x2, float y2);	// (x1, y1)���W���猩��(x2, y2)���W�́A�E��0�x�Ƃ��Ĕ����v����32�����̃��W�A���l��Ԃ��܂�
	void SetSpeedFromAngle(float angle, float speed);		// �p�x������X�AY�ړ����x��ݒ肷��֐�
	void AddScore();	// �X�R�A���Z�֐�
	void DamageFunction(bool isHit);	// ��{�̔�e�����֐�

	virtual void Calc();	// �X�p���I�v�Z�p
	virtual void Calc(float playerX, float playerY, float groundMove, bool isHit);	// �󒆓G�v�Z�p
	virtual void Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit);	// �n�㕨�v�Z�p
	virtual void Draw(int colorID);	// �`��

	void BrokenCalc(float groundMove);	// �j�󏈗�
	void BrokenDraw();	// �j�󏈗�

	void GroundBrokenCalc(float groundMove); //�n��G�j��A�j���[�V�����v�Z
	void GroundBrokenDraw(); //�n��G�j��A�j���[�V����
	void GroundBrokenCalc2(float groundMove); //�n��G�j��A�j���[�V�����v�Z����2
	void GroundBrokenDraw2(); //�n��G�j��A�j���[�V����2
	void AerialBrokenCalc(); //�j��A�j���[�V�����v�Z
	void AerialBrokenDraw(); //�j��A�j���[�V����


};