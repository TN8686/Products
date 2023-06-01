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

    // �e��H�������[���h��̍��W�A�X�P�[�����𔽉f������`��Ԃ��܂�.
    AABB2D AABBCollision2D::getWorldAABB2D() const
    {
        AABB2D aabb = aabb_;
        auto p = getParent();
        if (p != nullptr) {
            // �X�P�[���𔽉f.
            aabb.halfLength = aabb.halfLength * p->getTransform().getWorldScale();

            // ���̒l�������琳�̒l�ɏC��.
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

    // �e��H�������[���h��̍��W��Ԃ��܂�.
    Float2 AABBCollision2D::getWorldPosition() const
    {
        Float2 pos = aabb_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform(); // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();		// �X�P�[���𔽉f.
            
            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();     // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }

    bool CircleCollision2D::render(unsigned int color) const
    {
        auto pos = getWorldPosition();

        DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(circle_.radius), color, false);

        return false;
    }

    // �e��H�������[���h��̍��W�𔽉f�����~��Ԃ��܂�.
    Circle2D CircleCollision2D::getWorldCircle2D() const
    {
        Circle2D circle = circle_;
        auto p = getParent();
        if (p != nullptr) {
            circle.position = getWorldPosition();
        }

        return circle;
    }

    // �e��H�������[���h��̍��W��Ԃ��܂�.
    Float2 CircleCollision2D::getWorldPosition() const
    {
        Float2 pos = circle_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform(); // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();		// �X�P�[���𔽉f.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();     // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }
}
