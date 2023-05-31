#pragma once
#include <stdint.h>

/**
* �Q�[���p�b�h���͂�\���\����.
*/
struct GamePad
{
	enum {
		DPAD_UP = 0x0001,
		DPAD_DOWN = 0x0002,
		DPAD_LEFT = 0x0004,
		DPAD_RIGHT = 0x0008,
		START = 0x0010,
		SELECT = 0x0020,
		LSB = 0x0040,
		RSB = 0x0080,
		L = 0x0100,
		R = 0x0200,
		LT = 0x400,
		RT = 0x800,
		A = 0x1000,
		B = 0x2000,
		X = 0x4000,
		Y = 0x8000,
		CONNECTED = 0x80000000, ///< �R���g���[���[���ڑ�����Ă���.
	};
	uint32_t buttons;
	uint32_t buttonDown; ///< �����ꂽ�u�Ԃ����r�b�g������.
	uint32_t prevButtons; ///< �O���buttons�̒l(oneShot�v�Z�p).
	uint32_t trigger;
	double connectionCheckInterval; ///< �R���g���[���[�̐ڑ��m�F�҂�����.
};


/// �Q�[���p�b�hID.
enum GamePadId {
	GamePadId_1P,
//	GamePadId_2P,
//	GamePadId_3P,
//	GamePadId_4P,
	countof_GamePadId, ///< �Q�[���p�b�h�̐�.
};

GamePad& GetGamePad(uint32_t id);
void InitializeGamePad();
void UpdateGamePad(double delta);
void VibrateGamePad(uint32_t id, uint32_t seqNo);
size_t GetVibrationListSize();