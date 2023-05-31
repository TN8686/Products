#pragma once
#include <memory>
#include <vector>
#include <string>

/**
* �V�[���̑J�ڕ��@.
*/
enum class TransitionType
{
	Jump, ///< ���݂̃V�[�����I�����Ď��̃V�[���ɑJ��.
	Push, ///< ���݂̃V�[�����X�^�b�N�ɐς�ł��玟�̃V�[���ɑJ��.
	Pop, ///< ���݂̃V�[�����X�^�b�N����~�낵�đO�̃V�[���ɑJ��.
};

/**
* �V�[���̑J�ڏ��.
*/
struct Transition
{
	int currentScene; ///< �J�ڌ��V�[��.
	struct {
		int exitCode; ///< ���̑J�ڂɑΉ�����I���R�[�h.
		TransitionType type; ///< �J�ڕ��@.
		int nextScene; ///< �J�ڐ�V�[��.
	} trans; ///< �J�ڐ���.
};

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

/**
* �V�[���쐬���.
*/
struct SceneCreator
{
	typedef ScenePtr(*Func)();

	int id; ///< �V�[��ID.
	Func func; ///< �쐬�֐��ւ̃|�C���^.
};

/**
* �V�[���J�ڂ𐧌䂷��N���X.
*
* -# Initialize()�ŃV�[���̑J�ڏ��ƍ쐬����ݒ肷��.
* -# Start()�ōŏ��̃V�[����ݒ肷��.
* -# ���t���[��Update()���Ăяo��.
* -# �K�v�ɉ�����Draw()���Ăяo��.
*/
class TransitionController
{
public:
	bool Initialize(
		const Transition* transitionList, size_t transitionCount,
		const SceneCreator* creatorList, size_t creatorCount);
	bool Start(int startSceneId);
	void Stop();
	bool Update(double delta);
	void Draw() const;

private:
	const SceneCreator* FindCreator(int) const;
	void LoadScene(const SceneCreator*);
	void UnloadScene();

	struct SceneInfo {
		int id;
		ScenePtr p;
	};

	std::vector<SceneCreator> creatorMap;
	std::vector<Transition> transitionMap;
	std::vector<SceneInfo> sceneStack;
};

/**
* ��ʂ��邢�͏�Ԃ�\���N���X.
*/
class Scene
{
	friend class TransitionController;

public:
	/// �V�[���̏��.
	enum class StatusCode {
		Loading, ///< �V�[���̏���������.
		Runnable, ///< �������������Ď��s�\�ȏ��.
		Unloading, ///< �V�[���̏I��������.
		Stopped, ///< �V�[���͏I�����Ă���.
	};

	/**
	* Update()�̖߂�l.
	* �h���N���X�œƎ��̏I���R�[�h���`����ꍇ�AExitCode_User�ȏ�̒l�����蓖�Ă邱��.
	*/
	enum ExitCode {
		ExitCode_Continue = -2, ///< ���݂̃V�[�����p��.
		ExitCode_Exit = -1, ///< ���݂̃V�[�����I��.
		ExitCode_User = 0, ///< ��������h���N���X�p�̏I���R�[�h.
	};

	explicit Scene(const wchar_t* s) : status(StatusCode::Loading), name(s) {}
	virtual ~Scene() {}
	StatusCode GetState() const { return status; }

private:
	virtual int Update(double) = 0;
	virtual void Draw() const = 0;
	virtual bool Load() { return true; }
	virtual bool Unload() { return true; }
	virtual void Pause() {}
	virtual void Resume() {}
	virtual void UpdateForPause(double) {}

private:
	StatusCode status;
	std::wstring name;
};
