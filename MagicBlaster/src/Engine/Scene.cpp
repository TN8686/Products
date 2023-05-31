#include "Scene.h"
#include "Graphics.h"
#include <algorithm>

/**
* 遷移元シーンIDの小なり比較を行う述語クラス.
*/
struct LessCurrentSceneId
{
    bool operator()(const Transition& lhs, int rhs) const
    {
        return lhs.currentScene < rhs;
    }
    bool operator()(int lhs, const Transition& rhs) const
    {
        return lhs < rhs.currentScene;
    }
};

/**
* 遷移制御クラスの初期化.
*
* @param transitionList  遷移情報の配列へのポインタ.
* @param transitionCount 遷移情報の数.
* @param creatorList     作成情報の配列へのポインタ.
* @param creatorCount    作成情報の数.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool TransitionController::Initialize(
    const Transition* transitionList, size_t transitionCount,
    const SceneCreator* creatorList, size_t creatorCount)
{
    transitionMap.assign(transitionList, transitionList + transitionCount);
    std::sort(transitionMap.begin(), transitionMap.end(),
        [](const Transition& lhs, const Transition& rhs) {
            return lhs.currentScene < rhs.currentScene; });
    creatorMap.assign(creatorList, creatorList + creatorCount);
    std::sort(creatorMap.begin(), creatorMap.end(),
        [](const SceneCreator& lhs, const SceneCreator& rhs) { return lhs.id < rhs.id; });
    return true;
}

/**
* 遷移を開始する.
*
* @param startSceneId 最初に実行するシーンのID.
*
* @retval true  正常に開始された.
* @retval false 開始に失敗した.
*/
bool TransitionController::Start(int startSceneId)
{
    if (const SceneCreator* creator = FindCreator(startSceneId)) {
        LoadScene(creator);
        return true;
    }
    return false;
}

/**
* シーンを更新する.
*
* @param delta 経過時間(秒).
*/
bool TransitionController::Update(double delta)
{

    if (sceneStack.empty()) {   // シーンが無ければ終了.
        return false;
    }

    // シーンアップデート.
    const auto itrEndPausedScene = sceneStack.end() - 1;
    for (auto itr = sceneStack.begin(); itr != itrEndPausedScene; ++itr) {
        itr->p->UpdateForPause(delta);
    }
    const int exitCode = sceneStack.back().p->Update(delta);
    if (exitCode == Scene::ExitCode_Continue) { // ExitCode_Continueだったらここで終了.
        return true;
    }

    // シーン遷移.
    const auto range = std::equal_range(transitionMap.begin(), transitionMap.end(), sceneStack.back().id, LessCurrentSceneId());
    const auto itr = std::find_if(range.first, range.second, [exitCode](const Transition& trans) { return trans.trans.exitCode == exitCode; });
    if (itr != range.second) {
        switch (itr->trans.type) {
        case TransitionType::Jump:
            if (const SceneCreator* creator = FindCreator(itr->trans.nextScene)) {
                while (!sceneStack.empty()) {
                    UnloadScene();
                }
                LoadScene(creator);
            }
            break;

        case TransitionType::Push:
            if (const SceneCreator* creator = FindCreator(itr->trans.nextScene)) {
                sceneStack.back().p->Pause();
                LoadScene(creator);
            }
            break;

        case TransitionType::Pop:
            UnloadScene();
            if (!sceneStack.empty()) {
                sceneStack.back().p->Resume();
            }
            break;
        }
    }
    Graphics::Get().WaitForGpu();   // GCでテクスチャが破棄されるタイミングで対象にアクセスしている可能性があるので事前にGPU待ちを行う.
    Graphics::Get().texMap.GC();

    return true;
}

/**
* シーンを描画する.
*/
void TransitionController::Draw() const
{
    for (const auto& scene : sceneStack) {
        if (scene.p->GetState() == Scene::StatusCode::Runnable) {
            scene.p->Draw();
        }
    }
}

/**
* シーン作成情報を検索する.
*
* @param sceneId 検索するシーンID.
*
* @retval nullptr以外 sceneIdに対応する作成情報へのポインタ.
* @retval nullptr     sceneIdに対応する作成情報は存在しない.
*/
const SceneCreator* TransitionController::FindCreator(int sceneId) const
{
    const auto itr = std::lower_bound(creatorMap.begin(), creatorMap.end(), sceneId,
        [](const SceneCreator& lhs, int rhs) { return lhs.id < rhs; });
    if (itr == creatorMap.end() || itr->id != sceneId) {
        return nullptr;
    }
    return &*itr;
}

/**
* 新しいシーンを開始する.
*
* @param creator 開始するシーンの作成情報へのポインタ.
*/
void TransitionController::LoadScene(const SceneCreator* creator)
{
    sceneStack.push_back({ creator->id, creator->func() });
    sceneStack.back().p->Load();
    sceneStack.back().p->status = Scene::StatusCode::Runnable;
}

/**
* シーンを終了する.
*/
void TransitionController::UnloadScene()
{
    sceneStack.back().p->status = Scene::StatusCode::Unloading;
    sceneStack.back().p->Unload();
    sceneStack.back().p->status = Scene::StatusCode::Stopped;
    sceneStack.pop_back();
}

/**
* 遷移を終了する.
*/
void TransitionController::Stop()
{
    while (!sceneStack.empty()) {
        UnloadScene();
    }
}
