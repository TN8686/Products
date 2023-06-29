#pragma once
#include <unordered_map>
#include <string>
#include "../Utility/MemoryBuff.h"

namespace Engine {

	class Player;
	class Camera;

	struct DataSet{
		std::unordered_map<std::string, int> hImage;	// 画像ハンドルリスト.
		std::unordered_map<std::string, int> hBGM;		// BGMハンドルリスト.
		std::unordered_map<std::string, int> hSE;		// SEハンドルリスト.
		std::unordered_map<std::string, Utility::MemoryPool> objBuff;	// オブジェクト領域のリスト.
		Player* pPlayer;								// プレイヤーのポインタ.
		Camera* pCamera;								// カメラのポインタ.
	};

}