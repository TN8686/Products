#include "Animation.h"

/**
* �R���X�g���N�^.
*
* @param al �ݒ肷��A�j���[�V�������X�g.
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
* �A�j���[�V�����V�[�P���X�̃C���f�b�N�X��ݒ肷��.
*
* @param index �ݒ肷��V�[�P���X�C���f�b�N�X.
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
* �A�j���[�V�����V�[�P���X�̃C���f�b�N�X�ƌo�ߎ��Ԃ�����������.
*/
void AnimationController::Reset()
{
	cellIndex = 0;
	time = 0.0f;
	isFinished = false;
}

/**
* �Z���C���f�b�N�X��ݒ肷��.
*
* @param index �ݒ肷��Z���C���f�b�N�X.
*
* �A�j���[�V�������X�g���ݒ肳��Ă��Ȃ��ꍇ�ɁA�蓮�ŃZ���C���f�b�N�X�𐧌䂷�邽�߂Ɏg��.
* �A�j���[�V�������X�g���ݒ肳��Ă���ꍇ�͉������Ȃ�.
*/
void AnimationController::SetCellIndex(uint32_t index)
{
	if(!list || index < (*list)[seqIndex].animationData.size()){
		cellIndex = index;
	}
}

/**
* �A�j���[�V�����̏�Ԃ��X�V����.
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
* �A�j���[�V�����f�[�^���擾����.
*
* @return �A�j���[�V�����f�[�^.
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
* ���X�g���̃A�j���[�V�����V�[�P���X�̐����擾����.
*
* @return �A�j���[�V�����V�[�P���X�̐�.
*/
size_t AnimationController::GetSeqCount() const
{
	return list ? list->size() : 0U;
}

/**
* �A�j���[�V�����V�[�P���X�̍Đ��I������.
*
* @retval true  �Đ��I���A�܂��̓A�j���[�V�������X�g���ݒ肳��Ă��Ȃ�.
* @retval false �Đ���.
*/
bool AnimationController::IsFinished() const
{
	if (isFinished || !list || seqIndex >= list->size() || cellIndex >= list->at(seqIndex).animationData.size()) {
		return true;
	}

	return false;
}