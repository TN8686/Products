#pragma once
#include <unordered_map>
#include <string>
#include "../Utility/MemoryBuff.h"

namespace Engine {

	class Player;
	class Camera;

	struct DataSet{
		std::unordered_map<std::string, int> hImage;	// �摜�n���h�����X�g.
		std::unordered_map<std::string, int> hBGM;		// BGM�n���h�����X�g.
		std::unordered_map<std::string, int> hSE;		// SE�n���h�����X�g.
		std::unordered_map<std::string, Utility::MemoryPool> objBuff;	// �I�u�W�F�N�g�̈�̃��X�g.
		Player* pPlayer;								// �v���C���[�̃|�C���^.
		Camera* pCamera;								// �J�����̃|�C���^.
	};

}