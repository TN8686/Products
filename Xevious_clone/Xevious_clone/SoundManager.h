#pragma once

#include "Xevious_clone.h"
#define BGM_SIZE   4		//���[�h�p��BGM�̍ő吔
#define LOAD_NOBGM_SE_SIZE 4	//���[�h�p��BGM��������SE�̍ő吔
#define LOAD_NOISE_SE_SIZE 4	//���[�h�p�̃m�C�YSE�̍ő吔
#define LOAD_SPECIAL_SE_SIZE 1	//���[�h�p�̃X�y�V����SE�̍ő吔
#define SE_SIZE    4		//SE�̍ő吔(���[�h�ɂ��g��)
#define NOBGM_SE_SIZE 4		//BGM��������SE�̍ő吔
#define NOISE_SIZE 4		//�m�C�Y�̍ő吔
#define SPECIAL_SE_SIZE 1	//����SE�̍ő吔

//BGM���X�g(�ォ��ԍ���)
enum class BgmId : int {
	BGM_loop,
	Game_Start,
	Name_Entry_01,
	Name_Entry_02,
};

//SE���X�g(�ォ��ԍ���)
enum class SeId : int {
	Andorgenesis_Loop,
	//BacuraHitSound_Zapper,
	BlasterShoot,
	//CreditSound,
	//EnemyStriken_Blaster,
	//EnemyStriken_Zapper,
	//ExtendSound,
	//GaruZakato_Explosion,
	//Miss,
	Shionite_DockingSound,
	//SpecialFlag_get,
	Zakato_DethoutSound,
	//ZapperShoot,
};

enum class NoBgmSeId : int {
	BacuraHitSound_Zapper,
	EnemyStriken_Zapper,
	SpecialFlag_get,
	ZapperShoot,
};

enum class NoiseSeId : int {
	CreditSound,
	EnemyStriken_Blaster,
	GaruZakato_Explosion,
	Miss,
};

enum class SpecialSeId : int {
	ExtendSound,
};

class SoundManager
{
private:
	int m_BgmHandle;		//BGM�p�̈���
	int m_SeHandle;			//SE�p�̈���
	int m_NoiseHandle;		//NOISE�p�̈���

	int m_BgmSource[BGM_SIZE];				//BGM�̃��[�h�p
	int m_SeSource[SE_SIZE];				//SE�̃��[�h�p
	int m_NoBgmSeSource[NOBGM_SE_SIZE];		//BGM��������SE�̃��[�h�p
	int m_NoiseSeSource[NOISE_SIZE];		//�m�C�Y�̃��[�h�p
	int m_SpecialSeSource[SPECIAL_SE_SIZE];	//����SE�̃��[�h�p

	//BGM���������Ƀ��[�h����֐�
	void LoadSoundBgm();

	//SE���������Ƀ��[�h����֐�
	void LoadSoundSe();

	//BGM���Ȃ�Ȃ�SE�����[�h����֐�
	void LoadNoBgmSe();

	//�m�C�Y�����[�h����֐�
	void LoadNoiseSe();

	//�X�y�V����SE�����[�h����֐�
	void LoadSpecialSe();

	//SE���[�h���܂Ƃ߂�֐�
	void LoadAllSeSound();

	//SE���Ȃ��Ă��邩�`�F�b�N���āABGM�������֐�
	void SeSoundCheck();

public:
	SoundManager();

	//���t���[���`�F�b�N�p�֐�
	void Calc();

	//BGM��炷�֐�
	//BgmId�̂ق��̓w�b�_�[�Ŕԍ����m�F�BBgmSetting�̕���false�����ʁAtrue�����[�v
	//���[�v���Ȃ��ꍇ����false�w��
	int BgmPlay(int id, bool bgmSetting);

	//SE��炷�BSeId�̈�����SE���w��
	int SePlay(int id);

	//BGM��������SE��炷�B������SE���w��
	int NoBgmSePlay(int id);

	//�m�C�Y����炷�֐�
	int NoisePlay(int id);

	//�����SE��炷�֐�
	void SpecialSePlay();

	//�Đ�����BGM���~�߂�֐�
	void BgmStop();

	//�A���h�W�F�l�V�X��SE���[�v���~�߂�֐�
	void AndorgenesisSeStop();

	//�Q�[���X�^�[�g����BGM�A�G�N�X�e���hBGM���Ȃ��Ă��邩�`�F�b�N����֐�
	bool GameStartBgmCheck();

};