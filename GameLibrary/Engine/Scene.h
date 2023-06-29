#pragma once
#include <vector>

namespace Engine {
	class GameObject;
	class ObjectManager;

	class Scene {
		// 定数.
	public:
		enum ExitCode {
			EXIT_CODE_CONTINUE,
			EXIT_CODE_EXIT
		};

	protected:
		ObjectManager* objectManager_;	// オブジェクトマネージャのポインタ.

		// コンストラクタ.
	protected:
		Scene();

		// デストラクタ.
	public:
		~Scene() = default;

		// 操作.
	public:
		virtual bool load() = 0;
		virtual bool init() = 0;
		virtual int	update() = 0;
		virtual bool render() = 0;
		virtual bool unload() = 0;

		// 非実装提供
	private:
		explicit Scene(const Scene& scn) = delete;	// 複製禁止.
		void operator=(const Scene& scn) = delete;	// 複製禁止.
	};
}