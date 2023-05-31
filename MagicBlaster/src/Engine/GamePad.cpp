#include "GamePad.h"
#include <Windows.h>
#include <Xinput.h>
#include <random>
#include <vector>
#include <iterator>

// TODO �T���v���̌`�ł͐U���̓�̃��[�^�[�ɑ΂��ČʂɃC�x���g��ݒ�ł��Ȃ��@�v�C��.
// �T�v���w�K���A���DirectInput�ɂ��Ή�������.

namespace { // unnamed namespace

/**
* ���[�^�[�̎��.
*/
	enum VibrationType
	{
		VibrationType_Low, ///< ����g���[�^�[.
		VibrationType_High, ///< �����g���[�^�[.
		countof_VibrationType ///< ���[�^�[�̎�ސ�.
	};

	/**
	* �U������p�����[�^.
	*/
	struct VibrationUnit
	{
		float currentTime; ///< �U���o�ߎ���.
		float gateTime; ///< ���U������.
		float startVelocity; ///< �U���J�n���̋���.
		float targetVelocity; ///< �U���I�����̋���.
	};

	/**
	* �U�����.
	*/
	struct VibrationState
	{
		VibrationState() : sequenceNo(-1) {}

		int sequenceNo; ///< �U���V�[�P���X�̔ԍ�.
		size_t index; ///< �V�[�P���X���̍Đ��ʒu.
		float deltaTime; ///< �Đ�����.
		VibrationUnit unit[countof_VibrationType]; ///< ���[�^�[���̃p�����[�^.
	};

	/**
	* �U���C�x���g(�A�j���[�V������`�p).
	*/
	struct VibrationEvent
	{
		float deltaTime; ///< �J�n����.
		VibrationType type; ///< ���[�^�[�̎��.
		float gateTime; ///< �U������.
		float velocity; ///< �U���̋���.
	};
	typedef std::vector<VibrationEvent> VibrationSequence; ///< �U���V�[�P���X.

	// �Ƃ肠�����̐U���T���v��.
	/// ����g�U��.
	const VibrationEvent ve00[] = {
	  { 0.0f, VibrationType_Low, 1.0f, 1.0f },
	  //{ 1.0f, VibrationType_Low, 1.0f, 1.0f },
	};

	/// �����g�U��.
	const VibrationEvent ve01[] = {
	  { 0.0f, VibrationType_High, 1.0f, 5.0f },
	  //{ 1.0f, VibrationType_High, 1.0f, 1.0f },
	};

	/// ������������g�U��.
	const VibrationEvent ve02[] = {
	  { 0.0f, VibrationType_Low, 0.5f, 1.0f },
	  { 0.5f, VibrationType_Low, 0.5f, 0.1f },
	  { 0.5f, VibrationType_Low, 0.5f, 1.0f },
	  { 0.5f, VibrationType_Low, 1.0f, 0.0f },
	};


	VibrationState vibrationState[countof_GamePadId]; ///< �R���g���[���[���̐U�����
	std::vector<VibrationSequence> vibrationList; ///< �U���V�[�P���X�̃��X�g.

	/// XInput�̃L�[��GamePad�̃L�[�̕ϊ��f�[�^.
	struct ConversionData
	{
		uint32_t di; ///< XInput�̃L�[.
		uint32_t gp; ///< GamePad�̃L�[.
	};

	/// XInput�̃L�[��GamePad�̃L�[�̕ϊ��\.
	const ConversionData conversionMap[] = {
		{ XINPUT_GAMEPAD_DPAD_UP, GamePad::DPAD_UP },
		{ XINPUT_GAMEPAD_DPAD_DOWN, GamePad::DPAD_DOWN },
		{ XINPUT_GAMEPAD_DPAD_LEFT, GamePad::DPAD_LEFT },
		{ XINPUT_GAMEPAD_DPAD_RIGHT, GamePad::DPAD_RIGHT },
		{ XINPUT_GAMEPAD_START, GamePad::START },
		{ XINPUT_GAMEPAD_BACK, GamePad::SELECT },
		{ XINPUT_GAMEPAD_LEFT_THUMB, GamePad::LSB },
		{ XINPUT_GAMEPAD_RIGHT_THUMB, GamePad::RSB },
		{ XINPUT_GAMEPAD_LEFT_SHOULDER, GamePad::L },
		{ XINPUT_GAMEPAD_RIGHT_SHOULDER, GamePad::R },
		{ XINPUT_GAMEPAD_A, GamePad::A },
		{ XINPUT_GAMEPAD_B, GamePad::B },
		{ XINPUT_GAMEPAD_X, GamePad::X },
		{ XINPUT_GAMEPAD_Y, GamePad::Y },
	};
	const double minInterval = 3.0; ///< �R���g���[���[�ڑ��`�F�b�N�̍ŒZ�҂�����(�b).
	const double maxInterval = 5.0; ///< �R���g���[���[�ڑ��`�F�b�N�̍Œ��҂�����(�b).

	/// �R���g���[���[�ڑ��`�F�b�N�p�҂����Ԑ�����.
	std::mt19937 randomGenerator;


	/**
	* XBOX360�R���g���[���[�̏�Ԃ�GamePad�\���̂ɔ��f����.
	*
	* @param id    �R���g���[���[ID.
	* @param delta �O��̌Ăяo������̌o�ߎ���.
	*/
	void ApplyControllerState(uint32_t id, double delta)
	{
		GamePad& gamepad = GetGamePad(id);
		// �ڑ��҂����ԃ`�F�b�N.
		if (gamepad.connectionCheckInterval > 0.0) {
			gamepad.connectionCheckInterval -= delta;
			return;
		}
		gamepad.connectionCheckInterval = 0.0;

		// �R���g���[���[�̏�Ԃ̎擾�ɐ������Ȃ�������҂����Ԃ��Đݒ�.
		XINPUT_STATE state;
		if (XInputGetState(id, &state) != ERROR_SUCCESS) {
			const std::uniform_real_distribution<double> range(minInterval, maxInterval);	// ���������_���ɂ��A�����̐ڑ��m�F������t���[���ɂȂ�Ȃ��悤��.
			gamepad.connectionCheckInterval = range(randomGenerator);
			gamepad.buttons &= ~GamePad::CONNECTED;
			return;
		}

		// �Ή��\������GamePad�N���X�̃L�[�ɕϊ�.
		gamepad.buttons = GamePad::CONNECTED;	// CONNECTED�ŏ��������ăL�[�{�[�h���͂𖳌�.
		for (auto e : conversionMap) {
			if (state.Gamepad.wButtons & e.di) {
				gamepad.buttons |= e.gp;
			}
		}

		// LT��RT���l�ɕϊ�.
		if (state.Gamepad.bLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			gamepad.buttons |= GamePad::LT;
		}
		if (state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
			gamepad.buttons |= GamePad::RT;
		}

		// �X�e�B�b�N���͂��\���L�[���͂ɕϊ�.
		if (state.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_LEFT;
		}
		else if (state.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_RIGHT;
		}
		if (state.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_DOWN;
		}
		else if (state.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
			gamepad.buttons |= GamePad::DPAD_UP;
		}

	}


	/**
	* ���[�^�[�p�̐U����Ԃ��X�V.
	*
	* @param unit �X�V����U�����j�b�g.
	* @param delta �O��̌Ăяo������̌o�ߎ���.
	*
	* @return �U���p�����[�^.
	*/
	WORD UpdateVibrationUnit(VibrationUnit& unit, float delta)
	{
		float ratio = 1.0f;
		if (unit.currentTime + delta < unit.gateTime) {
			unit.currentTime += delta;	// ���݂̐U�����Ԃ��X�V.
			ratio = unit.currentTime / unit.gateTime;	// �䗦���v�Z.
		}
		// �U���̋�������`�⊮���ĕԂ�.
		return static_cast<WORD>((unit.startVelocity +
			(unit.targetVelocity - unit.startVelocity) * ratio) * 65535.0f);
	}

	/**
	* �R���g���[���[�̐U����Ԃ��X�V.
	*
	* @param id �R���g���[���[ID.
	* @param delta �O��̌Ăяo������̌o�ߎ���.
	*/
	void UpdateVibration(uint32_t id, float delta)
	{
		VibrationState& vibState = vibrationState[id];

		// �U����Ԃ��擾���A�V�[�P���X���ݒ�Ȃ�U��0.
		if (vibState.sequenceNo < 0) {
			XINPUT_VIBRATION vib = {};
			XInputSetState(id, &vib);
			return;
		}

		// �U���A�j���[�V�������Ԃ��X�V.
		const VibrationSequence& vibSeq = vibrationList[vibState.sequenceNo];
		vibState.deltaTime += static_cast<float>(delta);

		// �Đ����Ԃɓ��B�����C�x���g���擾���A�U�����j�b�g�ɐݒ�.
		for (; vibState.index < vibSeq.size(); ++vibState.index) {
			const VibrationEvent& vibEvent = vibSeq[vibState.index];
			if (vibState.deltaTime < vibEvent.deltaTime) {
				vibState.unit[vibEvent.type].currentTime += static_cast<float>(delta);
				break;
			}
			vibState.deltaTime -= vibEvent.deltaTime;
			vibState.unit[vibEvent.type].currentTime = 0.0f;
			vibState.unit[vibEvent.type].gateTime = vibEvent.gateTime;
			vibState.unit[vibEvent.type].startVelocity =
				vibState.unit[vibEvent.type].targetVelocity;
			vibState.unit[vibEvent.type].targetVelocity = vibEvent.velocity;
		}

		// �U�����j�b�g�̏�Ԃ��X�V���A�R���g���[���[�ɔ��f.
		XINPUT_VIBRATION vib = {
		  UpdateVibrationUnit(vibState.unit[VibrationType_Low], delta),
		  UpdateVibrationUnit(vibState.unit[VibrationType_High], delta)
		};
		XInputSetState(id, &vib);
		
		}


} // unnamed namespace

/**
* �Q�[���p�b�h�̏�����.
*/
void InitializeGamePad()
{
	randomGenerator.seed(std::random_device()());	// �ŏ��̊��ʂ̓R���X�g���N�^�ŁA���̊��ʂ͊֐��Ăяo�����Z�q�̂���.

	// �U���V�[�P���X���X�g�̍쐬.
	// �Ƃ肠�����̃x�^���� �{���I�ɂ̓t�@�C������ǂݍ���.
	static const struct {
		const VibrationEvent* begin;
		const VibrationEvent* end;
	} seqList[] = {
	  { std::begin(ve00), std::end(ve00) },
	  { std::begin(ve01), std::end(ve01) },
	  { std::begin(ve02), std::end(ve02) },
	};
	vibrationList.reserve(_countof(seqList));
	for (const auto& e : seqList) {
		vibrationList.emplace_back(e.begin, e.end);
	}

}

/**
* �Q�[���p�b�h�̏�Ԃ��擾����.
*
* @param id �擾����Q�[���p�b�h��ID.
*           0~countof_GamePadId�ȊO�̒l��n���ƃ_�~�[�f�[�^��Ԃ�.
*           �_�~�[�f�[�^�͏�ɉ��̓��͂��Ȃ���ԂɂȂ�.
*
* @return id�ɑΉ�����Q�[���p�b�h�̏��.
*/
GamePad& GetGamePad(uint32_t id)
{
	if (id >= countof_GamePadId) {
		static GamePad dummy;
		dummy = {};
		return dummy;
	}
	static GamePad gamepad[countof_GamePadId] = {};
	return gamepad[id];
}

/**
* �Q�[���p�b�h�̏�Ԃ��X�V����.
*
* @param delta �O��̌Ăяo������̌o�ߎ���.
*/
void UpdateGamePad(double delta)
{
	for (uint32_t id = 0; id < countof_GamePadId; ++id) {
		ApplyControllerState(id, delta);
		GamePad& gamepad = GetGamePad(id);
		gamepad.buttonDown = ~gamepad.prevButtons & gamepad.buttons;	// �O�񉟂���Ă��Ȃ������{�^����������Ă����ꍇ�͏�����͂Ƃ���.
		gamepad.prevButtons = gamepad.buttons;	// �O�񉟂���Ă����{�^���Ƃ��ċL�^���Ă���
		//UpdateVibration(id, static_cast<float>(delta));
	}
}

/**
* �R���g���[���[��U��������.
*
* @param id    �R���g���[���[ID.
* @param seqNo �U���V�[�P���XNo.
*/
void VibrateGamePad(uint32_t id, uint32_t seqNo)
{
	if (id < countof_GamePadId && seqNo < vibrationList.size()) {
		vibrationState[id].sequenceNo = seqNo;
		vibrationState[id].index = 0;
		vibrationState[id].deltaTime = 0.0f;
	}
}

/**
* �U���V�[�P���X���X�g�̗v�f�����擾����.
*
* @return �U���V�[�P���X���X�g�̗v�f��.
*/
size_t GetVibrationListSize()
{
	return vibrationList.size();
}
