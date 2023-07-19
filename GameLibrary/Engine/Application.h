#pragma once

namespace Engine {

	class Scene;

	class Application {
		// �����o�ϐ� �z���g��SceneManager�ɒu����.
		Scene* pScene_;			// �V�[���̃|�C���^.
		long long firstTime_;	// �J�n���̎���.
		int tempScreen_;

		// �R���X�g���N�^.
	public:
		Application();
		explicit Application(const Application& rhs) = delete;	// �����֎~.
		void operator=(const Application& rhs) = delete;		// �����֎~.

		// �f�X�g���N�^.
	public:
		~Application() = default;

		// ����.
	public:
		bool initialize();	// ������.
		bool run();			// ���s.
		bool shutdown();	// �I��.

	};

}