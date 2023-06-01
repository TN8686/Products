#include "WindowWall.h"

using namespace Engine;
using namespace Utility;

namespace Object {
	bool WindowWall::load() {
        // 座標指定.
        auto pos = getTransform().position;
        pos.x = static_cast<float>(WindowSize::WIDTH) / 2;
        pos.y = static_cast<float>(WindowSize::HEIGHT) / 2;
        setPosition(pos);

        // コリジョンを生成.
        for (int i = 0; i < WALL_NUM; ++i) {
            if (!CollisionManager::getInstance()->createAABBCollision(this, LAYER_WALL, collision_[i])) {
                return false;
            }
        }

        // 矩形を設定.
        collision_[0]->setAABB2D({ Float2(0.0f, static_cast<float>(-WindowSize::HEIGHT) / 2 - HALF_THICKNESS), Float2(static_cast<float>(WindowSize::WIDTH + 64),  HALF_THICKNESS) });
        collision_[1]->setAABB2D({ Float2(0.0f, static_cast<float>(WindowSize::HEIGHT) / 2 + HALF_THICKNESS), Float2(static_cast<float>(WindowSize::WIDTH + 64),  HALF_THICKNESS) });
        collision_[2]->setAABB2D({ Float2(static_cast<float>(-WindowSize::WIDTH) / 2 - HALF_THICKNESS, 0.0f), Float2(HALF_THICKNESS, static_cast<float>(WindowSize::HEIGHT + 64)) });
        collision_[3]->setAABB2D({ Float2(static_cast<float>(WindowSize::WIDTH) / 2 + HALF_THICKNESS, 0.0f), Float2(HALF_THICKNESS, static_cast<float>(WindowSize::HEIGHT + 64)) });

        return true;
    }
}