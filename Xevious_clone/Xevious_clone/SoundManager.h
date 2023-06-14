#pragma once

#include "Xevious_clone.h"
#define BGM_SIZE   4		//ロード用のBGMの最大数
#define LOAD_NOBGM_SE_SIZE 4	//ロード用のBGMが消えるSEの最大数
#define LOAD_NOISE_SE_SIZE 4	//ロード用のノイズSEの最大数
#define LOAD_SPECIAL_SE_SIZE 1	//ロード用のスペシャルSEの最大数
#define SE_SIZE    4		//SEの最大数(ロードにも使う)
#define NOBGM_SE_SIZE 4		//BGMが消えるSEの最大数
#define NOISE_SIZE 4		//ノイズの最大数
#define SPECIAL_SE_SIZE 1	//特殊SEの最大数

//BGMリスト(上から番号順)
enum class BgmId : int {
	BGM_loop,
	Game_Start,
	Name_Entry_01,
	Name_Entry_02,
};

//SEリスト(上から番号順)
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
	int m_BgmHandle;		//BGM用の引数
	int m_SeHandle;			//SE用の引数
	int m_NoiseHandle;		//NOISE用の引数

	int m_BgmSource[BGM_SIZE];				//BGMのロード用
	int m_SeSource[SE_SIZE];				//SEのロード用
	int m_NoBgmSeSource[NOBGM_SE_SIZE];		//BGMが消えるSEのロード用
	int m_NoiseSeSource[NOISE_SIZE];		//ノイズのロード用
	int m_SpecialSeSource[SPECIAL_SE_SIZE];	//特殊SEのロード用

	//BGMをメモリにロードする関数
	void LoadSoundBgm();

	//SEをメモリにロードする関数
	void LoadSoundSe();

	//BGMがならないSEをロードする関数
	void LoadNoBgmSe();

	//ノイズをロードする関数
	void LoadNoiseSe();

	//スペシャルSEをロードする関数
	void LoadSpecialSe();

	//SEロードをまとめる関数
	void LoadAllSeSound();

	//SEがなっているかチェックして、BGMを消す関数
	void SeSoundCheck();

public:
	SoundManager();

	//毎フレームチェック用関数
	void Calc();

	//BGMを鳴らす関数
	//BgmIdのほうはヘッダーで番号を確認。BgmSettingの方はfalseが普通、trueがループ
	//ループしない場合だけfalse指定
	int BgmPlay(int id, bool bgmSetting);

	//SEを鳴らす。SeIdの引数でSEを指定
	int SePlay(int id);

	//BGMが消えるSEを鳴らす。引数でSEを指定
	int NoBgmSePlay(int id);

	//ノイズ等を鳴らす関数
	int NoisePlay(int id);

	//特殊なSEを鳴らす関数
	void SpecialSePlay();

	//再生中のBGMを止める関数
	void BgmStop();

	//アンドジェネシスのSEループを止める関数
	void AndorgenesisSeStop();

	//ゲームスタート時のBGM、エクステンドBGMがなっているかチェックする関数
	bool GameStartBgmCheck();

};