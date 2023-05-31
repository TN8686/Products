#pragma once

#include "../Engine/GameObject.h"

namespace Object {

    class BlockPiece : public Engine::GameObject {
    private:
        int halfLength_;
        Engine::Float2 vector_;
        float rotationVel_;

        // コンストラクタ.
    public:
        BlockPiece()
            : BlockPiece("Piece")
        {}

        BlockPiece(std::string name)
            : GameObject(name)
            , halfLength_(10)
            , vector_(0.0f, 0.0f)
            , rotationVel_(0.0f)
        {}

    public:
        // 操作.
        //bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;

        //void onAABBStay2D(Engine::AABBCollision2D* const collision) override;
    };

}