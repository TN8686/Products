#pragma once
#include "../Engine/GameObject.h"

namespace Object {

    class Player : public Engine::GameObject
    {
    private:
        // 定数.
        static constexpr float INIT_SPEED = 6.0f;

        enum Edge {
            EDGE_TOP,
            EDGE_LEFT,
            EDGE_RIGHT,
            EDGE_BOTTOM,
            EDGE_NUM
        };

        enum Direction {
            DRC_NON = -1,
            DRC_RIGHT,
            DRC_BOTTOM_RIGHT,
            DRC_BOTTOM,
            DRC_BOTTOM_LEFT,
            DRC_LEFT,
            DRC_TOP_LEFT,
            DRC_TOP,
            DRC_TOP_RIGHT,
            DRC_NUM
        };

        // メンバ変数.
    private:
        static Player* instance_;   // 参照用 プレイヤーは特例で簡単に参照可能にしておくと便利じゃないかと.
        float speed_;
        int halfLength_;
        Direction direction_;
        bool isHit_[EDGE_NUM];
        Engine::Float2 vector_;

        Engine::AABBCollision2D* collision_;
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;

        // コンストラクタ.
    public:
        Player()
            : Player("Player")
        {}
        Player(std::string name)
            : Engine::GameObject(name)
            , speed_(INIT_SPEED)
            , halfLength_(10)
            , direction_(DRC_NON)
            , collision_(nullptr)
            , isHit_{false,false, false, false}
        {}

        // 操作.
    public:
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;
        
        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;

        //属性.
    public:
        // プレイヤーの情報を取得します.
        static Player* getInstance() { return instance_; }
    };

}
