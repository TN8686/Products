#pragma once
#include <vector>

namespace Engine {
	class GameObject;
	class ObjectManager;

	class Scene {
		// �萔.
	public:
		enum ExitCode {
			EXIT_CODE_CONTINUE,
			EXIT_CODE_EXIT
		};

	protected:
		ObjectManager* objectManager_;	// �I�u�W�F�N�g�}�l�[�W���̃|�C���^.

		// �R���X�g���N�^.
	protected:
		Scene();

		// �f�X�g���N�^.
	public:
		~Scene() = default;

		// ����.
	public:
		virtual bool load() = 0;
		virtual bool init() = 0;
		virtual int	update() = 0;
		virtual bool render() = 0;
		virtual bool unload() = 0;

		// �������
	private:
		explicit Scene(const Scene& scn) = delete;	// �����֎~.
		void operator=(const Scene& scn) = delete;	// �����֎~.
	};
}