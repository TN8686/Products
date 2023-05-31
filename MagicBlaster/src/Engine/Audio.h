#pragma once
#include <memory>

/**
* �T�E���h����N���X.
*/
class Sound
{
public:
	Sound() = default;
	virtual ~Sound() = default;
	virtual void Play() = 0;
	virtual void Pause() = 0;
	virtual void Stop() = 0;
	virtual void SetVolume(float) = 0;
	virtual void SetPitch(float) = 0;
	virtual bool IsFinished() const = 0;
	virtual bool IsPlaying() const = 0;

	virtual void Seek(int) = 0;	// Seek�֐������삵�Ă݂�.
private:
	Sound(const Sound&) = delete;
	Sound& operator=(const Sound&) = delete;
};
typedef std::shared_ptr<Sound> SoundPtr;	// shared_ptr<Sound>��SoundPtr�Ƃ��čĒ�`

/**
* �T�E���h�Ǘ��N���X.
*/
class AudioEngine
{
public:
	static AudioEngine& Get();
	AudioEngine() = default;
	virtual ~AudioEngine() = default;
	virtual bool Initialize() = 0;
	virtual void Destroy() = 0;
	virtual SoundPtr Prepare(const wchar_t*) = 0;
	virtual void Update() = 0;
	virtual void SetMasterVolume(float) = 0;
private:
	AudioEngine(const AudioEngine&) = delete;
	AudioEngine& operator=(const AudioEngine&) = delete;
};
