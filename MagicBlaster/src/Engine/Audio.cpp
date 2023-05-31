#include "Audio.h"
#include <xaudio2.h>
#include <stdint.h>
#include <vector>
#include <wrl/client.h>
#include <list>

using Microsoft::WRL::ComPtr;

/**
* RIFFチャンク
*/
struct RIFFChunk
{
	uint32_t tag; ///< 識別子.
	uint32_t size; ///< チャンクデータのバイト数.
};

// PCM WAVフォーマットファイルで出現する識別子.
const uint32_t FOURCC_RIFF_TAG = MAKEFOURCC('R', 'I', 'F', 'F');
const uint32_t FOURCC_FORMAT_TAG = MAKEFOURCC('f', 'm', 't', ' ');
const uint32_t FOURCC_DATA_TAG = MAKEFOURCC('d', 'a', 't', 'a');
const uint32_t FOURCC_WAVE_FILE_TAG = MAKEFOURCC('W', 'A', 'V', 'E');

/**
* ファイルを読み込む.
*
* @param hFile ファイルハンドル.
* @param buf   読み込みバッファへのポインタ.
* @param size  読み込むバイト数.
*
* @retval true  読み込み成功.
* @retval false 読み込み失敗.
*/
bool Read(HANDLE hFile, void* buf, DWORD size)
{
	DWORD readSize;
	if (!ReadFile(hFile, buf, size, &readSize, nullptr) || readSize != size) {
		return false;
	}
	return true;
}

/**
* 最小限のスマートファイルハンドル.
* ファイルハンドルの代わりとして使うことができ、.
* スコープを抜けるときには自動的にCloseHandleを呼ぶだけ.
*/
struct ScopedHandle
{
	ScopedHandle(HANDLE h) : handle(h == INVALID_HANDLE_VALUE ? 0 : h) {}
	~ScopedHandle() { if (handle) { CloseHandle(handle); } }
	operator HANDLE() { return handle; }
	HANDLE handle;
};

/**
* WAVファイルを読み込む.
*
* @param filename WAVファイル名.
* @param wfx      読み込み先WAFフォーマット構造体へのポインタ.
* @param data     読み込んだデータを格納するバッファへのポインタ.
*
* @retval true  読み込み成功.
* @retval false 読み込み失敗.
*/
bool LoadWaveFile(
	const wchar_t* filename, WAVEFORMATEX* wfx, std::vector<uint8_t>* data)
{
	// ファイルオープン.
	ScopedHandle hFile = CreateFile2(
		filename, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	if (!hFile) {
		return false;
	}
	RIFFChunk riffChunk;
	if (!Read(hFile, &riffChunk, sizeof(riffChunk))) {
		return false;
	}
	if (riffChunk.tag != FOURCC_RIFF_TAG) {
		return false;
	}
	uint32_t fourcc;
	if (!Read(hFile, &fourcc, sizeof(fourcc))) {
		return false;
	}
	if (fourcc != FOURCC_WAVE_FILE_TAG) {
		return false;
	}

	bool hasWaveFormat = false;
	bool hasData = false;
	size_t offset = 12;	// RIFFチャンク8バイト + 識別子4バイト.
	do {
		if (SetFilePointer(hFile, offset, nullptr, FILE_BEGIN) != offset) {
			break;
		}

		RIFFChunk chunk;
		if (!Read(hFile, &chunk, sizeof(chunk))) {
			break;
		}

		if (chunk.tag == FOURCC_FORMAT_TAG) {
			if (!Read(hFile, wfx, sizeof(WAVEFORMATEX))) {
				break;
			}
			if (wfx->wFormatTag == WAVE_FORMAT_PCM) {
				wfx->cbSize = 0;
			}
			hasWaveFormat = true;
		}
		else if (chunk.tag == FOURCC_DATA_TAG) {
			data->resize(chunk.size);
			if (!Read(hFile, data->data(), chunk.size)) {
				return false;
			}
			hasData = true;
		}
		offset += chunk.size + sizeof(RIFFChunk);
	} while (!hasWaveFormat || !hasData);
	return hasWaveFormat && hasData;


}

/**
* Soundの実装クラス.
*/
class SoundImpl : public Sound
{
public:
	SoundImpl() = default;
	virtual ~SoundImpl() override;
	virtual void Play() override;
	virtual void Pause()override;
	virtual void Stop()override;
	virtual void SetVolume(float) override;
	virtual void SetPitch(float) override;
	virtual bool IsFinished() const override;
	virtual bool IsPlaying() const override;
	
	virtual void Seek(int) override;

	IXAudio2SourceVoice* sourceVoice = nullptr;
	std::vector<uint8_t> data;
	bool start = false;
	bool pause = false;

	int m_begin = 0; // 再生開始地点.
};

/**
* デストラクタ.
*/
SoundImpl::~SoundImpl()
{
	if (sourceVoice) {
		sourceVoice->DestroyVoice();
	}
}

/**
* 再生.
*/
void SoundImpl::Play()
{
	if (!pause) {
		Stop();
	}
	start = true;
	XAUDIO2_BUFFER buffer = {};
	buffer.Flags = XAUDIO2_END_OF_STREAM;	// 再生が終わったら停止.
	buffer.AudioBytes = data.size();
	buffer.pAudioData = data.data();
	buffer.PlayBegin = m_begin;
	sourceVoice->SubmitSourceBuffer(&buffer);	// 音声データをXAudio2に送る.
	sourceVoice->Start();
}

/**
* 一時停止.
*/
void SoundImpl::Pause()
{
	if (start && !pause) {
		pause = true;
		sourceVoice->Stop();
	}
}

/**
* 停止.
*/
void SoundImpl::Stop()
{
	if (start) {
		start = false;
		pause = false;
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();	// 送った音声データの削除.
	}
}

/**
* 音量を設定する.
*
* @param volume 音量(0=無音 1=増減なし 1以上=音量増加).
*/
void SoundImpl::SetVolume(float volume)
{
	sourceVoice->SetVolume(volume);
}

/**
* 音程を設定する.
*
* @param pitch ピッチ(1未満=音程を下げる 1=変化なし 1以上=音程を上げる).
*/
void SoundImpl::SetPitch(float pitch)
{
	sourceVoice->SetFrequencyRatio(pitch);
}

/**
* 再生完了を調べる.
*
* @retval true  再生完了.
* @retval false 再生中.
*/
bool SoundImpl::IsFinished() const
{
	XAUDIO2_VOICE_STATE s;
	sourceVoice->GetState(&s);
	return s.BuffersQueued == 0;
}

/**
* 再生状態を取得する.
*
* @retval true  再生中.
* @retval false 停止または一時停止している.
*/
bool SoundImpl::IsPlaying() const
{
	if (!start || pause) {
		return false;
	}
	return IsFinished();
}

void SoundImpl::Seek(int begin) {	// Seek関数を自作してみる.
	m_begin = begin;	// 再生開始位置を持っておき、Play関数での再生前の設定に使用する.
}

/**
* Engineの実装.
*/
class EngineImpl : public AudioEngine
{
public:
	virtual bool Initialize() override;
	virtual void Destroy() override;
	virtual SoundPtr Prepare(const wchar_t* filename) override;
	virtual void Update() override;
	virtual void SetMasterVolume(float vol) override;
private:
	ComPtr<IXAudio2> xaudio;	// XAudio2オブジェクトのインターフェイス.
	IXAudio2MasteringVoice* masteringVoice;	// マスターヴォイスのインターフェイス.
	typedef std::list<std::shared_ptr<SoundImpl>> SoundList;
	SoundList soundList;
};

/**
* XAudio2の初期化.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool EngineImpl::Initialize()
{
	// XAudio2オブジェクトの作成.
	ComPtr<IXAudio2> tmpAudio;
	if (FAILED(XAudio2Create(&tmpAudio))) {
		return false;
	}

	// マスターヴォイスの作成.
	if (FAILED(tmpAudio->CreateMasteringVoice(&masteringVoice))) {
		return false;
	}
	xaudio = tmpAudio;
	return true;
}

/**
* XAudio2の破棄.
*/
void EngineImpl::Destroy()
{
	soundList.clear();
	xaudio.Reset();
}

/**
* 音声の準備.
*
* @param filename 読み込む音声ファイル名.
*
* @retval nullptr以外 音声制御オブジェクトへのポインタ.
* @retval nullptr     読み込み失敗.
*/
SoundPtr EngineImpl::Prepare(const wchar_t* filename)
{
	WAVEFORMATEX wfx;
	std::shared_ptr<SoundImpl> sound(new SoundImpl);
	if (!LoadWaveFile(filename, &wfx, &sound->data)) {
		return nullptr;
	}
	if (FAILED(xaudio->CreateSourceVoice(&sound->sourceVoice, &wfx))) {
		return nullptr;
	}
	soundList.push_back(sound);
	return sound;
}

/**
* エンジン状態の更新.
*/
void EngineImpl::Update()
{
	// オーディオエンジン以外に所有者がいない場合は、音声の再生が終了していればそれを削除.
	// サンプルはIsFinished関数の戻り値が間違っていたので再生中だと削除するようになっていた。修正済.
	soundList.remove_if([](const SoundList::value_type& e) {return (e.use_count() <= 1) && e->IsFinished(); });
}

/**
* マスターボリュームを設定する.
*
* @param volume 音量(0=無音 1=増減なし 1以上=音量増加).
*/
void EngineImpl::SetMasterVolume(float volume)
{
	masteringVoice->SetVolume(volume);
}

/**
* オーディオエンジンを取得する.
*
* @return オーディオエンジンへの参照
*/
AudioEngine& AudioEngine::Get()
{
	static std::unique_ptr<EngineImpl> engine(new EngineImpl);
	return *engine;
}
