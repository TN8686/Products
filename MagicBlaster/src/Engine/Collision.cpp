#include "Collision.h"

namespace Collision {
/*
*�@2�̓_�̏Փ˔���.
*
* @param sa ���ӂ̓_�I�u�W�F�N�g.
* @param pa ���ӂ̓_�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̓_�I�u�W�F�N�g.
* @param pb �E�ӂ̓_�I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool PointOnPoint(Point2D sa, Float2 ra, Point2D sb, Float2 rb) {
		Float2 num = (sa + ra) - (sb + rb);
		if (fabsf(num.x) < _OX_EPSILON_ && fabsf(num.y) < _OX_EPSILON_) {
			return true;
		}
		return false;
	}

/*
*�@�_�Ɛ����̏Փ˔���.
*
* @param sa ���ӂ̓_�I�u�W�F�N�g.
* @param pa ���ӂ̓_�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̐����I�u�W�F�N�g.
* @param pb �E�ӂ̐����I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool PointOnSegment(Point2D sa, Float2 ra, Segment2D sb, Float2 rb) {

		sa = sa + ra;
		sb.p = sb.p + rb;
		Point2D segEnd = sb.getEndPoint();

		float l1 = sqrtf((segEnd.x - sb.p.x) * (segEnd.x - sb.p.x) + (segEnd.y - sb.p.y) * (segEnd.y - sb.p.y));	// �����̒���.
		float l2 = sqrtf((sa.x - sb.p.x) * (sa.x - sb.p.x) + (sa.y - sb.p.y) * (sa.y - sb.p.y));					// �����̎n�_����_�܂ł̒���

		float aa = fabsf((segEnd.x - sb.p.x) * (sa.x - sb.p.x) + (segEnd.y - sb.p.y) * (sa.y - sb.p.y) - (l1 * l2));
		// l2��l1�����Z���������ŁA����l1��l2�����s�ł���Γ_��������ɑ��݂��鎖�ɂȂ�.
		if (l1 >= l2 && fabsf((segEnd.x - sb.p.x) * (sa.x - sb.p.x) + (segEnd.y - sb.p.y) * (sa.y - sb.p.y) - (l1 * l2)) < _OX_EPSILON_) {
			return true;
		}
		return false;
	}

/*
*�@�_�Ɖ~�̏Փ˔���.
*
* @param sa ���ӂ̓_�I�u�W�F�N�g.
* @param pa ���ӂ̓_�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̉~�I�u�W�F�N�g.
* @param pb �E�ӂ̉~�I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool PointOnCircle(Point2D sa, Float2 ra, Circle2D sb, Float2 rb) {
		sa = sa + ra;
		sb.p = sb.p + rb;

		// �~�̌��_����_�܂ł̋��������a�ȉ���������.
		if ((sa.x - sb.p.x) * (sa.x - sb.p.x) + (sa.y - sb.p.y) * (sa.y - sb.p.y) < (sb.r * sb.r)) {
			return true;
		}
		return false;
	}

/*
*�@�_�Ƌ�`�̏Փ˔���.
*
* @param sa ���ӂ̓_�I�u�W�F�N�g.
* @param pa ���ӂ̓_�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̋�`�I�u�W�F�N�g.
* @param pb �E�ӂ̋�`�I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool PointOnRect(Point2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa = sa + ra;
		sb.p = sb.p + rb;

		// �_���A�E�Ӌ�`�̍��E�̊ԁA���A�E�Ӌ�`�̏㉺�̊Ԃɂ���ꍇ�A�Փ˂��Ă���
		if (sa.x > sb.p.x - sb.hl.x && sa.x <= sb.p.x + sb.hl.x && sa.y > sb.p.y - sb.hl.y && sa.y <= sb.p.y + sb.hl.y) {
			return true;
		}
		return false;
	
	}

	bool SegmentOnSegment(Segment2D sa, Float2 ra, Segment2D sb, Float2 rb);
	bool SegmentOnCircle(Segment2D sa, Float2 ra, Circle2D sb, Float2 rb);
	bool SegmentOnRect(Segment2D sa, Float2 ra, AABB2D sb, Float2 rb);

/*
*�@2�̉~�̏Փ˔���.
*
* @param sa ���ӂ̉~�I�u�W�F�N�g.
* @param pa ���ӂ̉~�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̉~�I�u�W�F�N�g.
* @param pb �E�ӂ̉~�I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool CircleOnCircle(Circle2D sa, Float2 ra, Circle2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;

		// �~1�̌��_����~2�̌��_�܂ł̋������o���̔��a�̍��v�ȉ���������.
		if ((sa.p.x - sb.p.x) * (sa.p.x - sb.p.x) + (sa.p.y - sb.p.y) * (sa.p.y - sb.p.y) <= (sa.r + sb.r) * (sa.r + sb.r)) {
			return true;
		}
		return false;
	}

	bool CircleOnRect(Circle2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;
		// �܂��͉~����ӂ̒�����2r�̋�`�Ƃ��ē����蔻����s���@�����ŏՓ˂��Ă���΁A���ۂɏՓ˂��Ă���\��������.
		if (RectOnRect({ sa.p, { sa.r, sa.r } }, { 0, 0 }, sb, { 0, 0 })) {

			// �~�̔��a�̕�X�����ɒ�����`�ƁAY�����ɒ�����`�����.
			AABB2D rect1, rect2;
			rect1 = rect2 = sb;

			// TODO ���S���W�̕��A1���炵�ĉ��Z�c�ł����͂��@����������ł��Ă͂���̂ł����ƌ���.
			rect1.hl.x += sa.r - 1;
			rect2.hl.y += sa.r - 1;

			// ���̋�`�Ɖ~�̒��S���W�̔�������A�����Ă���Ώ��Ȃ��Ƃ��Փ˂��Ă���.
			if (PointOnRect(sa.p, { 0, 0 }, rect1, { 0, 0 }) || PointOnRect(sa.p, { 0, 0 }, rect2, { 0, 0 })) {
				return true;
			}
			else {
				// �Փ˂��Ă��Ȃ������ꍇ�A��`�̎l���̍��W�Ɖ~�̔�������A�����Ă���ΏՓ˂��Ă���@����ł������Ă��Ȃ���ΏՓ˂��Ă��Ȃ�.
				if (sa.p.x <= sb.p.x) {		// ��.
					if (sa.p.y <= sb.p.y) {	// ����.
						return PointOnCircle({ sb.p.x - sb.hl.x + 1, sb.p.y - sb.hl.y + 1 }, { 0, 0 }, sa, { 0, 0 });
					}
					else {					// ����.
						return PointOnCircle({ sb.p.x - sb.hl.x + 1, sb.p.y + sb.hl.y }, { 0, 0 }, sa, { 0, 0 });
					}
				}
				else {						// �E.
					if (sa.p.y <= sb.p.y) {	// �E��.
						return PointOnCircle({ sb.p.x + sb.hl.x, sb.p.y - sb.hl.y + 1 }, { 0, 0 }, sa, { 0, 0 });
					}
					else {					// �E��.
						return PointOnCircle({ sb.p.x + sb.hl.x, sb.p.y + sb.hl.y }, { 0, 0 }, sa, { 0, 0 });
					}
				}
			}
		}
		return false;
	}

/*
*�@2�̋�`�̏Փ˔���.
*
* @param sa ���ӂ̋�`�I�u�W�F�N�g.
* @param pa ���ӂ̋�`�I�u�W�F�N�g�̍��W.
* @param sb �E�ӂ̋�`�I�u�W�F�N�g.
* @param pb �E�ӂ̋�`�I�u�W�F�N�g�̍��W.
*
* @retval true  �Փ˂��Ă���.
* @retval false �Փ˂��Ă��Ȃ�.
*/
	bool RectOnRect(AABB2D sa, Float2 ra, AABB2D sb, Float2 rb) {
		sa.p = sa.p + ra;
		sb.p = sb.p + rb;

		struct {
			float top, bottom, right, left;
		}rect1, rect2;

		rect1.top = sa.p.y - sa.hl.y;
		rect1.bottom = sa.p.y + sa.hl.y;
		rect1.right = sa.p.x + sa.hl.x;
		rect1.left = sa.p.x - sa.hl.x;

		rect2.top = sb.p.y - sb.hl.y;
		rect2.bottom = sb.p.y + sb.hl.y;
		rect2.right = sb.p.x + sb.hl.x;
		rect2.left = sb.p.x - sb.hl.x;

		// ���Ӌ�`�̏�ӂ��E�Ӌ�`�̉��ӂ�荂��.
		// ���Ӌ�`�̉��ӂ��E�Ӌ�`�̏�ӂ��Ⴍ.
		// ���Ӌ�`�̍��ӂ��E�Ӌ�`�̉E�ӂ�荶.
		// ���Ӌ�`�̉E�ӂ��E�Ӌ�`�̍��ӂ��E�̎��A�Փ˂��Ă���.
		if (rect1.top < rect2.bottom &&
			rect1.bottom > rect2.top &&
			rect1.left < rect2.right &&
			rect1.right > rect2.left) {
			return true;
		}

		return false;
	}

	/// <summary>
	/// ��������֐�.
	/// ��`�R���W�����������ɁA�X�v���C�g�̒l��ݒ肵�܂�.
	/// </summary>
	/// <param name="sprite">i �l��ݒ肷��X�v���C�g</param>
	/// <param name="collision">o ��`�R���W����</param>
	/// <param name="position">o ���S���W</param>
	void RectToSprite(Sprite *sprite, AABB2D collision, Float2 position) {
		sprite->pos.x = position.x + collision.p.x;
		sprite->pos.y = position.y + collision.p.y;

		sprite->scale = { 1.0f ,1.0f };

		sprite->rectOffset.left = -collision.hl.x;
		sprite->rectOffset.right = collision.hl.x;

		sprite->rectOffset.top = -collision.hl.y;
		sprite->rectOffset.bottom = collision.hl.y;
	}
}