#include "Application.h"
#include "DxLib.h"
#include "HitChecker.h"
#include "../Scene/GameScene.h"
#include "../Utility/Reference.h"

#include <string>

using namespace Utility;
using namespace Scene;

namespace Engine {

	// �R���X�g���N�^.
	Application::Application()
		: pScene_(nullptr)
		, firstTime_(0)
	{
	}

	// ������.
	bool Application::initialize() {
		
		// �`����ʂ𗠉�ʂɃZ�b�g
		SetDrawScreen(DX_SCREEN_BACK);

		HitChecker::getInstance()->initialize();	// �R���W�����}�l�[�W����������.

		pScene_ = new GameScene();

		return true;
	}

	// ���s.
	bool Application::run() {

		// �z���g�͏��XSceneManager�����.

		// ���[�h.
		pScene_->load();

		pScene_->init();

		// TODO �t���[�����[�g�v��������N���X��.
		static bool isSyorioti;
		static long long fpsTime, fpsFirstTime, fps, fpsCount;
		fpsFirstTime = GetNowHiPerformanceCount();
		fps = 0;
		std::string str;
		// �Ƃ肠�����̃Q�[�����[�v.
		int ret = Scene::EXIT_CODE_CONTINUE;
		while (ProcessMessage() == 0 && ret == Scene::EXIT_CODE_CONTINUE) {

			// ���݂̃J�E���g���擾����
			firstTime_ = GetNowHiPerformanceCount();

			// fps�v���p.
			if (firstTime_ - fpsFirstTime >= Frame::MICROSECONDS_PER_FRAME * 60) {
				fpsFirstTime = GetNowHiPerformanceCount();
				fps = fpsCount;
				fpsCount = 0;
			}

			// �X�V.
			ret = pScene_->update();

			// �`��.
			ClearDrawScreen();	// ��ʂ�������.
			pScene_->render();	// �`��.

			// fps�\��.
			str = std::to_string(fps);
			DrawString(WindowSize::WIDTH - 16, WindowSize::HEIGHT - 16, str.c_str(), GetColor(0xFF, 0xFF, 0xFF));

			// ����ʂ̓��e��\��ʂɔ��f������
			ScreenFlip();

			// 1�t���[�����̎��Ԃ��o�߂���܂őҋ@.
			while (GetNowHiPerformanceCount() - firstTime_ < Frame::MICROSECONDS_PER_FRAME) {
				Sleep(0);
			}

			++fpsCount;
		}

		// �I������.
		pScene_->unload();

		return true;
	}

	// �I��.
	bool Application::shutdown() {
		delete pScene_;

		return true;
	}

}
