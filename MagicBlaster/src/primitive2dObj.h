#pragma once

#include "Engine/primitive2d.h"
#include "Engine/Sprite.h"
#include <vector>

class GameObj;

// TODO ����͌ʂ̃`�F�b�N�̒��ő������肵�Ă��邪�A�����I�ɔ��肵�����̈���O���b�h�����ăq�b�g�`�F�b�N���s��.
class AABB2DObj : public AABB2D {
protected:
	// �ÓI�����o.
	static std::vector<AABB2DObj*> _objList;			// ���݂��Ă���S�ẴR���W�����I�u�W�F�N�g�̃��X�g.
	static std::vector<std::size_t> _freeIDList;		// _objList���́A�g�p��ɃI�u�W�F�N�g���j������ċ󂢂��C���f�b�N�X�̃��X�g.
	static std::vector<Sprite> _sprList;				// �X�v���C�g���X�g.
	static std::vector<Sprite> _dummySprList;			// �_�~�[�X�v���C�g���X�g ��������ɂ͂������n��.
	static bool _isDraw;								// �������邩�ۂ��̃t���O.

	// �����o.
	const GameObj* _pParent;							// �e�I�u�W�F�N�g.
	bool _onActive;										// �A�N�e�B�u���.
	std::size_t _index;									// _objList���Ō��ݎg�p���Ă���C���f�b�N�X.

	/// <summary>
	/// ��`�R���W�����������ɁA�X�v���C�g�̒l��ݒ肵�܂�.
	/// </summary>
	/// <param name="sprite">o �l��ݒ肷��X�v���C�g</param>
	/// <param name="collision">i ��`�R���W�����I�u�W�F�N�g</param>
	static void ToSprite(Sprite* sprite, const AABB2DObj* obj);

	/// <summary>
	/// ���g���I�u�W�F�N�g���X�g�ɓo�^���܂�.
	/// </summary>
	void Register();
public:
	// �������̐F.
	struct Color {
		float r, g, b, a;
	} color;

	AABB2DObj(const GameObj* pParent, const AABB2D& collision);
	AABB2DObj(const GameObj* pParent, const Point2D& p = { 0, 0 }, const Float2& hl = { 0, 0 });
	AABB2DObj(const AABB2DObj& obj);

	~AABB2DObj();

	virtual AABB2DObj& operator=(const AABB2DObj& obj);
	virtual AABB2DObj& operator=(const AABB2D& rect);

	void SetOnActive(bool onActive) { _onActive = onActive; }
	void SetParent(const GameObj* pParent) { _pParent = pParent; }
	Float2 GetWorldPosition() const;
	bool GetOnActive() const;

	/// <summary>
	/// �X�v���C�g���X�g�̃|�C���^��Ԃ��܂�.
	/// �������Ȃ���Ԃɐݒ肳��Ă���ꍇ�A�_�~�[�̃X�v���C�g���X�g�̃|�C���^��Ԃ��܂�.
	/// </summary>
	/// <returns>�X�v���C�g���X�g�̃|�C���^</returns>
	static const std::vector<Sprite>* pGetSprite() { return _isDraw ? &_sprList : &_dummySprList; }

	/// <summary>
	/// ��`�̓����蔻�蓯�m���q�b�g���Ă��邩���`�F�b�N���܂�.
	/// </summary>
	/// <param name="sa">��`1</param>
	/// <param name="sb">��`2</param>
	/// <returns>true:�q�b�g���Ă��� false:�q�b�g���Ă��Ȃ�</returns>
	static bool RectOnRect(const AABB2DObj& sa, const AABB2DObj& sb);

	/// <summary>
	/// �`��̑O�������s���܂�.
	/// �����蔻�����������ꍇ�A�`��̑O�Ɏ��s���Ă�������.
	/// </summary>
	/// <returns></returns>
	static bool DrawBegin();

	/// <summary>
	/// �����蔻����������邩�ۂ���ݒ肵�܂�.
	/// </summary>
	/// <param name="isDraw">true:�������� false:�������Ȃ�</param>
	static void SetIsDraw(bool isDraw) { _isDraw = isDraw; }

	/// <summary>
	/// ���ݓ����蔻����������Ă��邩�ۂ���Ԃ��܂�.
	/// </summary>
	/// <returns>true:�������� false:�������Ȃ�</returns>
	static bool GetIsDraw() { return _isDraw; }

};
