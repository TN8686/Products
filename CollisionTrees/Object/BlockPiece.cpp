#include "BlockPiece.h"
#include "../Utility/Reference.h"

#include<random>

using namespace Engine;

namespace Object {

    bool BlockPiece::init()
    {
        GameObject::init();

        // ��������.
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(-100, 100);

        // �����x�Ɖ�]���x�������_���Ɍ���.
        vector_.x = static_cast<float>(rand(mt)) * 0.05f;
        vector_.y = static_cast<float>(rand(mt)) * 0.05f;
        rotationVel_ = static_cast<float>(rand(mt)) * 0.2f;

        setActive(false);
        setDrawActive(true);
        return true;
    }

    bool BlockPiece::update()
    {
        GameObject::update();

        // ���x�Ɖ�]�𔽉f.
        auto pos = getTransform().position;
        pos = pos + vector_;
        setPosition(pos);

        auto r = getTransform().rotation;
        r.z += rotationVel_;
        setRotation(r);

        vector_.y += 0.2f;  // �d�͂̃C���[�W.

        // 20�t���[���o�߂�����_��.
        if (getLifeTime() > 20) {
            setDrawActive(!getDrawActive());
        }

        // 40�t���[���o�߂��������.
        if (getLifeTime() > 40) {
            setActive(false);
        }

        /*
        // ��ʊO�ɗ������������.
        if (pos.y > Utility::WindowSize::HEIGHT) {
            setActive(false);
        }
        */

        return true;
    }

    bool BlockPiece::render()
    {
        auto pos = getTransform().getWorldPosition();
        auto r = getTransform().getWorldRotation().z;
        auto s = getTransform().getWorldScale();

        // �X�P�[���𔽉f���������_�������.
        Float2 vertex[4] = {
            { -halfLength_ * s.x, -halfLength_ * s.y },
            { halfLength_ * s.x, -halfLength_ * s.y },
            { halfLength_ * s.x, halfLength_ * s.y },
            { -halfLength_ * s.x, halfLength_ * s.y },
        };

        // ��]������.
        for (int i = 0; i < 4; ++i) {
            vertex[i] = RotationZ(vertex[i], r);
        }

        // ���[���h���W�����Z���ĕ`��.
        DrawQuadrangle(
            (int)(vertex[0].x + pos.x), (int)(vertex[0].y + pos.y),
            (int)(vertex[1].x + pos.x), (int)(vertex[1].y + pos.y),
            (int)(vertex[2].x + pos.x), (int)(vertex[2].y + pos.y),
            (int)(vertex[3].x + pos.x), (int)(vertex[3].y + pos.y),
            GetColor(0x80, 0x80, 0x80), true);


        DrawQuadrangle(
            (int)(vertex[0].x + pos.x), (int)(vertex[0].y + pos.y),
            (int)(vertex[1].x + pos.x), (int)(vertex[1].y + pos.y),
            (int)(vertex[2].x + pos.x), (int)(vertex[2].y + pos.y),
            (int)(vertex[3].x + pos.x), (int)(vertex[3].y + pos.y),
            GetColor(0x40, 0x40, 0x40), false);

        return true;
    }

}
