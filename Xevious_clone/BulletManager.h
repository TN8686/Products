#pragma once
#include "Zapper.h"
#include "Blaster.h"
#include "Cursor.h"
#include "LandingPoint.h"
#include "SoundManager.h"

class BulletManager
{
public:
	// �U�b�p�[�q�b�g���\����
	typedef struct {
		bool enemy[ZAPPER_NUM];		// �G�q�b�g���
		bool baqura[ZAPPER_NUM];	// �o�L�����q�b�g���
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

	static const int M_BULLET_SIZE = 16;			// �e�̃T�C�Y
	static const int M_CURSOR_SIZE = 16;			// �J�[�\���̃T�C�Y

	static const int M_BLASTER_FRAME = 38;			// �u���X�^�[���˃t���[��
private:
	static const int M_BULLET_IMG_DIVSION_NUM = 72;	// �e�摜������
	static const int M_BULLET_IMG_DIVSION_X = 12;	// �́AX������
	static const int M_BULLET_IMG_DIVSION_Y = 6;	// �ƁAY������
	static const int M_RAPID_FRAME = 20;			// �����A�˂̔��ˊԊu
	static const int M_BLASTER_WAIT = 2;			// �u���X�^�[���ˑҋ@�t���[��


	static const int M_CURSOR_IMG_DIVSION_NUM = 6;  // �J�[�\���摜������
	static const int M_CURSOR_IMG_DIVSION_X = 6;	// �́AX������
	static const int M_CURSOR_IMG_DIVSION_Y = 1;	// �ƁAY������

	int m_BulletImage[M_BULLET_IMG_DIVSION_NUM];	// �e�摜
	int m_CursorImage[M_CURSOR_IMG_DIVSION_NUM];	// �J�[�\���摜

	bool m_OnCursor;								// �J�[�\�����d�Ȃ��Ă��邩�ۂ�

	int m_ZapperAnimationId;						// �e�A�j���[�V����ID
	bool m_isZapperFlip;							// ���E���]�t���O
	int m_FrameCount;								// �t���[���J�E���^
	int m_BlasterFrameCount;						// �u���X�^�[���ˑҋ@�t���[���J�E���^
	bool m_IsBlasterLaunching;						// �u���X�^�[���˒��t���O

	ZAPPER_HIT m_zapperHit;							// �U�b�p�[�q�b�g���

	Zapper* m_zapper[ZAPPER_NUM];					// �U�b�p�[�|�C���^�z��
	Cursor* m_cursor;								// �J�[�\���|�C���^
	LandingPoint* m_landingPoint;					// ���e�J�[�\���|�C���^
	Blaster* m_blaster;								// �u���X�^�[�|�C���^
	SoundManager* m_soundManagerP;					// �T�E���h�}�l�[�W���|�C���^
};

