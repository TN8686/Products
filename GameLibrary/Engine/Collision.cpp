#include "Collision.h"
#include "GameObject.h"

namespace Engine {

    bool Collision2D::getActive() const
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
    
    bool PointCollision2D::render(unsigned int color) const
    {
        auto pos = getWorldPosition();
        DrawPixel(static_cast<int>(pos.x), static_cast<int>(pos.y), color);

        return true;
    }

    ////////
    // 点.
    ////////
    Vector2 PointCollision2D::getWorldPosition() const
    {
        Vector2 pos = position_;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();	        	// スケールを反映.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();             // 親のワールド座標を反映.

        }
        return pos;
    }


    ///////////////
    // 軸並行矩形.
    ///////////////
    bool AABBCollision2D::render(unsigned int color) const
    {
        auto aabb = getWorldAABB2D();

        DrawBox(static_cast<int>(aabb.position.x - aabb.halfLength.x), static_cast<int>(aabb.position.y - aabb.halfLength.y),
            static_cast<int>(aabb.position.x + aabb.halfLength.x), static_cast<int>(aabb.position.y + aabb.halfLength.y), color, false);

        return true;
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
    Vector2 AABBCollision2D::getWorldPosition() const
    {
        Vector2 pos = aabb_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();		        // スケールを反映.
            
            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();             // 親のワールド座標を反映.

        }
        return pos;
    }

    ///////
    // 円.
    ///////
    bool CircleCollision2D::render(unsigned int color) const
    {
        auto pos = getWorldPosition();

        DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(circle_.radius), color, false);

        return true;
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
    Vector2 CircleCollision2D::getWorldPosition() const
    {
        Vector2 pos = circle_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();		        // スケールを反映.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();             // 親のワールド座標を反映.

        }
        return pos;
    }

    /////////////
    // カプセル.
    /////////////
    bool CapsuleCollision2D::render(unsigned int color) const
    {
        auto c = getWorldCapsule2D();
        auto begin = c.segment.position;
        auto end = c.segment.getEndPoint();
        
        DrawCircle(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(c.radius), color, false);
        DrawCircle(static_cast<int>(end.x), static_cast<int>(end.y), static_cast<int>(c.radius), color, false);

        auto vec = c.segment.vector;
        auto r = acosf(vec.dot({ 1.0f, 0.0f }) / vec.length()); // ベクトルと0度ベクトルの内積を元に角度を得る.

        if (vec.cross({ 1.0f, 0.0f }) > 0.0f) {                 // 外積の値が正の場合は反時計回りに回転.
            r = -r;
        }

        Vector2 relative = RotationZrad({ 0, c.radius }, r);  // 相対値を出して加算.
        begin = begin + relative;
        end = end + relative;
        DrawLine(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(end.x), static_cast<int>(end.y), color);

        // 相対値を二回減算する事でもう一方の線の座標も出す
        begin = begin - relative - relative;
        end = end - relative - relative;
        DrawLine(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
        return true;
    }

    // 親を辿ったワールド基準のカプセルを返します。
    // ただし、半径にスケールは反映されません。
    Capsule2D CapsuleCollision2D::getWorldCapsule2D() const
    {
        auto p = getParent();
        if (p == nullptr) {
            return capsule_;    // 親が無ければそのまま返す.
        }

        Capsule2D c = capsule_;
        auto ptf = getParent()->getTransform();         // 親のトランスフォームを取得.
        auto s = ptf.getWorldScale();		            // スケールを取得.
        auto r = ptf.getWorldRotation().z;              // 回転を取得.

        c.segment.position = c.segment.position * s;            // 座標にスケールを反映.
        c.segment.position = RotationZ(c.segment.position, r);	// 座標に回転を反映.

        c.segment.vector = c.segment.vector * s;                // ベクトルにスケールを反映.
        c.segment.vector = RotationZ(c.segment.vector, r);	    // ベクトルに回転を反映.
        
        c.segment.position = c.segment.position + ptf.getWorldPosition();   // 親のワールド座標を反映.
        return c;

    }

    Vector2 CapsuleCollision2D::getWorldPosition() const
    {
        Vector2 pos = capsule_.segment.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // 親のトランスフォームを取得.

            pos = pos * ptf.getWorldScale();		        // スケールを反映.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// 回転を反映.

            pos = pos + ptf.getWorldPosition();             // 親のワールド座標を反映.

        }
        return pos;
    }

}
