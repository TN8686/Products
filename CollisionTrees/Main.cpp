#include "DxLib.h"
#include "Utility/Reference.h"
#include "Engine/Application.h"

#include "Utility/MyArray.h"
#include <algorithm>

using namespace Utility;

Engine::Application app;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// DxLib_Init()�O�̏���
	ChangeWindowMode(true);	// true�ŃE�C���h�E���[�h�ŋN�� false�Ńt���X�N���[���ŋN��
	SetGraphMode(WindowSize::WIDTH, WindowSize::HEIGHT, 16);	// �E�C���h�E���쐬 
	SetMainWindowText("SpringMotion");	// �E�C���h�E�^�C�g����ݒ�

	// DX���C�u�����̏�����
	if (DxLib_Init() < 0) {
		return -1;
	}
	
	app.initialize();

	app.run();

	app.shutdown();

	// DX���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}