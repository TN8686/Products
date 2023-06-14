#include "Zoshi.h"

Zoshi::Zoshi(float x, float y, float playerX, float playerY, int type, IMAGE_HANDLE_STRUCT* imageHandleP, ScoreManager* scoreManagerP)
	: Enemy(static_cast<int>(EnemyId::ZOSHI), x, y, type, 0, imageHandleP, scoreManagerP) {
	switch (m_Type)																// �^�C�v�ŕ���
	{
	case 0:
		m_Score = 7;							// �X�R�A									
		break;
	case 1:
	case 2:
		m_Score = 10;							// �X�R�A
		break;

	default:
		break;
	}

	SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);		// X�AY�ړ����x��ݒ�
	this->m_Flame = 1;															//�A�j���[�V�����p�t���[���������l
	this->m_AnimationSum = 0;													//�A�j���[�V�������Z�����l
	//�����_���ȃt���[���擾
	std::random_device rd;
	std::default_random_engine rand(rd());
	std::uniform_int_distribution<int> RandFlame(MIN, MAX);						//64�`127
	std::uniform_int_distribution<int> Rand(MIN2, MAX2);						//1�`32
	this->m_seed = rand;
	this->m_rand = RandFlame;													//64�`127
	this->m_rand2 = Rand;														//1�`32
	this->m_Rand = this->m_rand2(this->m_seed);									//�����]���@1�`32
	this->m_RandFlame = this->m_rand(this->m_seed);								//�e�Ԋu�@64�`127 �s���̂��߃^���P���Q�l
}

void Zoshi::Calc(float playerX, float playerY, float groundMove, bool isHit) {
	DamageFunction(isHit);	// ��e��񏈗�
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
	this->m_RandFlame--;
	this->m_Flame--;
	if (this->m_Flame <= 0) {																//�t���[������							
		if (this->m_AnimationSum <= 2) {													//�A�j���[�V�������Z����
			this->m_AnimationSum++;
		}
		else if (this->m_AnimationSum == 3) {												//���[�v�̂��߃��Z�b�g
			this->m_AnimationSum = 0;
		}
		this->m_Flame = 1;																	//���t���[��
	}

	switch (m_Type)	// �^�C�v�ŕ���
	{
	case 0:	// �ォ��o�����A�����]���̓����_���ȕ����B70�_
																							//�e���ˏ���
		if (this->m_RandFlame <= 0) {						// �����_���ȃt���[����0���e���ˌ�t���O��false�Ȃ�
			this->m_Shot();																	// �e�𔭎�
			this->m_Rand = this->m_rand2(this->m_seed);										// �������擾�@�P�`�R�Q
			this->SetSpeedFromAngle(static_cast<float>(m_Rand) * RAD_32, M_SPEED_TABLE[0]);	// X�AY�ړ����x���Đݒ�
			this->m_RandFlame= this->m_rand(this->m_seed);									// �������擾�R�Q�`�P�Q�V
		}
		break;
	case 1:	//�ォ��o�����A�����]���̓\���o���E�̕����B100�_
	case 2:	//�\���o���E�Ƃ� X ���� 8 �L�����N�^�ȏ�̉�ʉ�������o�����A�����]���̓\���o���E�̕���
		if (this->m_RandFlame <= 0) {						// �����_���ȃt���[����0	// �e���ˌ�t���O��false�Ȃ�
			this->m_Shot();																	// �e�𔭎�
			this->SetSpeedFromAngle(Angle(m_X, m_Y, playerX, playerY), M_SPEED_TABLE[0]);	// X�AY�ړ����x���Đݒ�
			this->m_RandFlame= this->m_rand(this->m_seed);									// �������擾�U�S�`�P�Q�V
		}
		break;
	default:	// �ǂ�ɂ���v���Ȃ�������_�~�[
		break;
	}
	//�ړ����Z
	this->m_X += this->m_SpeedX;
	this->m_Y += this->m_SpeedY;
	break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenCalc(groundMove);
		break;

	default:
		break;
	}
}
void Zoshi::Draw(int colorID) {
	switch (m_Status)
	{
	case static_cast<int>(StatusId::ACTIVE):
	DrawGraph((int)this->m_Y, (int)this->m_X, m_imageHandleP->little[colorID][32 + m_AnimationSum], true);
	break;

	case static_cast<int>(StatusId::BROKEN):
		BrokenDraw();
		break;

	default:
		break;
	}
}