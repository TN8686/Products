#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/CollisionTrees.h"

namespace Object {

    class BlockPiece;

    class Block : public Engine::GameObject {
        // �萔.
    private:
        static constexpr int PIECE_NUM = 4;

        // �����o�ϐ�.
    private:
        static int breakSeHandle_;  // �j��SE�n���h���̑�{.
        static int blockObjNum_;    // �u���b�N�I�u�W�F�N�g�̑���.

        Engine::AABBCollision2D* collision_;
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;
        int halfLength_;
        BlockPiece* piece[PIECE_NUM];
        int localBreakSeHandle_;                      // �ʂ̔j��SE�n���h��.

        unsigned int color;

        // �R���X�g���N�^.
    public:
        Block()
            : Block("Block")
        {}

        Block(std::string name)
            : GameObject(name)
            , collision_(nullptr)
            , halfLength_(10)
            , localBreakSeHandle_(0)
            , color(0)
        {
            for (int i = 0; i < PIECE_NUM; ++i) {
                piece[i] = nullptr;
            }
        }

    public:
        // ����.
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;

        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;
    };

}