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
    // �_.
    ////////
    Vector2 PointCollision2D::getWorldPosition() const
    {
        Vector2 pos = position_;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();	        	// �X�P�[���𔽉f.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();             // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }


    ///////////////
    // �����s��`.
    ///////////////
    bool AABBCollision2D::render(unsigned int color) const
    {
        auto aabb = getWorldAABB2D();

        DrawBox(static_cast<int>(aabb.position.x - aabb.halfLength.x), static_cast<int>(aabb.position.y - aabb.halfLength.y),
            static_cast<int>(aabb.position.x + aabb.halfLength.x), static_cast<int>(aabb.position.y + aabb.halfLength.y), color, false);

        return true;
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
    Vector2 AABBCollision2D::getWorldPosition() const
    {
        Vector2 pos = aabb_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();		        // �X�P�[���𔽉f.
            
            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();             // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }

    ///////
    // �~.
    ///////
    bool CircleCollision2D::render(unsigned int color) const
    {
        auto pos = getWorldPosition();

        DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), static_cast<int>(circle_.radius), color, false);

        return true;
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
    Vector2 CircleCollision2D::getWorldPosition() const
    {
        Vector2 pos = circle_.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();		        // �X�P�[���𔽉f.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();             // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }

    /////////////
    // �J�v�Z��.
    /////////////
    bool CapsuleCollision2D::render(unsigned int color) const
    {
        auto c = getWorldCapsule2D();
        auto begin = c.segment.position;
        auto end = c.segment.getEndPoint();
        
        DrawCircle(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(c.radius), color, false);
        DrawCircle(static_cast<int>(end.x), static_cast<int>(end.y), static_cast<int>(c.radius), color, false);

        auto vec = c.segment.vector;
        auto r = acosf(vec.dot({ 1.0f, 0.0f }) / vec.length()); // �x�N�g����0�x�x�N�g���̓��ς����Ɋp�x�𓾂�.

        if (vec.cross({ 1.0f, 0.0f }) > 0.0f) {                 // �O�ς̒l�����̏ꍇ�͔����v���ɉ�].
            r = -r;
        }

        Vector2 relative = RotationZrad({ 0, c.radius }, r);  // ���Βl���o���ĉ��Z.
        begin = begin + relative;
        end = end + relative;
        DrawLine(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(end.x), static_cast<int>(end.y), color);

        // ���Βl���񌸎Z���鎖�ł�������̐��̍��W���o��
        begin = begin - relative - relative;
        end = end - relative - relative;
        DrawLine(static_cast<int>(begin.x), static_cast<int>(begin.y), static_cast<int>(end.x), static_cast<int>(end.y), color);
        return true;
    }

    // �e��H�������[���h��̃J�v�Z����Ԃ��܂��B
    // �������A���a�ɃX�P�[���͔��f����܂���B
    Capsule2D CapsuleCollision2D::getWorldCapsule2D() const
    {
        auto p = getParent();
        if (p == nullptr) {
            return capsule_;    // �e��������΂��̂܂ܕԂ�.
        }

        Capsule2D c = capsule_;
        auto ptf = getParent()->getTransform();         // �e�̃g�����X�t�H�[�����擾.
        auto s = ptf.getWorldScale();		            // �X�P�[�����擾.
        auto r = ptf.getWorldRotation().z;              // ��]���擾.

        c.segment.position = c.segment.position * s;            // ���W�ɃX�P�[���𔽉f.
        c.segment.position = RotationZ(c.segment.position, r);	// ���W�ɉ�]�𔽉f.

        c.segment.vector = c.segment.vector * s;                // �x�N�g���ɃX�P�[���𔽉f.
        c.segment.vector = RotationZ(c.segment.vector, r);	    // �x�N�g���ɉ�]�𔽉f.
        
        c.segment.position = c.segment.position + ptf.getWorldPosition();   // �e�̃��[���h���W�𔽉f.
        return c;

    }

    Vector2 CapsuleCollision2D::getWorldPosition() const
    {
        Vector2 pos = capsule_.segment.position;
        if (getParent() != nullptr) {
            auto ptf = getParent()->getTransform();         // �e�̃g�����X�t�H�[�����擾.

            pos = pos * ptf.getWorldScale();		        // �X�P�[���𔽉f.

            pos = RotationZ(pos, ptf.getWorldRotation().z);	// ��]�𔽉f.

            pos = pos + ptf.getWorldPosition();             // �e�̃��[���h���W�𔽉f.

        }
        return pos;
    }

}
