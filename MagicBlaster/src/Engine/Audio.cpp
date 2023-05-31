#include "Audio.h"
#include <xaudio2.h>
#include <stdint.h>
#include <vector>
#include <wrl/client.h>
#include <list>

using Microsoft::WRL::ComPtr;

/**
* RIFF�`�����N
*/
struct RIFFChunk
{
	uint32_t tag; ///< ���ʎq.
	uint32_t size; ///< �`�����N�f�[�^�̃o�C�g��.
};

// PCM WAV�t�H�[�}�b�g�t�@�C���ŏo�����鎯�ʎq.
const uint32_t FOURCC_RIFF_TAG = MAKEFOURCC('R', 'I', 'F', 'F');
const uint32_t FOURCC_FORMAT_TAG = MAKEFOURCC('f', 'm', 't', ' ');
const uint32_t FOURCC_DATA_TAG = MAKEFOURCC('d', 'a', 't', 'a');
const uint32_t FOURCC_WAVE_FILE_TAG = MAKEFOURCC('W', 'A', 'V', 'E');

/**
* �t�@�C����ǂݍ���.
*
* @param hFile �t�@�C���n���h��.
* @param buf   �ǂݍ��݃o�b�t�@�ւ̃|�C���^.
* @param size  �ǂݍ��ރo�C�g��.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
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
* �ŏ����̃X�}�[�g�t�@�C���n���h��.
* �t�@�C���n���h���̑���Ƃ��Ďg�����Ƃ��ł��A.
* �X�R�[�v�𔲂���Ƃ��ɂ͎����I��CloseHandle���ĂԂ���.
*/
struct ScopedHandle
{
	ScopedHandle(HANDLE h) : handle(h == INVALID_HANDLE_VALUE ? 0 : h) {}
	~ScopedHandle() { if (handle) { CloseHandle(handle); } }
	operator HANDLE() { return handle; }
	HANDLE handle;
};

/**
* WAV�t�@�C����ǂݍ���.
*
* @param filename WAV�t�@�C����.
* @param wfx      �ǂݍ��ݐ�WAF�t�H�[�}�b�g�\���̂ւ̃|�C���^.
* @param data     �ǂݍ��񂾃f�[�^���i�[����o�b�t�@�ւ̃|�C���^.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool LoadWaveFile(
	const wchar_t* filename, WAVEFORMATEX* wfx, std::vector<uint8_t>* data)
{
	// �t�@�C���I�[�v��.
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
	size_t offset = 12;	// RIFF�`�����N8�o�C�g + ���ʎq4�o�C�g.
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
* Sound�̎����N���X.
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

	int m_begin = 0; // �Đ��J�n�n�_.
};

/**
* �f�X�g���N�^.
*/
SoundImpl::~SoundImpl()
{
	if (sourceVoice) {
		sourceVoice->DestroyVoice();
	}
}

/**
* �Đ�.
*/
void SoundImpl::Play()
{
	if (!pause) {
		Stop();
	}
	start = true;
	XAUDIO2_BUFFER buffer = {};
	buffer.Flags = XAUDIO2_END_OF_STREAM;	// �Đ����I��������~.
	buffer.AudioBytes = data.size();
	buffer.pAudioData = data.data();
	buffer.PlayBegin = m_begin;
	sourceVoice->SubmitSourceBuffer(&buffer);	// �����f�[�^��XAudio2�ɑ���.
	sourceVoice->Start();
}

/**
* �ꎞ��~.
*/
void SoundImpl::Pause()
{
	if (start && !pause) {
		pause = true;
		sourceVoice->Stop();
	}
}

/**
* ��~.
*/
void SoundImpl::Stop()
{
	if (start) {
		start = false;
		pause = false;
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();	// �����������f�[�^�̍폜.
	}
}

/**
* ���ʂ�ݒ肷��.
*
* @param volume ����(0=���� 1=�����Ȃ� 1�ȏ�=���ʑ���).
*/
void SoundImpl::SetVolume(float volume)
{
	sourceVoice->SetVolume(volume);
}

/**
* ������ݒ肷��.
*
* @param pitch �s�b�`(1����=������������ 1=�ω��Ȃ� 1�ȏ�=�������グ��).
*/
void SoundImpl::SetPitch(float pitch)
{
	sourceVoice->SetFrequencyRatio(pitch);
}

/**
* �Đ������𒲂ׂ�.
*
* @retval true  �Đ�����.
* @retval false �Đ���.
*/
bool SoundImpl::IsFinished() const
{
	XAUDIO2_VOICE_STATE s;
	sourceVoice->GetState(&s);
	return s.BuffersQueued == 0;
}

/**
* �Đ���Ԃ��擾����.
*
* @retval true  �Đ���.
* @retval false ��~�܂��͈ꎞ��~���Ă���.
*/
bool SoundImpl::IsPlaying() const
{
	if (!start || pause) {
		return false;
	}
	return IsFinished();
}

void SoundImpl::Seek(int begin) {	// Seek�֐������삵�Ă݂�.
	m_begin = begin;	// �Đ��J�n�ʒu�������Ă����APlay�֐��ł̍Đ��O�̐ݒ�Ɏg�p����.
}

/**
* Engine�̎���.
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
	ComPtr<IXAudio2> xaudio;	// XAudio2�I�u�W�F�N�g�̃C���^�[�t�F�C�X.
	IXAudio2MasteringVoice* masteringVoice;	// �}�X�^�[���H�C�X�̃C���^�[�t�F�C�X.
	typedef std::list<std::shared_ptr<SoundImpl>> SoundList;
	SoundList soundList;
};

/**
* XAudio2�̏�����.
*
* @retval true  ����������.
* @retval false ���������s.
*/
bool EngineImpl::Initialize()
{
	// XAudio2�I�u�W�F�N�g�̍쐬.
	ComPtr<IXAudio2> tmpAudio;
	if (FAILED(XAudio2Create(&tmpAudio))) {
		return false;
	}

	// �}�X�^�[���H�C�X�̍쐬.
	if (FAILED(tmpAudio->CreateMasteringVoice(&masteringVoice))) {
		return false;
	}
	xaudio = tmpAudio;
	return true;
}

/**
* XAudio2�̔j��.
*/
void EngineImpl::Destroy()
{
	soundList.clear();
	xaudio.Reset();
}

/**
* �����̏���.
*
* @param filename �ǂݍ��މ����t�@�C����.
*
* @retval nullptr�ȊO ��������I�u�W�F�N�g�ւ̃|�C���^.
* @retval nullptr     �ǂݍ��ݎ��s.
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
* �G���W����Ԃ̍X�V.
*/
void EngineImpl::Update()
{
	// �I�[�f�B�I�G���W���ȊO�ɏ��L�҂����Ȃ��ꍇ�́A�����̍Đ����I�����Ă���΂�����폜.
	// �T���v����IsFinished�֐��̖߂�l���Ԉ���Ă����̂ōĐ������ƍ폜����悤�ɂȂ��Ă����B�C����.
	soundList.remove_if([](const SoundList::value_type& e) {return (e.use_count() <= 1) && e->IsFinished(); });
}

/**
* �}�X�^�[�{�����[����ݒ肷��.
*
* @param volume ����(0=���� 1=�����Ȃ� 1�ȏ�=���ʑ���).
*/
void EngineImpl::SetMasterVolume(float volume)
{
	masteringVoice->SetVolume(volume);
}

/**
* �I�[�f�B�I�G���W�����擾����.
*
* @return �I�[�f�B�I�G���W���ւ̎Q��
*/
AudioEngine& AudioEngine::Get()
{
	static std::unique_ptr<EngineImpl> engine(new EngineImpl);
	return *engine;
}
