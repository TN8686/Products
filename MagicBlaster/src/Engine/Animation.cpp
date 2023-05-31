#include "Animation.h"

/**
* コンストラクタ.
*
* @param al 設定するアニメーションリスト.
*/
AnimationController::AnimationController(const AnimationList* al)
	: list(al)
	, seqIndex(0)
	, cellIndex(0)
	, time(0.0)
	, isFinished(false)
{
}

/**
* アニメーションシーケンスのインデックスを設定する.
*
* @param index 設定するシーケンスインデックス.
*/
void AnimationController::SetSeqIndex(uint32_t index)
{
	if (!list || index >= list->size() || seqIndex == index) {
		return;
	}
	seqIndex = index;
	cellIndex = 0;
	time = 0.0f;
}


/**
* アニメーションシーケンスのインデックスと経過時間を初期化する.
*/
void AnimationController::Reset()
{
	cellIndex = 0;
	time = 0.0f;
	isFinished = false;
}

/**
* セルインデックスを設定する.
*
* @param index 設定するセルインデックス.
*
* アニメーションリストが設定されていない場合に、手動でセルインデックスを制御するために使う.
* アニメーションリストが設定されている場合は何もしない.
*/
void AnimationController::SetCellIndex(uint32_t index)
{
	if(!list || index < (*list)[seqIndex].animationData.size()){
		cellIndex = index;
	}
}

/**
* アニメーションの状態を更新する.
*/
void AnimationController::Update(double delta)
{
	if (!list || (*list)[seqIndex].animationData.empty()) {
		return;
	}

	time += delta;
	for (;;) {
		const double targetTime = list->at(seqIndex).animationData.at(cellIndex).time;

		if (targetTime == 0.0f || time < targetTime) {
			break;
		}
		time -= targetTime;
		++cellIndex;
		if (cellIndex >= list->at(seqIndex).animationData.size()) {
			if (list->at(seqIndex).isLoop) {
				cellIndex = 0;
				isFinished = false;
			}
			else {
				isFinished = true;
				--cellIndex;
			}
			break;
		}
	}
}


/**
* アニメーションデータを取得する.
*
* @return アニメーションデータ.
*/
AnimationData AnimationController::GetAnimationData() const
{
	if (!list) {
		return { cellIndex };
	}
	if (seqIndex >= list->size() || (*list)[seqIndex].animationData.empty()) {
		AnimationData ret;
		ret.cellIndex = 0;
		return ret;
	}
	return (*list)[seqIndex].animationData[cellIndex];
}

/**
* リスト内のアニメーションシーケンスの数を取得する.
*
* @return アニメーションシーケンスの数.
*/
size_t AnimationController::GetSeqCount() const
{
	return list ? list->size() : 0U;
}

/**
* アニメーションシーケンスの再生終了判定.
*
* @retval true  再生終了、またはアニメーションリストが設定されていない.
* @retval false 再生中.
*/
bool AnimationController::IsFinished() const
{
	if (isFinished || !list || seqIndex >= list->size() || cellIndex >= list->at(seqIndex).animationData.size()) {
		return true;
	}

	return false;
}