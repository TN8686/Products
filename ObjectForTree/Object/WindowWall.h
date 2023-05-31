#pragma once
#include "../Engine/GameObject.h"

namespace Object {

    class WindowWall : public Engine::GameObject {
    private:
        static constexpr int WALL_NUM = 4;
        static constexpr int HALF_THICKNESS = 32;

    private:
        Engine::AABBCollision2D* collision_[WALL_NUM];

        // �R���X�g���N�^.
    public:
        WindowWall()
            : WindowWall("WindowWall")
        {}
        WindowWall(std::string name)
            : GameObject(name)
            , collision_{nullptr, nullptr ,nullptr ,nullptr }
        {}

    public:
        // ����.

        bool load() override;
        //bool update() override;
        //bool render() override;

        //void onAABBStay2D(Engine::AABBCollision2D* const collision) override {}
    };

}