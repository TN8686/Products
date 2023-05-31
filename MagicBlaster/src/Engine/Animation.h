#pragma once

#include <stdint.h>
#include <vector>
#include <DirectXMath.h>

/**
* �A�j���[�V�����f�[�^.
*/
struct AnimationData
{
	uint32_t cellIndex; ///< �Z���f�[�^���X�g���̃C���f�b�N�X.
	double time; ///< �\�����鎞��(�b).
	float rotation = 0; ///< �摜�̉�]�p(���W�A��).
	DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }; ///< �摜�̊g�嗦.
	DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f }; ///< �摜�̐F.
	bool flipHorizontal = false;	// ���E���].
	bool flipVertical = false;		// �㉺���].
};


/**
* �P��̃A�j���[�V�������\������A�j���[�V�����f�[�^�̃��X�g.
*/
struct AnimationSequence{
	std::vector<AnimationData> animationData;
	bool isLoop = true;			///< ���[�v�t���O.
};

/**
* �A�j���[�V�����V�[�P���X�̃��X�g.
*
* �C���f�b�N�X�w��ɂ���āA�Đ�����A�j���[�V�������擾�ł���悤�ɂ���.
*/
typedef std::vector<AnimationSequence> AnimationList;

/**
* �A�j���[�V��������N���X.
*
* - ���t���[��Update���ĂԂ��ƂŃA�j���[�V�������i�s����.
* - �A�j���[�V������؂�ւ���ɂ�SetSeqIndex���Ă�.
* - ���݂̃Z���C���f�b�N�X���擾����ɂ�GetCellIndex���Ă�.
* - �f�t�H���g�R���X�g���N�^���邢�͈�����nullptr���w�肵�č\�z�����ꍇ�A�A�j���[�V�������X�g�������Ȃ���ԂɂȂ�.
*   �A�j���[�V�������X�g�������Ȃ���Ԃł́ASetCellIndex�֐��ŃZ���C���f�b�N�X��ݒ�ł���.
*/
class AnimationController
{
public:
	AnimationController() = default;
	explicit AnimationController(const AnimationList* al);
	~AnimationController() = default;
	AnimationController& operator=(const AnimationController&) = default;

	void SetSeqIndex(uint32_t index);
	void Reset();
	void Update(double delta);
	void SetCellIndex(uint32_t index);
	AnimationData GetAnimationData() const;
	size_t GetSeqCount() const;
	bool IsFinished() const;
	uint32_t GetSeqIndex() const { return seqIndex; }
	//void SetLoop(bool);

private:
	const AnimationList* list = nullptr; ///< �A�j���[�V�����V�[�P���X�̃��X�g.
	uint32_t seqIndex = 0; ///< �Đ����̃V�[�P���X�C���f�b�N�X.
	uint32_t cellIndex = 0; ///< �Đ����̃Z���C���f�b�N�X.
	double time = 0; ///< �Đ�����.
	bool isFinished = false;	///< �Đ��I���t���O.
	//bool isLoop = true;			///< ���[�v�t���O.
};
