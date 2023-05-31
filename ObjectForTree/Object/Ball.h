#pragma once
#include "../Engine/GameObject.h"

namespace Object {

	class Ball : public Engine::GameObject {
        // �萔.
    public:
        static float constexpr BREAK_SPEED = 10.0f * 10.0f;

    private:
        Engine::Float2 velocity_;               // �����x.
        Engine::Float2 tempVelocity_;           // �ꎞ�I�ȉ����x.
        Engine::AABBCollision2D* collision_;    // �R���W����.
        Engine::ObjectForTree<Engine::AABBCollision2D> oft;

        float speedSquare_; // ���݂̒����ړ����x�̓��.
        float length_;      // �q���̒���.
        int r_;             // �~�̔��a.

        int wait_;          // �q�b�g�X�g�b�v�̌��ݎc��t���[��.

        int hitSeHandle_;   // se�n���h��.

        // �R���X�g���N�^.
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

        // ����.
    public:
        bool load() override;
        bool init() override;
        bool update() override;
        bool render() override;
        bool unload() override;

        void onAABBStay2D(Engine::AABBCollision2D* const collision) override;

        // ����.
    public:
        float getSpeedSquare() { return speedSquare_; }
	};

}