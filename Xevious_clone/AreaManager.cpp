#include "AreaManager.h"

AreaManager::AreaManager(){
	Init();
}

void AreaManager::Init() {
	m_aria = 0;												// �G���A
	m_loadAria = 0;											// ���[�h�p�G���A
	m_isStart = true;
	m_line = M_LINE_INIT;									// ���C��
	m_groundEnemyLine = M_LINE_INIT;						// �n��G���C��
	m_y = M_LINE_INIT * CHARACTER_SIZE;						// �G���A��Y���W�i�s�x
	m_areaGroundMoveSum = 0;								// �G���A���C���p
	m_enemyGroundMoveSum = M_ENEMY_GROUND_MOVE_SUM_OFFSET;	// �n��G���C���p 2�h�b�g���Y����
}

void AreaManager::Restart() {
	// �����Ń~�X������̍ŏI���C���ɉ����Ď��̃G���A�ɍs�����ǂ����̏���������

	m_isStart = true;
	m_line = M_LINE_INIT;									// ���C��
	m_groundEnemyLine = M_LINE_INIT;						// �n��G���C��
	m_y = M_LINE_INIT * CHARACTER_SIZE;						// �G���A��Y���W�i�s�x
	m_areaGroundMoveSum = 0;								// �G���A���C���p
	m_enemyGroundMoveSum = M_ENEMY_GROUND_MOVE_SUM_OFFSET;	// �n��G���C���p 2�h�b�g���Y����
}

void AreaManager::Calc(float groundMove) {
	m_y += groundMove;				// �w�i�ړ����x��ݐ�
	m_areaGroundMoveSum += groundMove;
	m_enemyGroundMoveSum += groundMove;

	// �G���A���C������
	if (m_areaGroundMoveSum >= CHARACTER_SIZE) {				// �L�����N�^�T�C�Y�𒴂�����
		m_areaGroundMoveSum -= CHARACTER_SIZE;					// �L�����N�^�T�C�Y�����Z
		m_line++;												// ���C�����C���N�������g
		if (m_line + M_LOAD_LINE_OFFSET == LINE_MAX) {
			m_loadAria++;
		}

		if (m_line >= LINE_MAX || (m_isStart && m_line > M_START_LINE_MAX)) {	// �G���A�I�[�A�܂��͊J���̐X�ŏI�[��������
			if (m_isStart) {									// �J����������
				m_y -= M_START_LINE_MAX * CHARACTER_SIZE;
				m_line -= M_START_LINE_MAX;						// line��1��
			}
			else {
				m_y -= LINE_MAX * CHARACTER_SIZE;				// ���Z�͉��̒l
				m_line -= LINE_MAX;								// line��0��
				//m_aria++;										// ���̃G���A�ց@����������͑Ή����Ȃ�
				if (m_aria >= AREA_NUM) {						// ���Z��̃G���AID��16�ȏゾ������
					m_aria = M_LOOP_AREA_NUM;					// �G���AID6�i�G���A7�j��
				}
			}
		}
	}

	// �n��G���C�������@����̎d�l����255�̈ʒu�ɒn��G�����݂���ꍇ�O�G���A�I�[�ŏo�����Ă��܂��̂Œ��Ӂi����ɂ͑��݂��Ȃ����j
	if (m_enemyGroundMoveSum >= CHARACTER_SIZE) {				// �L�����N�^�T�C�Y�𒴂�����
		m_enemyGroundMoveSum -= CHARACTER_SIZE;					// �L�����N�^�T�C�Y�����Z
		m_groundEnemyLine++;;									// ���C�����C���N�������g
		if (m_groundEnemyLine >= LINE_MAX || (m_isStart && m_groundEnemyLine > M_START_LINE_MAX)) {	// �G���A�I�[�A�܂��͊J���̐X�ŏI�[��������
			if (m_isStart) {									// �J����������
				m_groundEnemyLine -= M_START_LINE_MAX;			// line��1��
				m_isStart = false;								// �J���t���O��false��
			}
			else {
				m_groundEnemyLine -= LINE_MAX;			// line��0��
			}
		}
	}
}

// ���݂̃G���AID��Ԃ��܂��@0�`15�̒l�@�i����ŕ\���͖������j�\����̒l���1���������ɒ���
int AreaManager::GetAria() {
	return m_aria;
}

// ���݂̃G���A���C����Ԃ��܂��@0�`255�̒l
int AreaManager::GetLine() {
	return m_line;
}

// ���݂̃��[�h�p�G���AID��Ԃ��܂��@0�`15�̒l�@�i����ŕ\���͖������j�\����̒l���1���������ɒ���
int AreaManager::GetLoadAria() {
	return m_loadAria;
}

// ���݂̃��[�h�p�G���A���C����Ԃ��܂��@0�`255�̒l
int AreaManager::GetGroundEnemyLine() {
	return m_groundEnemyLine;
}

// ���݂�y���W��Ԃ��܂��@0�`2047�̒l
float AreaManager::GetY() {
	return m_y;
}

// ���݊J�n���̐X���ǂ�����Ԃ��܂��@true�F�J�n���@false�F�ʏ�G���A
bool AreaManager::GetIsStart() {
	return m_isStart;
}

