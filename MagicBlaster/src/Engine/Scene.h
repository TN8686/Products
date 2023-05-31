#pragma once
#include <memory>
#include <vector>
#include <string>

/**
* シーンの遷移方法.
*/
enum class TransitionType
{
	Jump, ///< 現在のシーンを終了して次のシーンに遷移.
	Push, ///< 現在のシーンをスタックに積んでから次のシーンに遷移.
	Pop, ///< 現在のシーンをスタックから降ろして前のシーンに遷移.
};

/**
* シーンの遷移情報.
*/
struct Transition
{
	int currentScene; ///< 遷移元シーン.
	struct {
		int exitCode; ///< この遷移に対応する終了コード.
		TransitionType type; ///< 遷移方法.
		int nextScene; ///< 遷移先シーン.
	} trans; ///< 遷移先情報.
};

class Scene;
typedef std::shared_ptr<Scene> ScenePtr;

/**
* シーン作成情報.
*/
struct SceneCreator
{
	typedef ScenePtr(*Func)();

	int id; ///< シーンID.
	Func func; ///< 作成関数へのポインタ.
};

/**
* シーン遷移を制御するクラス.
*
* -# Initialize()でシーンの遷移情報と作成情報を設定する.
* -# Start()で最初のシーンを設定する.
* -# 毎フレームUpdate()を呼び出す.
* -# 必要に応じてDraw()を呼び出す.
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
* 画面あるいは状態を表すクラス.
*/
class Scene
{
	friend class TransitionController;

public:
	/// シーンの状態.
	enum class StatusCode {
		Loading, ///< シーンの準備処理中.
		Runnable, ///< 準備が完了して実行可能な状態.
		Unloading, ///< シーンの終了処理中.
		Stopped, ///< シーンは終了している.
	};

	/**
	* Update()の戻り値.
	* 派生クラスで独自の終了コードを定義する場合、ExitCode_User以上の値を割り当てること.
	*/
	enum ExitCode {
		ExitCode_Continue = -2, ///< 現在のシーンを継続.
		ExitCode_Exit = -1, ///< 現在のシーンを終了.
		ExitCode_User = 0, ///< ここから派生クラス用の終了コード.
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
