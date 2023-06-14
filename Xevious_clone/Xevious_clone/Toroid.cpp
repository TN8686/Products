#include "Toroid.h"

/* 
* �R���X�g���N�^�̈����́A�Ȃ�ׂ���������Enemy�N���X�̃R���X�g���N�^�̈����̏��Ԃƈ�v�����Ă�������
* ���̉���Enemy�N���X�̃R���X�g���N�^���Ă�ł��܂��@���s���Ă��܂����A��s�̍\���ł��@�u �q�N���X��(�q�N���X�������X�g) : �e�N���X��(�e�N���X�������X�g) �v�ƂȂ��Ă��܂�
* �܂��A�S�Ă̓G�ňꉞX���WY���W���w��ł���悤�ɂ��鎖�ɂ��܂����@�o��Y���W���{���Œ�̓G�������ɗp�ӂ��Ă�������
* Enemy�N���X�̃R���X�g���N�^�ɂ́A���e�����Ȓl���A�Q�Ɖ\�Ȓl���g�p�ł��܂��@�q�N���X�̃R���X�g���N�^�̈�����������ė����l���Q�Ɖ\�ł�
* Enemy�N���X�̃R���X�g���N�^�̈����ɂ�����̂́A�����ő�����Ă��������@���������鎞�Ɉ������X�g���\�������Ǝv���܂�
* 
* ���̌�A�ʂ̓G�Œǉ������ϐ���A�^�C�v�ŃX�R�A���ς�镨�Ȃǂ̓R���X�g���N�^�̒��ő�����Ă�������
* 
* IMAGE_HANDLE_STRUCT�^��Enemy�N���X�̃w�b�_�[�ɒ�`����Ă���A�G���g�p����S�Ẳ摜�n���h��������Z�߂ɂ����\���̂ł�
* EnemyManager�̒��ŉ摜���[�h���Ă��̌^�̕ϐ��ɑS�Ẵn���h����ێ����Ă��܂�
* ���̕ϐ��̃|�C���^���Q�Ƃ��鎖�ŁA�S�Ẳ摜�ɃA�N�Z�X�ł����Ԃɂ��Ă��܂�
* Draw�֐��Ŏg�p����̂ŁAEnemy.h�ō\�����m�F���Ă����ĉ�����
*/
Toroid::Toroid(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::TOROID), x, y, type, 0, imageHandleP, scoreManagerP) {

	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);		// X�AY�ړ����x��ݒ�
	m_AccelerationX = 0.0625;				// X�����x
	m_isRightAway = false;					// �E�����t���O
	m_isLeftAway = false;					// �������t���O
	m_AnimationSum = 0;						// �摜�Q�ƃI�t�Z�b�g
	m_Flame = 2;							// �t���[���J�E���g
	m_Score = 3;							// �X�R�A
}


/*
* �n��G�p��Calc�Ƌ󒆓G�p��Calc�ň������قȂ�܂��@���ӂ��Ă�������
* �n��p��Bara���Q�Ƃ��Ă�������
* 
* �����Ɍ��݂̒n��X�N���[�����x�Ɣ�e�������悤�ɂ��܂���
* �n��X�N���[�����x���g�p����ꍇ�͂�������Q�Ƃ��Ă��������i�g���̂͋󒆓G�ł͑����^���P���̂݁H�j
*/
void Toroid::Calc(float playerX, float playerY, float groundMove, bool isHit) {
	/*
	* ��ʂ̓G��DamageFunction�ɔ�e����n�������Ŕ�e�������ł��܂��@�ꉞ�֐��̒��g���ǂ�ł����ĉ�����
	* DamageFunction�͕K����ԍŏ��ɌĂ�ł���ASwitch���Ɉڍs���Ă�������
	*/
	DamageFunction(isHit);	// ��e��񏈗�

	/*
	* ������Switch���Ŏ��g�̏�ԕϐ��ɉ����ď�����ς��܂�
	* �ʏ�̍s����ACTIVE�̗��ɏ����܂��@������Ί֐��ɐ؂�o���āA�����ACTIVE�̗��ŌĂԌ`�ɂ��Ă��ǂ��ł�
	* �����āABROKEN�̗���BrokenCalc���ĂԂ̂���{�\���ɂȂ�܂�
	*/
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):

		// ��������
		if (!m_isRightAway && !m_isLeftAway &&									// �����t���O���ǂ���������Ă��Ȃ�
			((int)playerX / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) < 2 &&		// �����@�Ƃ̍��W����2�L�����N�^����
			((int)playerX / CHARACTER_SIZE) - ((int)m_X / CHARACTER_SIZE) >= -2) {	// �����@�Ƃ̍��W����-2�L�����N�^�ȏ�	����ł̍��E�ł̃Y�����Č�
			if (playerX >= m_X) {
				m_isRightAway = true;		// �E���痈�Ă鎞�͉E�ɓ�����t���O��ON
				if (m_Type == 1) {		// �e�����^�C�v�̏ꍇ
					m_Shot();				// �e�𔭎�
				}
			}
			else {
				m_isLeftAway = true;		// �����痈�Ă鎞�͍��ɓ�����t���O��ON
				if (m_Type == 1) {		// �e�����^�C�v�̏ꍇ
					m_Shot();				// �e�𔭎�
				}
			}
		}

		// ��������
		if (m_isRightAway) {
			m_SpeedX -= m_AccelerationX;	// �E�ɉ������Ă���

			// �A�j���[�V��������
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum -= 1;
				if (m_AnimationSum < 0) {
					m_AnimationSum = 7;
				}
				m_Flame += 2;
			}

		}
		if (m_isLeftAway) {
			m_SpeedX += m_AccelerationX;	// ���ɉ������Ă���

			// �A�j���[�V��������
			m_Flame--;
			if (m_Flame <= 0) {
				m_AnimationSum += 1;
				if (m_AnimationSum >= 8) {
					m_AnimationSum = 0;
				}
				m_Flame += 2;
			}

		}

		//���x�����W�ɔ��f
		m_X += m_SpeedX;
		m_Y += m_SpeedY;

		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}


/*
* Calc�Ɠ��lSwitch���ŌĂѕ����܂�
* ������colorID�����悤�ɂ��܂���
* �Q�Ƃ���摜�ɂ���Ă��̒l�̎g�����͈قȂ邽�߁A�K��IMAGE_HANDLE_STRUCT�\���̂Ɖ摜�\�[�X���m�F���Ă����ĉ�����
* �������A�v�Z����ύX����K�v�͖����͂��ł�
*/
// �`��
void Toroid::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
		DrawGraph((int)m_Y, (int)m_X, m_imageHandleP->little[colorID][8 + m_AnimationSum], true);	// ��������J���[�ݒ�
		break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}