#pragma once

#include "Engine/primitive2d.h"
#include "Engine/Sprite.h"
#include <vector>

class GameObj;

// TODO 現状は個別のチェックの中で総当たりしているが、将来的に判定したい領域をグリッド化してヒットチェックを行う.
class AABB2DObj : public AABB2D {
protected:
	// 静的メンバ.
	static std::vector<AABB2DObj*> _objList;			// 存在している全てのコリジョンオブジェクトのリスト.
	static std::vector<std::size_t> _freeIDList;		// _objList内の、使用後にオブジェクトが破棄されて空いたインデックスのリスト.
	static std::vector<Sprite> _sprList;				// スプライトリスト.
	static std::vector<Sprite> _dummySprList;			// ダミースプライトリスト 非可視化時にはこちらを渡す.
	static bool _isDraw;								// 可視化するか否かのフラグ.

	// メンバ.
	const GameObj* _pParent;							// 親オブジェクト.
	bool _onActive;										// アクティブ状態.
	std::size_t _index;									// _objList内で現在使用しているインデックス.

	/// <summary>
	/// 矩形コリジョン情報を元に、スプライトの値を設定します.
	/// </summary>
	/// <param name="sprite">o 値を設定するスプライト</param>
	/// <param name="collision">i 矩形コリジョンオブジェクト</param>
	static void ToSprite(Sprite* sprite, const AABB2DObj* obj);

	/// <summary>
	/// 自身をオブジェクトリストに登録します.
	/// </summary>
	void Register();
public:
	// 可視化時の色.
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
	/// スプライトリストのポインタを返します.
	/// 可視化しない状態に設定されている場合、ダミーのスプライトリストのポインタを返します.
	/// </summary>
	/// <returns>スプライトリストのポインタ</returns>
	static const std::vector<Sprite>* pGetSprite() { return _isDraw ? &_sprList : &_dummySprList; }

	/// <summary>
	/// 矩形の当たり判定同士がヒットしているかをチェックします.
	/// </summary>
	/// <param name="sa">矩形1</param>
	/// <param name="sb">矩形2</param>
	/// <returns>true:ヒットしている false:ヒットしていない</returns>
	static bool RectOnRect(const AABB2DObj& sa, const AABB2DObj& sb);

	/// <summary>
	/// 描画の前準備を行います.
	/// 当たり判定を可視化する場合、描画の前に実行してください.
	/// </summary>
	/// <returns></returns>
	static bool DrawBegin();

	/// <summary>
	/// 当たり判定を可視化するか否かを設定します.
	/// </summary>
	/// <param name="isDraw">true:可視化する false:可視化しない</param>
	static void SetIsDraw(bool isDraw) { _isDraw = isDraw; }

	/// <summary>
	/// 現在当たり判定を可視化しているか否かを返します.
	/// </summary>
	/// <returns>true:可視化する false:可視化しない</returns>
	static bool GetIsDraw() { return _isDraw; }

};
