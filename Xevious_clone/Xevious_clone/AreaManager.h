#pragma once
#include "Xevious_clone.h"

class AreaManager
{
public:
	AreaManager();
	void Init();
	void Restart();

	void Calc(float groundMove);

	// �Q�b�^�[
	int GetAria();
	int GetLine();
	int GetGroundEnemyLine();
	int GetLoadAria();
	float GetY();
	bool GetIsStart();

	static constexpr int AREA_NUM = 16;							// �G���A��
	static constexpr int LINE_MAX = 256;						// 1�G���A������̃��C����
	static constexpr int AREA_CHARACTER_WIDTH = 28;				// �G���A�̃L�����N�^�[�P�ʂ̕�

private:
	static constexpr int M_START_LINE_MAX = 49;					// �J�n���̐X�̃��C�����@LINE_MAX�̒l���炷���50�����������A����̋������l�����49�̕������R
	static constexpr int M_LOOP_AREA_NUM = 6;					// �ŏI�G���A�𔲂������̃��[�v��̃G���AID
	static constexpr int M_LINE_INIT = 39;						// �X�^�[�g���̃��C��
	static constexpr int M_LOAD_LINE_OFFSET = 12;				// ���[�h�p���C���I�t�Z�b�g
	static constexpr int M_ENEMY_GROUND_MOVE_SUM_OFFSET = -2;	// �n��G�p�w�i�ړ��݌v�I�t�Z�b�g

	bool m_isStart;					// �J�n���̐X�t���O
	int m_aria;						// ���݂̃G���A
	int m_loadAria;					// ���[�h����G���A
	int m_line;						// ���݂̃��C��
	int m_groundEnemyLine;			// �n��G�p���C��
	float m_y;						// �G���AY���W�i�s�x
	float m_areaGroundMoveSum;		// �G���A�p�n��ړ��ݐ�
	float m_enemyGroundMoveSum;		// �n��G�p�n��ړ��ݐ�
};