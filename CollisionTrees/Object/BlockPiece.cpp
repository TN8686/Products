#include "BlockPiece.h"
#include "../Utility/Reference.h"

#include<random>

using namespace Engine;

namespace Object {

    bool BlockPiece::init()
    {
        GameObject::init();

        // 乱数生成.
        std::random_device rnd;
        std::mt19937 mt(rnd());
        std::uniform_int_distribution<> rand(-100, 100);

        // 加速度と回転速度をランダムに決定.
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

        // 速度と回転を反映.
        auto pos = getTransform().position;
        pos = pos + vector_;
        setPosition(pos);

        auto r = getTransform().rotation;
        r.z += rotationVel_;
        setRotation(r);

        vector_.y += 0.2f;  // 重力のイメージ.

        // 20フレーム経過したら点滅.
        if (getLifeTime() > 20) {
            setDrawActive(!getDrawActive());
        }

        // 40フレーム経過したら消滅.
        if (getLifeTime() > 40) {
            setActive(false);
        }

        /*
        // 画面外に落下したら消滅.
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

        // スケールを反映させた頂点情報を作る.
        Float2 vertex[4] = {
            { -halfLength_ * s.x, -halfLength_ * s.y },
            { halfLength_ * s.x, -halfLength_ * s.y },
            { halfLength_ * s.x, halfLength_ * s.y },
            { -halfLength_ * s.x, halfLength_ * s.y },
        };

        // 回転させる.
        for (int i = 0; i < 4; ++i) {
            vertex[i] = RotationZ(vertex[i], r);
        }

        // ワールド座標を加算して描画.
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
