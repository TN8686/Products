#pragma once
#include "../Engine/GameObject.h"

namespace Object {

    class Player : public Engine::GameObject
    {
    private:
        // �萔.
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

        // �����o�ϐ�.
    private:
        static Player* instance_;   // �Q�Ɨp �v���C���[�͓���ŊȒP�ɎQ�Ɖ\�ɂ��Ă����ƕ֗�����Ȃ�����.
        float speed_;
        int halfLength_;
        Direction direction_;
        bool isHit_[EDGE_NUM];
        Engine::Float2 vector_;

        Engine::AABBCollision2D* collision_;
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;

        // �R���X�g���N�^.
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

        // ����.
    public:
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;
        
        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;

        //����.
    public:
        // �v���C���[�̏����擾���܂�.
        static Player* getInstance() { return instance_; }
    };

}
