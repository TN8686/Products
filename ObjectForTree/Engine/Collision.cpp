#include "Collision.h"
#include "GameObject.h"

namespace Engine {

    bool AbstractCollitsion2D::getActive() const
    {
        if (!isActive_) {
            return false;
        }
        if (parent_ != nullptr) {
            if (!parent_->getActive()) {
                return false;
            }
        }

        return true;
    }

    bool AABBCollision2D::render(unsigned int color) const
    {
        auto aabb = getWorldAABB2D();

        DrawBox(static_cast<int>(aabb.position.x - aabb.halfLength.x), static_cast<int>(aabb.position.y - aabb.halfLength.y),
            static_cast<int>(aabb.position.x + aabb.halfLength.x), static_cast<int>(aabb.position.y + aabb.halfLength.y), color, false);

        return false;
    }

    // 親を辿ったワールド基準の座標、スケール情報を反映した矩形を返します.
    AABB2D AABBCollision2D::getWorldAABB2D() const
    {
        AABB2D aabb = aabb_;
        auto p = getParent();
        if (p != nullptr) {
            // スケールを反映.
            aabb.halfLength = aabb.halfLength * p->getTransform().getWorldScale();

            // 負の値だったら正の値に修正.
            if (aabb.halfLength.x < 0.0f) {
                aabb.halfLength.x *= -1;
            }
            if (aabb.halfLength.y < 0.0f) {
                aabb.halfLength.y *= -1;
            }
            aabb.position = getWorldPosition();
        }

        return aabb;
    }

    // 親を辿ったワールド基準の座標を返します.
    Float2 AABBCollision2D::getWorldPosition() const
    {
        Float2 pos = aabb_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform(); // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();		// スケールを反映.
            
            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();     // 親のワールド座標を反映.

        }
        return pos;
    }

    bool CircleCollision2D::render(unsigned int color) const
    {
        auto pos = getWorldPosition();

        DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(circle_.radius), color, false);

        return false;
    }

    // 親を辿ったワールド基準の座標を反映した円を返します.
    Circle2D CircleCollision2D::getWorldCircle2D() const
    {
        Circle2D circle = circle_;
        auto p = getParent();
        if (p != nullptr) {
            circle.position = getWorldPosition();
        }

        return circle;
    }

    // 親を辿ったワールド基準の座標を返します.
    Float2 CircleCollision2D::getWorldPosition() const
    {
        Float2 pos = circle_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform(); // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();		// スケールを反映.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();     // 親のワールド座標を反映.

        }
        return pos;
    }
}
