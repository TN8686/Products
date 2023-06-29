#include <algorithm>
#include <cmath>
#include <cassert>

#include "HitChecker.h"
#include "GameObject.h"

#include "CollisionTrees.h"


namespace Engine {

	using namespace Utility;

	HitChecker HitChecker::instace_;

	HitChecker::~HitChecker() {
	}

	bool HitChecker::initialize()
	{
		if (!l4tree_.init(4, -128, -128, (float)WindowSize::WIDTH + 128, (float)WindowSize::HEIGHT + 128)) {
			return false;	// �l���ؐ������s.
		}

		return true;
	}

	// �S�Ă̓����蔻��̃`�F�b�N���s���܂�.
	// combinationList����Ƀ��C���[���m�Ńq�b�g�`�F�b�N���s���܂�.
	// ���̃Q�[�����[�v�ň�x�������s���Ă�������.
	bool HitChecker::allHitCheck()
	{
		static CollisionList<Collision2D>* pList = nullptr;
		auto colNum = l4tree_.getAllCollisionList(&pList);
		colNum /= 2;	// ���X�g�͓��g�Ȃ̂Ń`�F�b�N�񐔂͔���.

		if (pList == nullptr) {
			return false;
		}

		Collision2D** pRoot = pList->getRootPtr();
		
		for (unsigned int i = 0; i < colNum; ++i) {

			auto a = pRoot[i * 2];
			auto b = pRoot[i * 2 + 1];

			if (!a->getActive() || !b->getActive()) {
				continue;
			}

			// a����type�������Ȓl�ɂȂ�悤�ɂ���.
			if (a->getType() > b->getType()) {
				auto tmp = a;
				a = b;
				b = tmp;
			}

			auto aType = a->getType();
			int num1 = COLTYPE_NUM * (COLTYPE_NUM + 1) / 2;									// COLTYPE_NUM�̑g�����̐�.
			int num2 = (COLTYPE_NUM - aType) * (COLTYPE_NUM - aType + 1) / 2;				// COLTYPE_NUM - a�̑g�����̐�.
			HitFuncID id = static_cast<HitFuncID>((num1 - num2) + (b->getType() - aType));	// num1 - num2 �ŁA�g����aa��ID���o���A������b - a�𑫂����őg����ab�̈ʒu���o���Ă���.

			// ��ނɉ����ăR���W�������_�E���L���X�g���q�b�g�`�F�b�N������U�蕪��.
			switch (id)
			{
			case POINT_POINT:
				if (PointOnPoint(dynamic_cast<PointCollision2D*>(a)->getPoint2D(), dynamic_cast<PointCollision2D*>(b)->getWorldPosition())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_AABB:
				if (PointOnAABB(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<AABBCollision2D*>(b)->getWorldAABB2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_CIRCLE:
				if (PointOnCircle(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case POINT_CAPSULE:
				if (PointOnCapsule(dynamic_cast<PointCollision2D*>(a)->getWorldPosition(), dynamic_cast<CapsuleCollision2D*>(b)->getWorldCapsule2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_AABB:
				if (AABBOnAABB(dynamic_cast<AABBCollision2D*>(a)->getWorldAABB2D(), dynamic_cast<AABBCollision2D*>(b)->getWorldAABB2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_CIRCLE:
				if (AABBOnCircle(dynamic_cast<AABBCollision2D*>(a)->getWorldAABB2D(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case AABB_CAPSULE:
				// TODO
				//MessageBox(NULL, "AABB On Capsule is not implemented.", NULL, MB_OK);
				break;

			case CIRCLE_CIRCLE:
				if (CircleOnCircle(dynamic_cast<CircleCollision2D*>(a)->getWorldCircle2D(), dynamic_cast<CircleCollision2D*>(b)->getWorldCircle2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case CIRCLE_CAPSULE:
				if (CircleOnCapsule(dynamic_cast<CircleCollision2D*>(a)->getWorldCircle2D(), dynamic_cast<CapsuleCollision2D*>(b)->getWorldCapsule2D())) {
					a->getParent()->onCollisionStay2D(a, b);
					b->getParent()->onCollisionStay2D(b, a);
				}
				break;

			case CAPSULE_CAPSULE:
				// TODO
				//MessageBox(NULL, "Capsule On Capsule is not implemented.", NULL, MB_OK);
				break;

			default:
				break;
			}

		}
		return true;
	}


	// TODO �S�Ă̓����蔻���`�悵�܂�.
	bool HitChecker::render()
	{
		unsigned int c = 0;
		for (int i = 0; i < OFTLAYER_NUM; ++i) {

			// ���C���[�ŐF��ς���.
			c = COLOR[i];

			// TODO �o�^���ꂽ���X�g����`��.
			/*
			for (auto& e : aabbCls2DList_[i]) {
				if (!e->getActive()) {
					continue;
				}
				e->render(c);
			}*/
		}
		return true;
	}
	

	// �_�Ɛ����̍ŒZ�����i2D�j.
	// position : �_.
	// line : ����.
	// �߂�l: �ŒZ����.
	float HitChecker::pointSegmentDistance2D(const Vector2& position, const Segment2D& line)
	{
		float t;
		Vector2 h;
		return pointSegmentDistance2D(position, line, h, t);
	}

	// �_�Ɛ����̍ŒZ�����i2D�j.
	// position : �_.
	// line : ����.
	// mp : �_�ɑ΂��čł��߂�������̍��W�i�߂�l�j.
	// m : �x�N�g���̔�i�߂�l�j.
	// �߂�l: �ŒZ����.
	float HitChecker::pointSegmentDistance2D(const Vector2& position, const Segment2D& line, Vector2& mp, float& m)
	{
		/*
		* line.vector���x�N�g���� a �Ƃ���.
		* line.position���n�_�Ƃ��� �_position�܂ł̃x�N�g���� b �Ƃ���.
		* �x�N�g��a�̑傫����1�Ƃ������A�n�_����mp�܂ł̑傫���̔��m�Ƃ���.
		* position�ɑ΂��čł��߂�������̍��W��mp�Ƃ���imp = a * m�j.
		*
		* dot(a, b - mp) = 0
		* dot(a, b - a * m) = 0
		* ���z�@������
		* dot(a, b) + dot(a, -a * m) = 0
		* �萔�{����
		* dot(a, b) + m * -dot(a, a) = 0
		* m = -dot(a, b) / -dot(a, a)
		* m = dot(a, b) / dot(a, a)
		* m = dot(a, b) / |a||a|
		*/

		float lenSqV = line.vector.lengthSquare();	// �����̓��i�������g�Ƃ̓��ςƓ����j���o��.
		m = 0.0f;

		if (lenSqV > 0.0f) {
			m = line.vector.dot(position - line.position) / lenSqV;	// �����̃x�N�g���ƁA�����n�_����_�܂ł̃x�N�g���̓��ς��������Ŋ��� m = dot(a, b) / |a||a|.
		}

		mp = line.position + m * line.vector;

		return (mp - position).length();	// �ŒZ����.
	}

	bool HitChecker::PointOnPoint(Vector2 a, Vector2 b)
	{
		return (fabs(a.x - b.x) < EPSILON) && (fabs(a.y - b.y) < EPSILON);		// �������e�덷�Ɣ�r.
	}

	bool HitChecker::PointOnAABB(Vector2 a, AABB2D b)
	{
		Rect rect = b;

		return  a.y < rect.bottomRight.y&&
				a.y > rect.topLeft.y &&
				a.x < rect.bottomRight.x&&
				a.x > rect.topLeft.x;
	}

	bool HitChecker::PointOnCircle(Vector2 a, Circle2D b)
	{

		// ���W�Ɖ~�̌��_�̋��������a�̖�����������.
		return	(a.x - b.position.x) * (a.x - b.position.x) +
				(a.y - b.position.y) * (a.y - b.position.y) <
				(b.radius * b.radius);
	}

	bool HitChecker::PointOnCapsule(Vector2 a, Capsule2D b)
	{
		
		auto v1 = a - b.segment.position;							// �n�_����_�ւ̃x�N�g��.
		if (v1.isSharpAngle(b.segment.vector)) {					// �n�_����̓�̃x�N�g�����s�p��.
			Vector2 v2 = -b.segment.vector;							// �I�_����n�_�ւ̃x�N�g��.
			Vector2 v3 = a - b.segment.getEndPoint();				// �I�_����_�ւ̃x�N�g��.
			if (v2.isSharpAngle(v3)) {								// �I�_����̓�̃x�N�g�����s�p��.
				auto ref = pointSegmentDistance2D(a, b.segment);	// �����s�p�Ȃ̂Ő����Ƃ̋������o��.
				return (ref < b.radius);							// ���������a������������Փ�.
			}
			else {	// �I�_���̓݊p.
				return PointOnCircle(a, Circle2D(b.segment.getEndPoint(), b.radius));	// �I�_�𒆐S�Ƃ����~�Ƃ̏Փ˔���.
			}
		}
		else {	// �n�_���̓݊p.
			return PointOnCircle(a, Circle2D(b.segment.position, b.radius));	// �n�_�𒆐S�Ƃ����~�Ƃ̏Փ˔���.
		}
	}

	bool HitChecker::AABBOnAABB(AABB2D a, AABB2D b)
	{

		// ��`�ɕϊ�.
		Rect rect1, rect2;
		rect1 = a;
		rect2 = b;

		return	rect1.topLeft.y < rect2.bottomRight.y&&
				rect1.bottomRight.y > rect2.topLeft.y &&
				rect1.topLeft.x < rect2.bottomRight.x&&
				rect1.bottomRight.x > rect2.topLeft.x;
	}

	bool HitChecker::AABBOnCircle(AABB2D a, Circle2D b)
	{
		Rect rect = a;

		// �~�̌��_�ɍł��߂���`�̓_��T��.
		Vector2 pos;
		pos.x = std::min<float>(std::max<float>(rect.topLeft.x, b.position.x), rect.bottomRight.x);	// ��`�̍���x�Ɖ~�̌��_�ł��E�ɂ���������A����ɂ���Ƌ�`�̉E��x�ł�荶�ɂ��������鎖�ŁA�ł��߂�x���W���o��.
		pos.y = std::min<float>(std::max<float>(rect.topLeft.y, b.position.y), rect.bottomRight.y);	// ���������ōł��߂�y�����.

		// ���̍��W�Ɖ~�̔��a�ŐڐG������s��.
		return	(pos.x - b.position.x) * (pos.x - b.position.x) +
				(pos.y - b.position.y) * (pos.y - b.position.y) <
				(b.radius * b.radius);
	}

	bool HitChecker::CircleOnCircle(Circle2D a, Circle2D b)
	{
		// �~1�̌��_����~2�̌��_�܂ł̋������o���̔��a�̍��v������������.
		return	(a.position.x - b.position.x) * (a.position.x - b.position.x) +
				(a.position.y - b.position.y) * (a.position.y - b.position.y) <
				(a.radius + b.radius) * (a.radius + b.radius);
	}

	bool HitChecker::CircleOnCapsule(Circle2D a, Capsule2D b)
	{
		auto v1 = a.position - b.segment.position;							// �n�_����_�ւ̃x�N�g��.
		if (v1.isSharpAngle(b.segment.vector)) {							// �n�_����̓�̃x�N�g�����s�p��.
			Vector2 v2 = -b.segment.vector;									// �I�_����n�_�ւ̃x�N�g��.
			Vector2 v3 = a.position - b.segment.getEndPoint();				// �I�_����_�ւ̃x�N�g��.
			if (v2.isSharpAngle(v3)) {										// �I�_����̓�̃x�N�g�����s�p��.
				auto ref = pointSegmentDistance2D(a.position, b.segment);	// �����s�p�Ȃ̂Ő����Ƃ̋������o��.
				return (ref < b.radius + a.radius);							// ���������a�̍��v������������Փ�.
			}
			else {	// �I�_���̓݊p.
				return CircleOnCircle(a, Circle2D(b.segment.getEndPoint(), b.radius));	// �I�_�𒆐S�Ƃ����~�Ƃ̏Փ˔���.
			}
		}
		else {	// �n�_���̓݊p.
			return CircleOnCircle(a, Circle2D(b.segment.position, b.radius));	// �n�_�𒆐S�Ƃ����~�Ƃ̏Փ˔���.
		}
	}

	bool HitChecker::PointOnPoint(PointCollision2D a, PointCollision2D b)
	{
		return PointOnPoint(a.getWorldPosition(), b.getWorldPosition());
	}

	bool HitChecker::PointOnAABB(PointCollision2D a, AABBCollision2D b)
	{
		return  PointOnAABB(a.getWorldPosition(), b.getWorldAABB2D());
	}

	bool HitChecker::PointOnCircle(PointCollision2D a, CircleCollision2D b)
	{
		return	PointOnCircle(a.getWorldPosition(), b.getWorldCircle2D());
	}

	bool HitChecker::AABBOnAABB(AABBCollision2D a, AABBCollision2D b)
	{
		return	AABBOnAABB(a.getWorldAABB2D(), b.getWorldAABB2D());
	}

	bool HitChecker::AABBOnCircle(AABBCollision2D a, CircleCollision2D b)
	{
		return	AABBOnCircle(a.getWorldAABB2D(), b.getWorldCircle2D());
	}

	bool HitChecker::CircleOnCircle(CircleCollision2D a, CircleCollision2D b)
	{
		return	CircleOnCircle(a.getWorldCircle2D(), b.getWorldCircle2D());
	}

	bool HitChecker::CircleOnCapsule(CircleCollision2D a, CapsuleCollision2D b) {
		return CircleOnCapsule(a.getWorldCircle2D(), b.getWorldCapsule2D());
	}

	// TODO �n���h�����L���Ȃ��̂��ǂ������`�F�b�N���܂�.
	bool HitChecker::handleCheck(int handle)
	{
		if (handle == -1) {	// -1���������Ȓl�Ƃ��Ĉ���.
			return false;
		}

		/* TODO L4Tree���X�g�̃T�C�Y�`�F�b�N.
		if (handle < 0 || handle >= ) {
			return false;
		}*/

		return true;
	}
}