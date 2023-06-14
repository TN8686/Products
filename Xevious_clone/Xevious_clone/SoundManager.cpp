#include "SoundManager.h"

SoundManager::SoundManager() {

	//BGM�\�[�X�̏�����
	for (int i = 0; i < BGM_SIZE; i++) {
		this->m_BgmSource[i] = 0;
	}

	//SE�\�[�X�̏�����
	for (int i = 0; i < SE_SIZE; i++) {
		this->m_SeSource[i] = 0;
	}

	for (int i = 0; i < NOBGM_SE_SIZE; i++) {
		this->m_SeSource[i] = 0;
	}

	for (int i = 0; i < NOISE_SIZE; i++) {
		this->m_SeSource[i] = 0;
	}

	for (int i = 0; i < SPECIAL_SE_SIZE; i++) {
		this->m_SeSource[i] = 0;
	}

	//�e�T�E���h�̃��[�h
	LoadSoundBgm();
	LoadAllSeSound();

	this->m_BgmHandle = 0;		//BGM�n���h���̏�����
	this->m_SeHandle  = 0;		//SE�n���h���̏�����

}

//1�ڂ�BGM�̈����𔻒f���āA2�ڂ�true�Ȃ�΃��[�v�����łȂ���΃o�b�N�O���E���h�Đ�
//�����Ŏw�肳�ꂽBGM�����[�h���Ă�����m_BgmSource��m_BgmHandle�ɂ���ăo�b�N�O���E���h�ōĐ�����
int SoundManager::BgmPlay(int id, bool bgmSetting) {
	int ret = 0;

	//�X�^�[�gBGM���A�G�N�X�e���h�����Ȃ��Ă���Ԃ͂Ȃ�Ȃ�
	if (!GameStartBgmCheck()) {
		this->m_BgmHandle = this->m_BgmSource[id];

		if (bgmSetting) {
			ret = PlaySoundMem(this->m_BgmHandle, DX_PLAYTYPE_LOOP);
		}
		else {
			ret = PlaySoundMem(this->m_BgmHandle, DX_PLAYTYPE_BACK);
		}

	}
	return ret;
}

//�����Ŏw�肳�ꂽSE�����[�h���Ă�����m_SeSource��m_SeHandle�ɂ���ăo�b�N�O���E���h�ōĐ�����
//ID�u0:�A���h�W�F�l�V�X�o�� 1:�u���X�^�[���� 2:�V�I�i�C�g�h�b�L���O 3:�U�J�[�g�o���v
int SoundManager::SePlay(int id) {
	int ret = 0;

	//�X�^�[�gBGM���A�G�N�X�e���h�����Ȃ��Ă���Ԃ͂Ȃ�Ȃ�
	if (!GameStartBgmCheck()) {

		if (id != 0) {
			//SE���Đ�����Ă����痬��Ă���SE�������ĐV����SE�𗬂�
			if (CheckSoundMem(this->m_SeSource[id]) == 1) {
				StopSoundMem(this->m_SeSource[id]);
			}
			ret = PlaySoundMem(this->m_SeSource[id], DX_PLAYTYPE_BACK);
		}
		else {
			ret = PlaySoundMem(this->m_SeSource[0], DX_PLAYTYPE_LOOP);
		}
	}
	return ret;
}

//BGM��������SE�𗬂��֐�
//ID�u0:�U�b�p�[�q�b�g�� 1:�U�b�p�[�o�L�����q�b�g 2:�X�y�V�����t���b�O 3:�U�b�p�[���ˉ��v
int SoundManager::NoBgmSePlay(int id) {
	int ret = 0;

	//�X�^�[�gBGM���A�G�N�X�e���h�����Ȃ��Ă���Ԃ͂Ȃ�Ȃ�
	if (!GameStartBgmCheck()) {

		//SE���Đ�����Ă����痬��Ă���SE�������ĐV����SE�𗬂�
		if (CheckSoundMem(this->m_NoBgmSeSource[id]) == 1) {
			StopSoundMem(this->m_NoBgmSeSource[id]);
		}
		ret = PlaySoundMem(this->m_NoBgmSeSource[id], DX_PLAYTYPE_BACK);

	}
	return ret;
}

//�����Ŏw�肳�ꂽ�m�C�Y����炷
//NoisePlay�Ŗ炷���̂�BGM�������Ȃ�
//ID�u0:�N���W�b�g�� 1:�n�㕨���j�� 2:�K���U�J�[�g���� 3:�\���o���E���j�v
int SoundManager::NoisePlay(int id) {
	int ret = 0;

	ret = PlaySoundMem(this->m_NoiseSeSource[id], DX_PLAYTYPE_BACK);

	return ret;
}

//����SE�𗬂��֐�
//ID�u0:�G�N�X�e���h�v
void SoundManager::SpecialSePlay() {
	if (!GameStartBgmCheck()) {
		PlaySoundMem(this->m_SpecialSeSource[0], DX_PLAYTYPE_BACK);
	}
}

//���t���[���m�F
void SoundManager::Calc() {
	SeSoundCheck();
}

//m_NoBgmSource�����SE������Ă��邩�`�F�b�N����BGM������
void SoundManager::SeSoundCheck() {
	for (int i = 0; i < NOBGM_SE_SIZE;i++) {
		if (CheckSoundMem(this->m_NoBgmSeSource[i]) == 1) {
			ChangeVolumeSoundMem(0, this->m_BgmHandle);
			break;
		}
		else {
			ChangeVolumeSoundMem(255, this->m_BgmHandle);
		}
	}
}

//�Q�[���X�^�[�g��BGM�ƃG�N�X�e���hBGM���Ȃ��Ă��邩�`�F�b�N����֐�
//�Q�[���X�^�[�g����BGM�ƃG�N�X�e���hBGM���Ȃ��Ă���Ƃ���true��Ԃ��A�����łȂ��Ƃ���false��Ԃ�
bool SoundManager::GameStartBgmCheck() {
	if (CheckSoundMem(this->m_SpecialSeSource[0]) == 1 ||
		CheckSoundMem(this->m_BgmSource[1]) == 1) {

		return true;
	}
	else {
		return false;
	}
}

//BGM��m_BgmSource�̔z��ɑS������Ă���(�z��̔ԍ���BgmId�̔ԍ��͓���)
void SoundManager::LoadSoundBgm() {

	for (int i = 0; i < BGM_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_BgmSource[0] = LoadSoundMem("sound/xev_music_BGM_loop.wav");
			break;
		case 1:
			this->m_BgmSource[1] = LoadSoundMem("sound/xev_music_GameStart.wav");
			break;
		case 2:
			this->m_BgmSource[2] = LoadSoundMem("sound/xev_music_NameEntry_01.wav");
			break;
		case 3:
			this->m_BgmSource[3] = LoadSoundMem("sound/xev_music_NameEntry_02.wav");
			break;
		}
	}
}

//���[�h����ɂ܂Ƃ߂�֐�
void SoundManager::LoadAllSeSound(){
	LoadSoundSe();
	LoadNoBgmSe();
	LoadNoiseSe();
	LoadSpecialSe();
}

//SE��m_SeSource�̔z��ɑS������Ă���(�z��̔ԍ���SeId�̔ԍ��͓���)
void SoundManager::LoadSoundSe() {
	for (int i = 0; i < SE_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_SeSource[i] = LoadSoundMem("sound/xev_se_Andorgenesis_loop.wav");
			break;
		case 1:	
			this->m_SeSource[i] = LoadSoundMem("sound/xev_se_BlasterShoot.wav");
			break;
		case 2:
			this->m_SeSource[i] = LoadSoundMem("sound/xev_se_Shionite_DockingSound.wav");
			break;
		case 3:
			this->m_SeSource[i] = LoadSoundMem("sound/xev_se_Zakato_DethoutSound.wav");
			break;
			
		}
	}
}

//BGM��������SE�̃��[�h�֐�
void SoundManager::LoadNoBgmSe() {
	for (int i = 0; i < LOAD_NOBGM_SE_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_NoBgmSeSource[i] = LoadSoundMem("sound/xev_se_BacuraHitSound_Zapper.wav");
			break;
		case 1:
			this->m_NoBgmSeSource[i] = LoadSoundMem("sound/xev_se_EnemyStriken_Zapper.wav");
			break;
		case 2:
			this->m_NoBgmSeSource[i] = LoadSoundMem("sound/xev_se_SpecialFlag_get.wav");
			break;
		case 3:
			this->m_NoBgmSeSource[i] = LoadSoundMem("sound/xev_se_ZapperShoot.wav");
			break;
		}
	}
}

//�m�C�YSE�����[�h����֐�
void SoundManager::LoadNoiseSe() {
	for (int i = 0; i < LOAD_NOISE_SE_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_NoiseSeSource[i] = LoadSoundMem("sound/xev_se_CreditSound.wav");
			break;
		case 1:
			this->m_NoiseSeSource[i] = LoadSoundMem("sound/xev_se_EnemyStriken_Blaster.wav");
			break;
		case 2:
			this->m_NoiseSeSource[i] = LoadSoundMem("sound/xev_se_GaruZakato_Explosion.wav");
			break;
		case 3:
			this->m_NoiseSeSource[i] = LoadSoundMem("sound/xev_se_Miss.wav");
			break;
		}
	}
}

//�X�y�V����SE�����[�h����֐�
void SoundManager::LoadSpecialSe() {
	for (int i = 0; i < LOAD_SPECIAL_SE_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_SpecialSeSource[i] = LoadSoundMem("sound/xev_se_ExtendSound.wav");
			break;
		}
	}
}

//BGM���~�߂�
void SoundManager::BgmStop() {
	StopSoundMem(m_BgmHandle);
}

//�A���h�W�F�l�V�X�̉����~�߂�
void SoundManager::AndorgenesisSeStop() {
	StopSoundMem(m_SeSource[0]);
}
