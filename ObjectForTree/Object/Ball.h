#pragma once
#include "../Engine/GameObject.h"

namespace Object {

	class Ball : public Engine::GameObject {
        // 定数.
    public:
        static float constexpr BREAK_SPEED = 10.0f * 10.0f;

    private:
        Engine::Float2 velocity_;               // 加速度.
        Engine::Float2 tempVelocity_;           // 一時的な加速度.
        Engine::AABBCollision2D* collision_;    // コリジョン.
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;

        float speedSquare_; // 現在の直線移動速度の二乗.
        float length_;      // ヒモの長さ.
        int r_;             // 円の半径.

        int wait_;          // ヒットストップの現在残りフレーム.

        int hitSeHandle_;   // seハンドル.

        // コンストラクタ.
    public:
        Ball()
            : Ball("Ball")
        {}
        Ball(std::string name)
            : GameObject(name)
            , velocity_()
            , collision_(nullptr)
            , speedSquare_(0.0f)
            , length_(70.0f)
            , r_(20)
            , wait_(0)
            , hitSeHandle_(0)
        {}

        // 操作.
    public:
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;

        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;

        // 属性.
    public:
        float getSpeedSquare() { return speedSquare_; }
	};

}