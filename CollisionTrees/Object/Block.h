#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/CollisionTrees.h"

namespace Object {

    class BlockPiece;

    class Block : public Engine::GameObject {
        // 定数.
    private:
        static constexpr int PIECE_NUM = 4;

        // メンバ変数.
    private:
        static int breakSeHandle_;  // 破壊SEハンドルの大本.
        static int blockObjNum_;    // ブロックオブジェクトの総数.

        Engine::AABBCollision2D* collision_;
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;
        int halfLength_;
        BlockPiece* piece[PIECE_NUM];
        int localBreakSeHandle_;                      // 個別の破壊SEハンドル.

        unsigned int color;

        // コンストラクタ.
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
        // 操作.
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;

        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;
    };

}