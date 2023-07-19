#pragma once

namespace Engine {

	class Scene;

	class Application {
		// メンバ変数 ホントはSceneManagerに置く物.
		Scene* pScene_;			// シーンのポインタ.
		long long firstTime_;	// 開始時の時間.
		int tempScreen_;

		// コンストラクタ.
	public:
		Application();
		explicit Application(const Application& rhs) = delete;	// 複製禁止.
		void operator=(const Application& rhs) = delete;		// 複製禁止.

		// デストラクタ.
	public:
		~Application() = default;

		// 操作.
	public:
		bool initialize();	// 初期化.
		bool run();			// 実行.
		bool shutdown();	// 終了.

	};

}