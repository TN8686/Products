#include "Enemy.h"

// �_�~�[�p�R���X�g���N�^
Enemy::Enemy() {
	m_CharcterId = static_cast<int>(EnemyId::DUMMY);
	m_Status = static_cast<int>(StatusId::NON_EXISTENT);		// ��ԁ@0:�񑶍݁@1:��ʊO�ҋ@�@2:���쒆�@3:�j��
	m_X = 0;						// X���W
	m_Y = 0;						// Y���W
	m_SpeedX = 0;					// X�ړ����x
	m_SpeedY = 0;					// Y�ړ����x
	m_AccelerationX = 0;			// X�����x
	m_AccelerationY = 0;			// Y�����x
	m_Width = 0;					// ��
	m_Height = 0;					// ����
	m_Score = 1;					// �X�R�A
	m_IsShot = false;				// �V���b�g�t���O�@true���ƒe������
	m_isCollision = false;			// �H�炢����t���O
	m_ShotInterval = 1;				// �e�𕡐����ꍇ�̎ˌ��Ԋu�@���ۂ̏����ł�8�{���Ĉ���
	m_Type = 0;						// ���^�ŕ����^�C�v������G�p
	m_BrokenType = 0;				// �j��A�j���[�V�����^�C�v
	m_imageHandleP= NULL;			// �摜�n���h���|�C���^
	m_scoreManagerP = NULL;			// �X�R�A�}�l�[�W���|�C���^
	m_pParentObject = NULL;			// �e�I�u�W�F�N�g�|�C���^
	m_IsFlip = true;				//���]�t���O
	m_BrokenImageAnimation = 0;		//�j��A�j���[�V�������Z
	m_Angle = 0;					//�p�x
	m_FCount = 0;					//�t���[���J�E���g
}

// �_�~�[�ȊO�p�R���X�g���N�^
Enemy::Enemy(int charcterId, float x, float y,int type, int brokenType, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP) {
	m_CharcterId = charcterId;
	m_Status = static_cast<int>(StatusId::ACTIVE);		// ���
	m_X = x;						// X���W
	m_Y = y;						// Y���W
	m_SpeedX = 0;					// X�ړ����x
	m_SpeedY = 0;					// Y�ړ����x
	m_AccelerationX = 0;			// X�����x
	m_AccelerationY = 0;			// Y�����x
	m_Width = LITTLE_SIZE;			// ��
	m_Height = LITTLE_SIZE;			// ����
	m_Score = 1;					// �X�R�A
	m_IsShot = false;				// �V���b�g�t���O�@true���ƒe������
	m_isCollision = true;			// �H�炢����t���O
	m_ShotInterval = 1;				// �e�𕡐����ꍇ�̎ˌ��Ԋu�@���ۂ̏����ł�8�{���Ĉ���
	m_Type = type;					// ���^�ŕ����^�C�v������G�p
	m_BrokenType = brokenType;		// �j��A�j���[�V�����^�C�v
	m_imageHandleP = imageHandleP;	// �摜�n���h���z��|�C���^
	m_scoreManagerP = scoreManagerP;// �X�R�A�}�l�[�W���|�C���^
	m_pParentObject = NULL;			// �e�I�u�W�F�N�g�|�C���^
	m_IsFlip = true;				//���]�t���O
	m_BrokenImageAnimation = 0;		//�j��A�j���[�V�������Z
	m_Angle = 0;					//�p�x
	m_FCount = 0;					//�t���[���J�E���g
}

void Enemy::m_Shot() {
	m_IsShot = true;
}

// Get�֐�
int Enemy::GetCharcterId() {	// �L�����N�^ID
	return m_CharcterId;
}
int Enemy::GetStatus() {		// ���
	return m_Status;
}
float Enemy::GetX() {			// X���W
	return m_X;
}
float Enemy::GetY() {			// Y���W
	return m_Y;
}
int Enemy::GetWidth() {			// Y���W
	return m_Width;
}
int Enemy::GetHeight() {		// Y���W
	return m_Height;
}
bool Enemy::GetIsShot() {		// �V���b�g�t���O
	return m_IsShot;
}
bool Enemy::GetIsCollision() {	// �H�炢����
	return m_isCollision;
}
int Enemy::GetScore() {			// �X�R�A
	return m_Score;
}

// Set�֐�
void Enemy::SetStatus(int status) {					// ��ԃZ�b�^�[
	m_Status = status;
}

void Enemy::SetIsCollision(bool isCollision) {		// �H�炢����t���O�Z�b�^�[
	m_isCollision = isCollision;
}

void Enemy::SetPParentObject(Enemy* pParentObject){	// �e�I�u�W�F�N�g�|�C���^�Z�b�^�[
	m_pParentObject = pParentObject;
}

void Enemy::SetIsShotFalse() {						// �V���b�g�t���O��false�ɂ���
	m_IsShot = false;
}

// (x1, y1)���W���猩��(x2, y2)���W�́A����0�x�Ƃ��Ĕ����v����32�����̃��W�A���l��Ԃ��܂�
float Enemy::Angle(float x1, float y1, float x2, float y2) {
	int quarter = 0;	// �l�����łǂ���������
	float angle = (float)atan2((double)y2 - (double)y1, (double)x2 - (double)x1);	// �����Ȋp�x

	// ����񐔂����肳���邽�߂ɂ܂����W����l�����łǂ������𔻒肷��
	if (x2 <= x1) {		// x2���E��
		if (y2 <= y1) {	// y2���㑤
			quarter = 2;
		}
		else {			// y2������
			quarter = 1;
		}
	}else {				// x2������
		if (y2 <= y1) {	// y2���㑤
			quarter = 3;
		}
		else {			// y2������
			quarter = 0;
		}
	}

	// 360�x��ł̃��W�A���l�ɕϊ�
	if (quarter >= 2) {
		angle = PI * 2 + angle;
	}

	/*
	�p�x�`�F�b�N���[�v
	32���������p�x���A64����1�̊p�x���Y�����Ĕ��肷�鎖�ŁA��̉E�Ȃ�E�A�݂����Ȕ���ɂ��Ă���
	*/
	for (int i = 0; i <= 8; i++) {
		if (angle < (RAD_32 * (i + 1)) + (RAD_4 * quarter) - (RAD_32 / 2)) {
			float test = (RAD_32 * i) + (RAD_4 * quarter);
			return (RAD_32 * i) + (RAD_4 * quarter);
		}
	}

	return -1;	// �G���[�@�����ǂ�������Ԉ���Ă�
}

// �p�x�Ɗ���x������X�AY�ړ����x��ݒ肷��֐�
void Enemy::SetSpeedFromAngle(float angle, float speed) {
	m_SpeedX = speed * (float)cos(angle);
	m_SpeedY = speed * (float)sin(angle);
}

// �X�R�A���Z�֐��@���g�̃X�R�A�����Z����
void Enemy::AddScore() {
	if (m_scoreManagerP != NULL) {
		m_scoreManagerP->ScoreCalc(m_Score);
	}
}


/*
* ���O�FDamageFunction
* 
* �@�\�F��{�̔�e�������s���܂��@�ʏ�̓G�͂��̊֐����ĂԂ�����OK�ł�
* 
* �����G(i) bool isHit �q�b�g���
* 
* �߂�l�F����
* 
* ���ӁF�ʏ�̓G�̏ꍇ��Calc�̒��ŕK����ԍŏ��ɂ��̊֐����ĂсA���̌㎩�g��Status������Switch����u���Ă�������
*/
void Enemy::DamageFunction(bool isHit) {
	if (isHit) {				// �q�b�g����
		AddScore();				// �X�R�A���Z
		m_isCollision = false;	// ����𖳂���
		//m_Status = static_cast<int>(StatusId::BROKEN);		// ��Ԃ�BROKEN��
		m_Status++;												// �C���N�������g�ōς܂���
	}
}

// �v�Z�n�@Enemy�N���X�ł̓_�~�[�Ȃ̂łȂɂ����Ȃ�
// �X�p���I�v�Z�p
void Enemy::Calc() {}

// �󒆓G�p�v�Z
void Enemy::Calc(float playerX, float playerY, float groundMove, bool isHit) {}

// �n��G�p�v�Z
void Enemy::Calc(float playerX, float playerY, float cursorX, float cursorY, float landingPointX, float landingPointY, float groundMove, bool isHit) {}

// �`��@�_�~�[�Ȃ̂łȂɂ����Ȃ�
void Enemy::Draw(int colorID) {}

// �j��v�Z
void Enemy::BrokenCalc(float groundMove) {
	switch (m_BrokenType)					// �����Ń^�C�v�ŕ��򂵂Đ���
	{
	case 0:		
		AerialBrokenCalc();					//�󒆓G�v�Z
		break;
	case 1:
		GroundBrokenCalc(groundMove);		//�n�㕨�`��@�N���[�^�[����
		break;
	case 2:
		GroundBrokenCalc2(groundMove);		//�n�㕨2�`��@�N���[�^�[�Ȃ�
		break;
	default:
		break;
	}
}

// �j��`��
void Enemy::BrokenDraw() {
	switch (m_BrokenType)					// �����Ń^�C�v�ŕ��򂵂Đ���
	{
	case 0:
		AerialBrokenDraw();					//�󒆓G�`��
		break;
	case 1:
		GroundBrokenDraw();					// �n�㕨�`��@�N���[�^�[����
		break;
	case 2:
		GroundBrokenDraw2();				// �n�㕨�Q�`��@�N���[�^�[�Ȃ�
		break;
	default:
		break;
	}
}

//�󒆓G�j��v�Z
void Enemy::AerialBrokenCalc() {
	m_IsFlip = true;
	switch (m_FCount % 4)			// �����Ń^�C�v�ŕ��򂵂Đ��� �t���[���̗]��Z
	{
	case 0:
		m_IsFlip = false;
		m_Angle = 0;				//���]�Ȃ�
		break;

	case 1:
	    m_Angle = 0;				//�㉺���]
		break;

	case 2:
		m_Angle = (double)PI;		//���E���]
		break;

	case 3:
		m_IsFlip = false;
		m_Angle = (double)PI;		//�㉺���E���]
		break;

	default:
		break;
	}

	if (m_FCount % 4 == 0 && m_FCount != 0) { //�͂��߂�����4�t���[�����@
		m_BrokenImageAnimation++;
	}
	if (m_BrokenImageAnimation >= 5) { //20�t���[����ɔ񑶍�
		m_Status = static_cast<int>(StatusId::NON_EXISTENT);
	}
	m_FCount++;									//�t���[�����Z

	m_Y += m_SpeedY;						//Y�ړ����Z
	m_X += m_SpeedX;						//X�ړ����Z
}



//�n��G�j��v�Z
void Enemy::GroundBrokenCalc(float groundMove) {
	m_Y += groundMove;

	if (m_FCount % 8 == 0 && m_FCount != 0) { //�͂��߂�����8�t���[�����@
		m_BrokenImageAnimation++;
	}
	if (m_FCount >= 55) {  //55�t���[����ɔj���
		m_Status = static_cast<int>(StatusId::DESTROYED);
	}
	m_FCount++;
}

//�n��G�j��v�Z����2
void Enemy::GroundBrokenCalc2(float groundMove) {
	m_Y += groundMove;

	if (m_FCount % 4 == 0 && m_FCount != 0) { //�͂��߂�����4�t���[�����@
		m_BrokenImageAnimation++;
	}
	if (m_BrokenImageAnimation >= 7) {  //�Q�W�t���[����ɔj���
		m_Status = static_cast<int>(StatusId::NON_EXISTENT);
	}
	m_FCount++;
}

//�󒆓G�j��`��
void Enemy::AerialBrokenDraw() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[0 + m_BrokenImageAnimation], true, m_IsFlip);
}

//�n��G�j��`��
void Enemy::GroundBrokenDraw() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[7 + m_BrokenImageAnimation], true, m_IsFlip);
}

//�n��G�j��`�悻��2
void Enemy::GroundBrokenDraw2() {
	DrawRotaGraph((int)(m_Y + (int)(m_Height / 2)), (int)(m_X + (int)(m_Width / 2)),
		1.0, m_Angle, m_imageHandleP->broken[7 + m_BrokenImageAnimation], true, m_IsFlip);
}