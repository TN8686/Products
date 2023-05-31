#include "Scene.h"
#include "Graphics.h"
#include <algorithm>

/**
* �J�ڌ��V�[��ID�̏��Ȃ��r���s���q��N���X.
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
* �J�ڐ���N���X�̏�����.
*
* @param transitionList  �J�ڏ��̔z��ւ̃|�C���^.
* @param transitionCount �J�ڏ��̐�.
* @param creatorList     �쐬���̔z��ւ̃|�C���^.
* @param creatorCount    �쐬���̐�.
*
* @retval true  ����������.
* @retval false ���������s.
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
* �J�ڂ��J�n����.
*
* @param startSceneId �ŏ��Ɏ��s����V�[����ID.
*
* @retval true  ����ɊJ�n���ꂽ.
* @retval false �J�n�Ɏ��s����.
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
* �V�[�����X�V����.
*
* @param delta �o�ߎ���(�b).
*/
bool TransitionController::Update(double delta)
{

    if (sceneStack.empty()) {   // �V�[����������ΏI��.
        return false;
    }

    // �V�[���A�b�v�f�[�g.
    const auto itrEndPausedScene = sceneStack.end() - 1;
    for (auto itr = sceneStack.begin(); itr != itrEndPausedScene; ++itr) {
        itr->p->UpdateForPause(delta);
    }
    const int exitCode = sceneStack.back().p->Update(delta);
    if (exitCode == Scene::ExitCode_Continue) { // ExitCode_Continue�������炱���ŏI��.
        return true;
    }

    // �V�[���J��.
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
    Graphics::Get().WaitForGpu();   // GC�Ńe�N�X�`�����j�������^�C�~���O�őΏۂɃA�N�Z�X���Ă���\��������̂Ŏ��O��GPU�҂����s��.
    Graphics::Get().texMap.GC();

    return true;
}

/**
* �V�[����`�悷��.
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
* �V�[���쐬������������.
*
* @param sceneId ��������V�[��ID.
*
* @retval nullptr�ȊO sceneId�ɑΉ�����쐬���ւ̃|�C���^.
* @retval nullptr     sceneId�ɑΉ�����쐬���͑��݂��Ȃ�.
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
* �V�����V�[�����J�n����.
*
* @param creator �J�n����V�[���̍쐬���ւ̃|�C���^.
*/
void TransitionController::LoadScene(const SceneCreator* creator)
{
    sceneStack.push_back({ creator->id, creator->func() });
    sceneStack.back().p->Load();
    sceneStack.back().p->status = Scene::StatusCode::Runnable;
}

/**
* �V�[�����I������.
*/
void TransitionController::UnloadScene()
{
    sceneStack.back().p->status = Scene::StatusCode::Unloading;
    sceneStack.back().p->Unload();
    sceneStack.back().p->status = Scene::StatusCode::Stopped;
    sceneStack.pop_back();
}

/**
* �J�ڂ��I������.
*/
void TransitionController::Stop()
{
    while (!sceneStack.empty()) {
        UnloadScene();
    }
}
