#include "BulletManager.h"

// �R���X�g���N�^
BulletManager::BulletManager(SoundManager* soundManagerP) {
	LoadDivGraph("img/Xevious_OBJ_bullet1.png", M_BULLET_IMG_DIVSION_NUM,
		M_BULLET_IMG_DIVSION_X, M_BULLET_IMG_DIVSION_Y,
		M_BULLET_SIZE, M_BULLET_SIZE, this->m_BulletImage, FALSE);	// �e�摜
	
	this->m_ZapperAnimationId = 0;
	this->m_isZapperFlip = false;
	this->m_FrameCount = 0;
	this->m_BlasterFrameCount = this->M_BLASTER_FRAME + 1;
	this->m_IsBlasterLaunching = false;
	memset(&this->m_zapperHit, false, sizeof(ZAPPER_HIT));

	// �J�[�\���摜
	LoadDivGraph("img/Xevious_OBJ_solvalou.png", M_CURSOR_IMG_DIVSION_NUM, M_CURSOR_IMG_DIVSION_X, M_CURSOR_IMG_DIVSION_Y,
		M_CURSOR_SIZE, M_CURSOR_SIZE, m_CursorImage, FALSE);	// �摜	�S�̂�6�����@��6�����@�c1����

	this->m_soundManagerP = soundManagerP;						// �T�E���h�}�l�[�W���|�C���^

	this->m_cursor = new Cursor(m_CursorImage);					// �J�[�\�����̐錾
	this->m_landingPoint = new LandingPoint(m_CursorImage);		// ���e�J�[�\���̎��̐錾
	this->m_blaster = new Blaster(this->m_BulletImage);			// �u���X�^�[�̎��̐錾

	for (int i = 0; i < ZAPPER_NUM; i++) {					// �U�b�p�[�̎��̐錾
		this->m_zapper[i] = new Zapper(this->m_BulletImage);
	}

}

// �f�X�g���N�^
BulletManager::~BulletManager() {
	delete m_cursor;
	delete m_landingPoint;
	delete m_blaster;

	for (int i = 0; i < ZAPPER_NUM; i++) {
		delete this->m_zapper[i];
	}
}

// �U�b�p�[����
void BulletManager::CreateZapper(float playerX, float playerY, char key) {
	if (key) {												// �L�[��������Ă�����
		if (this->m_FrameCount % M_RAPID_FRAME == 0) {		// �t���[���J�E���g�����ˊԊu�t���[���o�߂��閈��
			for (int i = 0; i < ZAPPER_NUM; i++) {		// �󂫂�T�����[�v
				if (!this->m_zapper[i]->GetOnActive()) {		// �󂫂���������
					this->m_zapper[i]->SetX(playerX);			// X���W�ݒ�
					this->m_zapper[i]->SetY(playerY + 6);		// Y���W�ݒ�
					this->m_zapper[i]->SetOnActive(true);		// ��ԃt���O��true��
					this->m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::ZapperShoot));	// SE�Đ�
					break;
				}
			}
		}
		this->m_FrameCount++;								// �t���[���J�E���g���C���N�������g
	}
	else if(this->m_FrameCount != 0){						// �L�[��������Ă��Ȃ��A���t���[���J�E���g��0�łȂ���
		this->m_FrameCount = 0;								// �t���[���J�E���g��0��
	}
}

// �u���X�^�[����
void BulletManager::CreateBlaster(float playerX, float playerY, char key){

	// �u���X�^�[���˒��łȂ�
	if (this->m_BlasterFrameCount > this->M_BLASTER_FRAME) {
		if (key) {	// ���˃L�[�����͂���Ă���
			this->m_BlasterFrameCount = 0;
			this->m_soundManagerP->SePlay(static_cast<int>(SeId::BlasterShoot));	// SE�Đ�
		}
	}
	else {
		if (this->m_BlasterFrameCount == this->M_BLASTER_WAIT) {
			this->m_landingPoint->Create(this->m_cursor->GetX(), this->m_cursor->GetY());	// ���e�J�[�\������
			this->m_blaster->Create(playerX, playerY);
			this->m_IsBlasterLaunching = true;
		}
	}
}

// �v�Z
void BulletManager::Calc(int frameCount) {

	if (frameCount % 2 == 0) {						// 2�t���[������
		this->m_ZapperAnimationId++;				// �A�j���[�V����ID���C���N�������g

		if (this->m_ZapperAnimationId >= 4) {		// �A�j���[�V����ID��4��������
			this->m_ZapperAnimationId = 0;			// �A�j���[�V����ID��0��
		}
	}

	this->m_isZapperFlip = !this->m_isZapperFlip;	// ���E���]

	for (int i = 0; i < ZAPPER_NUM; i++) {							// �v�Z���[�v
		if (this->m_zapperHit.enemy[i]) {
			this->m_zapper[i]->SetOnActive(false);	// ��ԃt���O��false��
			this->m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::EnemyStriken_Zapper));
		}


		this->m_zapper[i]->SetAnimationId(this->m_ZapperAnimationId);	// �A�j���[�V����ID���Z�b�g
		this->m_zapper[i]->SetIsTurn(this->m_isZapperFlip);				// ���E���]�t���O���Z�b�g
		this->m_zapper[i]->Calc();										// �v�Z

		if (this->m_zapper[i]->GetY() < 0) {			// ��ʊO�ɏo����
			this->m_zapper[i]->SetOnActive(false);	// ��ԃt���O��false��
		}

	}

	if (this->m_BlasterFrameCount <= this->M_BLASTER_FRAME) {
		this->m_BlasterFrameCount++;
	}
	if (this->m_BlasterFrameCount == this->M_BLASTER_FRAME) {
		this->m_IsBlasterLaunching = false;
	}
}

/******************************************************************
�֐����FCursorCalc

�@�\�F���@�J�[�\���̌v�Z���s���܂�

�����F
float�^	playerX		(i)
float�^	playerY		(i)
int�^	frameCount	(i)

�ԋp�l�F�Ȃ�

���ӎ����F

*********************************************************************/
void BulletManager::CursorCalc(float playerX, float playerY, int frameCount) {
	this->m_cursor->Calc(playerX, playerY, this->m_OnCursor, this->m_IsBlasterLaunching, frameCount);
}

// ���e�J�[�\���̌v�Z���s���܂�
void BulletManager::LandingPointCalc() {
	this->m_landingPoint->Calc();
}

// �u���X�^�[�̌v�Z���s���܂�
void BulletManager::BlasterCalc() {
	this->m_blaster->Calc();
}


// �U�b�p�[�̕`����s���܂�
void BulletManager::ZapperDraw() {
	for (int i = 0; i < ZAPPER_NUM; i++) {
		if (this->m_zapper[i]->GetOnActive()) {			// �A�N�e�B�u�ȕ��̂�
			this->m_zapper[i]->Draw();

		}
	}
}

// ���@�J�[�\���̕`����s���܂�
void BulletManager::CursorDraw() {
	this->m_cursor->Draw();
}

// ���e�J�[�\���̕`����s���܂�
void BulletManager::LandingPointDraw() {
	this->m_landingPoint->Draw();
}

// �u���X�^�[�̕`����s���܂�
void BulletManager::BlasterDraw() {
	this->m_blaster->Draw();
}

/******************************************************************
�֐����FGetZapperX

�@�\�F�����Ŏw�肵��ID�̃U�b�p�[��X���W��Ԃ��܂�

�����F	int�^	zapperId	(i)	�U�b�p�[��ID

�ԋp�l�F
	�����FX���W
	���s�F0
���ӎ����F
	�P�FzapperId�ɂ�BulletManager::M_ZAPPER_NUM�����i3�����j�̒l���g�p���Ă�������
	�Q�F�e���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ0��Ԃ��܂�
*********************************************************************/
float BulletManager::GetZapperX(int zapperId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (zapperId < ZAPPER_NUM && this->m_zapper[zapperId]->GetOnActive()) {
			return this->m_zapper[zapperId]->GetX();
	}
	return 0;
}

/******************************************************************
�֐����FGetZapperY

�@�\�F�����Ŏw�肵��ID�̃U�b�p�[��Y���W��Ԃ��܂�

�����F	int�^	zapperId	(i)	�U�b�p�[��ID

�ԋp�l�F
	�����FY���W
	���s�F0
���ӎ����F
	�P�FzapperId�ɂ�BulletManager::M_ZAPPER_NUM�����̒l���g�p���Ă�������
	�Q�F�e���A�N�e�B�u�łȂ��A�܂��͕s����ID�̏ꍇ0��Ԃ��܂�
*********************************************************************/
float BulletManager::GetZapperY(int zapperId) {
	// ������������ID�A���Ώۂ��A�N�e�B�u�̏ꍇ
	if (zapperId < ZAPPER_NUM && this->m_zapper[zapperId]->GetOnActive()) {
			return this->m_zapper[zapperId]->GetY();
	}
	return 0;
}

/******************************************************************
�֐����FGetZapperY

�@�\�F�����Ŏw�肵��ID�̃U�b�p�[�̃A�N�e�B�u��Ԃ�Ԃ��܂�

�����F	int�^	zapperId	(i)	�U�b�p�[��ID

�ԋp�l�F	bool�^
	�����F�A�N�e�B�u���
	���s�F0
���ӎ����F
	�P�FzapperId�ɂ�BulletManager::M_ZAPPER_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇfalse��Ԃ��܂�
*********************************************************************/
bool BulletManager::GetZapperOnActive(int zapperId) {
	if (zapperId < ZAPPER_NUM) {
		return this->m_zapper[zapperId]->GetOnActive();
	}
	return false;
}

int BulletManager::GetBlasterFrameCount() {
	return this->m_BlasterFrameCount;
}

float BulletManager::GetLandingPointX() {
	return this->m_landingPoint->GetX();
}

float BulletManager::GetLandingPointY() {
	return this->m_landingPoint->GetY();
}

float BulletManager::GetCursorX() {
	return this->m_cursor->GetX();
}

float BulletManager::GetCursorY() {
	return this->m_cursor->GetY();
}


// �I���J�[�\���Z�b�^�[
void BulletManager::SetOnCursor(bool onCursor) {
	this->m_OnCursor = onCursor;
}

/******************************************************************
�֐����FSetZapperOnActive

�@�\�F�����Ŏw�肵��ID�̃U�b�p�[�̃A�N�e�B�u��Ԃ�ݒ肵�܂�

�����F	int�^	zapperId	(i)	�U�b�p�[��ID
		bool�^	onActive	(i)	�A�N�e�B�u���

�ԋp�l�F�Ȃ�

���ӎ����F
	�P�FzapperId�ɂ�BulletManager::M_ZAPPER_NUM�����̒l���g�p���Ă�������
	�Q�F�s����ID�̏ꍇ�A�Ȃɂ����܂���
*********************************************************************/
void BulletManager::SetZapperOnActive(int zapperId, bool onActive) {
	if (zapperId < ZAPPER_NUM) {
			this->m_zapper[zapperId]->SetOnActive(onActive);
	}
}

// �U�b�p�[�q�b�g���󂯎��
void BulletManager::SetZapperHit(ZAPPER_HIT zapperHit) {
	this->m_zapperHit = zapperHit;
}