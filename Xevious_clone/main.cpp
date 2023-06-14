#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool isDebug = true;	// �Q�[���O�f�o�b�O���[�h�t���O�@�E�B���h�E�T�C�Y�̊֌W�Ŏg�p

	// DxLib_Init()�O�̏���
	ChangeWindowMode(isDebug);	// true�ŃE�C���h�E���[�h�ŋN�� false�Ńt���X�N���[���ŋN��
	if (isDebug) {
		SetGraphMode(WINDOW_WIDTH + TEMP_SCREEN_HEIGHT, WINDOW_HEIGHT, 16);	// �\���̈�̓�{�~��{�̃T�C�Y+�E���ɉ���ʕ\���g�������ăE�C���h�E���쐬
	}
	else {
		SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 16);	// �\���̈�̓�{�~��{�̃T�C�Y�ŃE�C���h�E���쐬 
	}
	SetMainWindowText("Xevious_clone");	// �E�C���h�E�^�C�g����ݒ�

	// DX���C�u�����̏�����
	if (DxLib_Init() < 0) return -1;

	GameManager gameManager(isDebug);	// �Q�[���}�l�[�W���̎��̐錾

	gameManager.MainLoop();				// ���C�����[�v

	// DX���C�u�����̌�n��
	DxLib_End();

	// �\�t�g�̏I��
	return 0;
}