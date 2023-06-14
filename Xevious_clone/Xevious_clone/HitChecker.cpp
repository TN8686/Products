#include "HitChecker.h"

HitChecker::HitChecker(EnemyManager* enemyManagerP, BulletManager* bulletManagerP, Player* playerP, SoundManager* soundManagerP) {
	memset(&m_EnemyHit, false, sizeof(EnemyManager::ENEMY_HIT));
	memset(&m_ZapperHit, false, sizeof(BulletManager::ZAPPER_HIT));

	m_enemyManagerP = enemyManagerP;
	m_bulletManagerP = bulletManagerP;
	m_playerP = playerP;
	m_soundManagerP = soundManagerP;
}

void HitChecker::Calc() {
	// ������
	memset(&m_EnemyHit, false, sizeof(EnemyManager::ENEMY_HIT));
	memset(&m_ZapperHit, false, sizeof(BulletManager::ZAPPER_HIT));

	// �q�b�g�`�F�b�N
	AerialHitCheck();
	GroundHitCheck();
	SparioHitCheck();

	// �q�b�g����n��
	m_enemyManagerP->SetEnemyHit(m_EnemyHit);
	m_bulletManagerP->SetZapperHit(m_ZapperHit);
}

// �󒆓G�q�b�g�`�F�b�N
void HitChecker::AerialHitCheck() {
	bool ret;

	for (int i = 0; i < AERIAL_NUM; i++) {					// �󒆓G�`�F�b�N���[�v
		if (m_enemyManagerP->GetAerialIsCollision(i)) {	// �H�炢���肪����
			for (int j = 0; j < ZAPPER_NUM; j++) {				// �U�b�p�[�`�F�b�N���[�v
				if (m_bulletManagerP->GetZapperOnActive(j)) {
					ret = HitCheck(
						m_bulletManagerP->GetZapperX(j), m_bulletManagerP->GetZapperY(j),		// �U��������W
						BulletManager::M_BULLET_SIZE, BulletManager::M_BULLET_SIZE,							// �U�����蕝�A����
						m_enemyManagerP->GetAerialX(i) + EnemyManagerConst::HURTBOX_ORIGIN,				// �H�炢����X���W
						m_enemyManagerP->GetAerialY(i) + EnemyManagerConst::HURTBOX_ORIGIN,				// �H�炢����Y���W
						EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);						// �H�炢���蕝�A����

					if (!m_EnemyHit.aerial[i]) {		// �q�b�g���false��������
						m_EnemyHit.aerial[i] = ret;	// ���ʂ��Z�b�g
					}
					if (!m_ZapperHit.enemy[j]) {		// �q�b�g���false��������
						m_ZapperHit.enemy[j] = ret;	// ���ʂ��Z�b�g
					}
				}
			}

			// �v���C���[�q�b�g�`�F�b�N
			if (m_playerP->GetOnActive()) {	// �v���C���[���A�N�e�B�u�Ȏ�
				ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
					m_enemyManagerP->GetAerialX(i) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetAerialY(i) + EnemyManagerConst::HURTBOX_ORIGIN,
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

				if (ret) {
					m_playerP->SetIsHit(true);
				}
			}
		}
	}
}

// �n��G�q�b�g�`�F�b�N
void HitChecker::GroundHitCheck() {
	bool ret = false;
	bool isBlasterHit = false;
	bool onCursor = false;

	// �I���J�[�\������
	for (int i = 2; i < GROUND_NUM; i++) {					// �n��G�`�F�b�N���[�v�@1��2�͗�O�g�Ŕ������Ȃ�
		if (m_enemyManagerP->GetGroundIsCollision(i)) {	// �H�炢���肪����
			ret = HitCheck(
				m_bulletManagerP->GetCursorX(), m_bulletManagerP->GetCursorY(),			// �J�[�\�����W
				BulletManager::M_CURSOR_SIZE, BulletManager::M_CURSOR_SIZE,				// �J�[�\�����A����
				m_enemyManagerP->GetGroundX(i) + EnemyManagerConst::HURTBOX_ORIGIN,		// �H�炢����X���W
				m_enemyManagerP->GetGroundY(i) + EnemyManagerConst::HURTBOX_ORIGIN,		// �H�炢����Y���W
				EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);		// �H�炢���蕝�A����

			if (ret) {									// ��x�ł��q�b�g������
				onCursor = true;						// true�ɂ��ă��[�v�E�o
				break;
			}
		}
	}
	m_bulletManagerP->SetOnCursor(onCursor);		// ���ʂ��Z�b�g

	// �u���X�^�[�q�b�g����
	ret = false;	// ������
	if (m_bulletManagerP->GetBlasterFrameCount() == BulletManager::M_BLASTER_FRAME - 1) {	//	�u���X�^�[���e�t���[����1f�O
		for (int i = 0; i < GROUND_NUM; i++) {					// �n��G�`�F�b�N���[�v
			if (m_enemyManagerP->GetGroundIsCollision(i)) {	// �H�炢���肪����
				ret = HitCheck(
					m_bulletManagerP->GetLandingPointX(), m_bulletManagerP->GetLandingPointY(),			// �U��������W
					BulletManager::M_BULLET_SIZE, BulletManager::M_BULLET_SIZE,							// �U�����蕝�A����
					m_enemyManagerP->GetGroundX(i) + EnemyManagerConst::HURTBOX_ORIGIN,					// �H�炢����X���W
					m_enemyManagerP->GetGroundY(i) + EnemyManagerConst::HURTBOX_ORIGIN,					// �H�炢����X���W
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);					// �H�炢���蕝�A����


				//if (!m_EnemyHit.ground[i]) {		// �q�b�g���false�������� �����̒e�Ŕ�������ꍇ�K�v�ɂȂ鏈��
					m_EnemyHit.ground[i] = ret;	// ���ʂ��Z�b�g
				//}
				if (!isBlasterHit && ret) {				// �u���X�^�[�q�b�g��false�Ŕ��茋�ʂ�true��������
					isBlasterHit = true;				// �u���X�^�[�q�b�g��true��
				}

			}
		}
		if (isBlasterHit) {

			m_soundManagerP->NoisePlay(static_cast<int>(NoiseSeId::EnemyStriken_Blaster));
		}
	}

	// �v���C���[��SP�t���b�O�̃q�b�g�`�F�b�N
	if (m_playerP->GetOnActive() && m_enemyManagerP->GetGroundCharcterId(0) == static_cast<int>(EnemyId::SPFLAG) &&
		!m_enemyManagerP->GetGroundIsCollision(0)) {	// �v���C���[���A�N�e�B�u�ŁA�Ώۂ�SP�t���b�O�ŁA�H�炢���肪������
		ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
			m_enemyManagerP->GetGroundX(0) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetGroundY(0) + EnemyManagerConst::HURTBOX_ORIGIN,
			EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

		if (ret) {
			m_EnemyHit.ground[0] = ret;
			m_playerP->Extend();
			m_soundManagerP->NoBgmSePlay(static_cast<int>(NoBgmSeId::SpecialFlag_get));
		}
	}
}

// �X�p���I�i�G�ʏ�e�j�q�b�g�`�F�b�N
void HitChecker::SparioHitCheck() {
	bool ret = false;

	if (m_playerP->GetOnActive()) {				// �v���C���[���A�N�e�B�u
		for (int i = 0; i < SPARIO_NUM; i++) {	// �e�`�F�b�N���[�v
			if (m_enemyManagerP->GetBulletStatus(i) == static_cast<int>(StatusId::ACTIVE)) {	// �e���A�N�e�B�u
				ret = HitCheck(m_playerP->GetX(), m_playerP->GetY(), Player::PLAYER_SIZE, Player::PLAYER_SIZE,
					m_enemyManagerP->GetBulletX(i) + EnemyManagerConst::HURTBOX_ORIGIN, m_enemyManagerP->GetBulletY(i) + EnemyManagerConst::HURTBOX_ORIGIN,
					EnemyManagerConst::HURTBOX_SIZE, EnemyManagerConst::HURTBOX_SIZE);

				if (ret) {
					m_playerP->SetIsHit(true);	// ���ʂ��Z�b�g
					break;							// ���[�v�E�o
				}
			}
		}
	}
}

/******************************************************************
�֐����FHitCheck

�@�\�F�q�b�g�`�F�b�N���s����{�֐��ł�

�����F
	float�^ HitBoxX, HitBoxY			(i)	�U��������W
	int�^	HitBoxWidth, HitBoxHeight	(i)	�U�����蕝�A����
	float�^	HurtBoxX, HurtBoxY			(i)	�H�炢������W
	int�^	HurtBoxWidth, HurtBoxHeight	(i)	�H�炢���蕝�A����

�ԋp�l�F bool�^�@��͈̔͂��d�Ȃ��Ă��邩�ۂ��@�d�Ȃ��Ă���:true�@�d�Ȃ��Ă��Ȃ�:false
*********************************************************************/
bool HitChecker::HitCheck(float HitBoxX, float HitBoxY, int HitBoxWidth, int HitBoxHeight, float HurtBoxX, float HurtBoxY, int HurtBoxWidth, int HurtBoxHeight) {
	if (HitBoxX <= HurtBoxX + HurtBoxWidth && HitBoxX + HitBoxWidth >= HurtBoxX &&
		HitBoxY <= HurtBoxY + HurtBoxHeight && HitBoxY + HitBoxHeight >= HurtBoxY) {
		return true;
	}
	return false;
}

