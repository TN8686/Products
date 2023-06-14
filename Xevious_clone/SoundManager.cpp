#include "SoundManager.h"

SoundManager::SoundManager() {

	//BGMソースの初期化
	for (int i = 0; i < BGM_SIZE; i++) {
		this->m_BgmSource[i] = 0;
	}

	//SEソースの初期化
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

	//各サウンドのロード
	LoadSoundBgm();
	LoadAllSeSound();

	this->m_BgmHandle = 0;		//BGMハンドルの初期化
	this->m_SeHandle  = 0;		//SEハンドルの初期化

}

//1つ目でBGMの引数を判断して、2つ目でtrueならばループそうでなければバックグラウンド再生
//引数で指定されたBGMをロードしておいたm_BgmSourceをm_BgmHandleにいれてバックグラウンドで再生する
int SoundManager::BgmPlay(int id, bool bgmSetting) {
	int ret = 0;

	//スタートBGMか、エクステンド音がなっている間はならない
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

//引数で指定されたSEをロードしておいたm_SeSourceをm_SeHandleにいれてバックグラウンドで再生する
//ID「0:アンドジェネシス出現 1:ブラスター発射 2:シオナイトドッキング 3:ザカート出現」
int SoundManager::SePlay(int id) {
	int ret = 0;

	//スタートBGMか、エクステンド音がなっている間はならない
	if (!GameStartBgmCheck()) {

		if (id != 0) {
			//SEが再生されていたら流れているSEを消して新たにSEを流す
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

//BGMが消えるSEを流す関数
//ID「0:ザッパーヒット音 1:ザッパーバキュラヒット 2:スペシャルフラッグ 3:ザッパー発射音」
int SoundManager::NoBgmSePlay(int id) {
	int ret = 0;

	//スタートBGMか、エクステンド音がなっている間はならない
	if (!GameStartBgmCheck()) {

		//SEが再生されていたら流れているSEを消して新たにSEを流す
		if (CheckSoundMem(this->m_NoBgmSeSource[id]) == 1) {
			StopSoundMem(this->m_NoBgmSeSource[id]);
		}
		ret = PlaySoundMem(this->m_NoBgmSeSource[id], DX_PLAYTYPE_BACK);

	}
	return ret;
}

//引数で指定されたノイズ等を鳴らす
//NoisePlayで鳴らすものはBGMが消えない
//ID「0:クレジット音 1:地上物爆破音 2:ガルザカート自爆 3:ソルバルウ爆破」
int SoundManager::NoisePlay(int id) {
	int ret = 0;

	ret = PlaySoundMem(this->m_NoiseSeSource[id], DX_PLAYTYPE_BACK);

	return ret;
}

//特殊SEを流す関数
//ID「0:エクステンド」
void SoundManager::SpecialSePlay() {
	if (!GameStartBgmCheck()) {
		PlaySoundMem(this->m_SpecialSeSource[0], DX_PLAYTYPE_BACK);
	}
}

//毎フレーム確認
void SoundManager::Calc() {
	SeSoundCheck();
}

//m_NoBgmSourceからのSEが流れているかチェックしてBGMを消す
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

//ゲームスタートのBGMとエクステンドBGMがなっているかチェックする関数
//ゲームスタート時のBGMとエクステンドBGMがなっているときはtrueを返し、そうでないときはfalseを返す
bool SoundManager::GameStartBgmCheck() {
	if (CheckSoundMem(this->m_SpecialSeSource[0]) == 1 ||
		CheckSoundMem(this->m_BgmSource[1]) == 1) {

		return true;
	}
	else {
		return false;
	}
}

//BGMをm_BgmSourceの配列に全部入れている(配列の番号とBgmIdの番号は同じ)
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

//ロードを一つにまとめる関数
void SoundManager::LoadAllSeSound(){
	LoadSoundSe();
	LoadNoBgmSe();
	LoadNoiseSe();
	LoadSpecialSe();
}

//SEをm_SeSourceの配列に全部入れている(配列の番号とSeIdの番号は同じ)
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

//BGMが消えるSEのロード関数
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

//ノイズSEをロードする関数
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

//スペシャルSEをロードする関数
void SoundManager::LoadSpecialSe() {
	for (int i = 0; i < LOAD_SPECIAL_SE_SIZE; i++) {
		switch (i) {
		case 0:
			this->m_SpecialSeSource[i] = LoadSoundMem("sound/xev_se_ExtendSound.wav");
			break;
		}
	}
}

//BGMを止める
void SoundManager::BgmStop() {
	StopSoundMem(m_BgmHandle);
}

//アンドジェネシスの音を止める
void SoundManager::AndorgenesisSeStop() {
	StopSoundMem(m_SeSource[0]);
}
