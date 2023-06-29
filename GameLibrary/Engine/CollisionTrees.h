#pragma once
#include <windows.h>

#include "Primitive.h"
#include "../Utility/MyArray.h"

// モートン順序を利用した空間分割による衝突判定

namespace Engine {

	// レイヤー.
	enum OFTLayer {
		OFTLAYER_DEFAULT,
		OFTLAYER_PLAYER,
		OFTLAYER_ITEM,
		OFTLAYER_ENEMY_ATTACK,
		OFTLAYER_WALL,
		OFTLAYER_NUM
	};

	// 前方宣言.
	template <class T>
	class Cell;

	//////////////////////////////
	// 分木登録オブジェクト(OFT).
	//////////////////////////////
	template< class T>
	class ObjectForTree
	{
		// メンバ変数.
	public:
		int id_;					// ID.
		Cell<T>* pCell_;			// 登録空間.
		T* pObject_;				// 判定対象オブジェクト.
		ObjectForTree<T>* pPre_;	// 前のObjectForTree構造体.
		ObjectForTree<T>* pNext_;	// 次のObjectForTree構造体.
		OFTLayer layer_;

		// コンストラクタ.
	public:
		ObjectForTree(int id = 0)
			: id_(id)
			, pCell_(nullptr)
			, pObject_(nullptr)
			, pPre_(nullptr)
			, pNext_(nullptr)
			, layer_(OFTLAYER_DEFAULT)
		{}

		// デストラクタ.
		virtual ~ObjectForTree() {
			remove();
		}

		// 操作.
	public:
		// 自らリストから外れる.
		bool remove() {
			// すでに逸脱している時は処理終了.
			if (pCell_ == nullptr) {
				return false;
			}

			// 自分を登録している空間に自身を通知.
			if (!pCell_->onRemove(this)) {
				return false;
			}

			// 逸脱処理.
			// 前後のオブジェクトを結びつける.
			if (pPre_ != nullptr) {
				pPre_->pNext_ = pNext_;
			}
			if (pNext_ != nullptr) {
				pNext_->pPre_ = pPre_;
			}
			pPre_ = nullptr;
			pNext_ = nullptr;
			pCell_ = nullptr;

			return true;
		}

	};

	//////////////////////////////////
	// 衝突リスト ポインタを保持するシンプルなコンテナクラス
	//////////////////////////////////
	template < class T >
	class CollisionList {
		static constexpr int COLLISIONLIST_REALLOCSIZE = 100;
	private:
		T** root_;		// リストルートポインタ.
		size_t	pos_;
		size_t	allocSize_;

	public:
		CollisionList() 
			: root_(nullptr)
			, pos_(0)
			, allocSize_(0) 
		{
			root_ = (T**)malloc(0);
		}
		~CollisionList() {
			reflesh();
		}

		size_t getSize() {
			return pos_;
		}

		T** getRootPtr() {
			return root_;
		}

		void resetPos() {
			pos_ = 0;
		}

		void wright(T* obj1, T* obj2) {
			if (pos_ >= allocSize_) {
				root_ = (T**)realloc(root_, sizeof(T*) * (allocSize_ + COLLISIONLIST_REALLOCSIZE));
				allocSize_ += COLLISIONLIST_REALLOCSIZE;
			}
			root_[pos_++] = obj1;
			root_[pos_++] = obj2;
		}

		void reflesh() {
			if (root_) {
				free(root_);
			}
		}

	};

	////////////////////////////
	// 線形4分木空間管理クラス.
	////////////////////////////

	// 定数.
	namespace {
		constexpr unsigned int LINER4TREEMANAGER_MAXLEVEL = 9;	// 空間分割レベルの上限.
	}

	template <class T>
	class Liner4TreeManager {

		// ヒットするレイヤーの組合せのリスト TODO 直接ここで定義すべきではないが、今だけここで.
		bool combinationList_[15] = {

			//		de		pl		it		at		wl
			/*de*/	false,	false,	false,	false,	false,
			/*pl*/			false,	true,	true,	true,
			/*it*/					false,	false ,	false,
			/*at*/							false,	true,
			/*wl*/									false,
		};

		// メンバ変数.
	protected:
		Cell<T>** ppCellAry_;								// 線形空間ポインタ配列.
		unsigned int pow_[LINER4TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列.
		Vector2 widthHight_;								// 領域の幅と高さ.
		Vector2 topLeft_;									// 領域の基準点 左上.
		Vector2 unitWidthHight_;							// 最小レベル空間の幅と高さ.
		DWORD cellNum_;										// 空間の数.
		unsigned int level_;								// 最下位レベル.
		CollisionList<T> colList_;							// 衝突リスト
		Utility::MyArray<ObjectForTree<T>*> colStac_;		// 衝突オブジェクトスタック.

		// コンストラクタ.
	public:
		Liner4TreeManager()
			: ppCellAry_(nullptr)
			, widthHight_{0.0f, 0.0f}
			, topLeft_{0.0f, 0.0f}
			, unitWidthHight_{0.0f, 0.0f}
			, cellNum_(0)
			, level_(0)
			, colStac_(128)
		{
			// 各レベルでの空間数を算出
			int i;
			pow_[0] = 1;	// ルート空間の数なので1
			for (i = 1; i < LINER4TREEMANAGER_MAXLEVEL + 1; i++) {
				pow_[i] = pow_[i - 1] * 4;		// 1つ前の4倍を入れていく　1, 4, 16, 64...
			}
		}

		// デストラクタ.
		~Liner4TreeManager() {
			// 空間を破棄.
			for (unsigned int i = 0; i < cellNum_; ++i) {
				if (ppCellAry_[i] != nullptr) {
					delete ppCellAry_[i];
				}
			}
			delete[] ppCellAry_;
		}


		// 操作.
	public:
		// 線形4分木配列を構築する.
		bool init(unsigned int level, float left, float top, float right, float bottom) {
			// 設定最高レベル以上の空間は作れない
			if (level >= LINER4TREEMANAGER_MAXLEVEL) {
				return false;
			}

			// 空間が存在している場合一度破棄.
			if (ppCellAry_ != nullptr) {
				for (unsigned int i = 0; i < cellNum_; ++i) {
					if (ppCellAry_[i] != nullptr) {
						delete ppCellAry_[i];
					}
				}
				delete[] ppCellAry_;
			}

			// 0基点でlevel分の配列作成.
			cellNum_ = (pow_[level + 1] - 1) / 3;	// ((4 ^ n) - 1) / (4 - 1)
			ppCellAry_ = new Cell<T>*[cellNum_] { nullptr };

			// 領域を登録.
			topLeft_.x = left;
			topLeft_.y = top;
			widthHight_.x = right - left;
			widthHight_.y = bottom - top;
			unitWidthHight_.x = widthHight_.x / (1 << level);	// レベル数に応じた値で割る.
			unitWidthHight_.y = widthHight_.y / (1 << level);
			level_ = level;

			return true;
		}


		bool regist(Rect rect, ObjectForTree<T>* pOFT) {
			return regist(rect.topLeft.x, rect.topLeft.y, rect.bottomRight.x, rect.bottomRight.y, pOFT);
		}

		// オブジェクトを登録する.
		bool regist(float left, float top, float right, float bottom, ObjectForTree<T>* pOFT) {
			
			// 判定領域と比較.
			// 左上は、より右下に近い値を選択.
			left = max(left, topLeft_.x);
			top = max(top, topLeft_.y);

			// 右下は、より左上に近い値を選択.
			right = min(right, topLeft_.x + widthHight_.x - 1);
			bottom = min(bottom, topLeft_.y + widthHight_.y - 1);

			// 領域内に存在する部分が無ければ登録しない.
			if (left > right || top > bottom) {
				return false;
			}

			DWORD elem = getMortonNumber(left, top, right, bottom);
			if (elem < cellNum_) {
				// 空間が無い場合は新規作成.
				if (ppCellAry_[elem] == nullptr) {
					createNewCell(elem);
				}
				return ppCellAry_[elem]->push(pOFT);
			}
			return false;	// 登録失敗.
		}

		// 衝突判定リストを作成する.
		DWORD getAllCollisionList(CollisionList<T>** colList) {

			// リスト（配列）は必ず初期化します.
			colList_.resetPos();

			// ルート空間の存在をチェック.
			if (ppCellAry_[0] == nullptr) {
				return 0;	// 空間が存在しない.
			}
			getCollisionList(0, colStac_);

			*colList = &colList_;

			return static_cast<DWORD>(colList_.getSize());
		}

	protected:
		// 空間内で衝突リストを作成する.
		DWORD getCollisionList(DWORD elem, Utility::MyArray<ObjectForTree<T>*>& colStac) {

			// 同じ空間内のオブジェクト同士の衝突リスト作成.
			ObjectForTree<T>* pOFT1 = ppCellAry_[elem]->getFirstObj();

			// 二重ループで全ての組み合わせをリストアップしていく
			while (pOFT1 != nullptr) {	// 一つ目のオブジェクトを辿るループ.
				ObjectForTree<T>* pOFT2 = pOFT1->pNext_;

				while (pOFT2 != nullptr) {	// 一つ目の後に続くオブジェクトを辿るループ.
					if (layerCheck(pOFT1->layer_, pOFT2->layer_)) {	// 衝突しうるレイヤーかどうかをチェック.
						colList_.wright(pOFT1->pObject_, pOFT2->pObject_);
					}
					pOFT2 = pOFT2->pNext_;
				}

				// 衝突スタックとの衝突リスト作成.
				auto p = colStac.begin();
				auto end = colStac.end();
				for (; p != end; ++p) {
					if (layerCheck(pOFT1->layer_, (*p)->layer_)) {	// 衝突しうるレイヤーかどうかをチェック.
						colList_.wright(pOFT1->pObject_, (*p)->pObject_);
					}
				}
				pOFT1 = pOFT1->pNext_;
			}

			bool childFlag = false;

			// 子空間に移動.
			DWORD objNum = 0;
			DWORD i, nextElem;

			for (i = 0; i < 4; ++i) {
				nextElem = elem * 4 + 1 + i;	// 現在の要素 * 4 + 1で子空間の最初の要素を指し、 + iで四つを順に巡る.
				if (nextElem < cellNum_ && ppCellAry_[nextElem] != nullptr) {
					if (!childFlag) {
						// 登録オブジェクトをスタックに追加.
						pOFT1 = ppCellAry_[elem]->getFirstObj();
						while (pOFT1 != nullptr) {					// 一つ目のオブジェクトを辿るループ.
							colStac.push(pOFT1);
							++objNum;
							pOFT1 = pOFT1->pNext_;
						}
					}
					childFlag = true;
					getCollisionList(nextElem, colStac);	// 再帰的に子空間へ.
				}
			}

			// スタックからオブジェクトを外す.
			if (childFlag) {
				for (i = 0; i < objNum; ++i) {
					colStac.pop();
				}
			}

			return true;
		}

		// 空間を生成.
		bool createNewCell(DWORD elem)
		{
			while (ppCellAry_[elem] == nullptr)
			{
				ppCellAry_[elem] = new Cell<T>;	// 空間を生成.

				// 親空間にジャンプ.
				elem = (elem - 1) >> 2;
				if (elem >= cellNum_) {	// 仮に0(ルート空間)が生成された場合、その後elemがオーバーフローしてこの条件を満たす.
					break;
				}
			}

			return true;
		}

		// 座標から空間番号を算出.
		DWORD getMortonNumber(float left, float top, float right, float bottom) {
			//最小レベルにおける各軸位置を算出.
			DWORD lt = getPointElem(left, top);
			DWORD rb = getPointElem(right, bottom);

			// 空間番号の排他的論理和から所属レベルを算出.
			// 境界図形の所属する分割レベルはこの排他的論理和の結果ビットが0じゃない最上位の区切りに表現される.
			DWORD def = rb ^ lt;
			unsigned int hiLevel = 0;
			for (unsigned int i = 0; i < level_; ++i) {
				DWORD check = (def >> (i * 2)) & 0b11;
				if (check != 0) {		// マスクの結果0じゃなかった時に.
					hiLevel = i + 1;	// 所属最高レベルを更新.
				}
			}

			// 空間番号を算出.
			DWORD spaceNum = rb >> (hiLevel * 2);			// 右下の所属番号をレベル * 2の分シフト.
			spaceNum += (pow_[level_ - hiLevel] - 1) / 3;	// レベルに応じた空間番号のオフセット値を取り出し加算 ((4 ^ n) - 1) / (4 - 1).

			if (spaceNum > cellNum_) {	// 算出結果が用意しているセル数より多い.
				return 0xffffffff;		// エラーを返す.
			}
			return spaceNum;

		}

		// ビット分割関数.
		DWORD bitSeparate32(DWORD n) {
			// ズラして元とorを取り、マスクする操作を繰り返す.
			n = (n | (n << 8)) & 0x00ff00ff;	// 0b00000000111111110000000011111111
			n = (n | (n << 4)) & 0x0f0f0f0f;	// 0b0000111100001111...
			n = (n | (n << 2)) & 0x33333333;	// 0b00110011...
			return (n | (n << 1)) & 0x55555555;	// 0b0101...
		}

		// 2Dモートン空間番号算出関数.
		WORD get2DMortonNumber(WORD x, WORD y) {
			return static_cast<WORD>(bitSeparate32(x) | (bitSeparate32(y) << 1));	// xとyから分割bitを作り、y側を1つ左シフトしてorで一つに.
		}

		// 座標から線形4分木要素番号に変換する関数.
		DWORD getPointElem(float posX, float posY) {
			// x,yそれぞれ座標の相対から領域基準の座標を出し、それを単位幅で割って点の所属領域を出す.
			return get2DMortonNumber(
				static_cast<WORD>((posX - topLeft_.x) / unitWidthHight_.x),
				static_cast<WORD>((posY - topLeft_.y) / unitWidthHight_.y));
		}

		// チェックを行うレイヤー同士かどうかを返します.
		bool layerCheck(OFTLayer a, OFTLayer b) {

			// インデックス参考
			//		de	pl	pat	bl	wl
			//de*/	0,	1,	2,	3,	4,
			//pl*/		5,	6,	7,	8,
			//pat*/			9,	10,	11,
			//bl*/				12,	13,
			//wl*/					14,

			// a側が小さな値になるようにする.
			if (a > b) {
				auto tmp = a;
				a = b;
				b = tmp;
			}

			if (a < 0 || b >= OFTLAYER_NUM) {	// 範囲外チェック.
				return false;
			}

			int num1 = OFTLAYER_NUM * (OFTLAYER_NUM + 1) / 2;			// OFTLAYER_NUMの組合せの数.
			int num2 = (OFTLAYER_NUM - a) * (OFTLAYER_NUM - a + 1) / 2;	// OFTLAYER_NUM - aの組合せの数.
			return combinationList_[(num1 - num2) + (b - a)];			// num1 - num2 で、組合せaaのIDを出し、そこにb - aを足す事で組合せabの位置を出している.

		}
	};


	/////////////////////////
	// 分割された空間クラス.
	/////////////////////////
	template <class T>
	class Cell
	{
	protected:
		ObjectForTree<T>* pLatest_;	// 最新OFTのポインタ.

	public:
		// コンストラクタ.
		Cell()
			: pLatest_(nullptr)
		{}

		// デストラクタ.
	public:
		virtual ~Cell()
		{
			if (pLatest_ != nullptr) {
				resetLink(pLatest_);
			}
		}

		// 操作.
	public:
		// リンクを全てリセットする.
		void resetLink(ObjectForTree<T>* pOFT)
		{
			if (pOFT->pNext_ != nullptr) {
				resetLink(pOFT->pNext_);
			}
			pOFT = nullptr;
		}

		// OFTをプッシュ.
		bool push(ObjectForTree<T>* pOFT)
		{
			if (pOFT == nullptr) {			// 無効オブジェクトは登録しない.
				return false;
			}
			if (pOFT->pCell_ == this) {		// 二重登録チェック.
				return false;
			}

			if (pLatest_ == nullptr) {		// 最新OFTが無い.
				pLatest_ = pOFT;			// 空間に新規登録.
			}
			else {							// 最新OFTがある.
				pOFT->pNext_ = pLatest_;	// 最新オブジェクトを更新 今までの最新を二個目に.
				pLatest_->pPre_ = pOFT;		
				pLatest_ = pOFT;			// 最新を新しい物に.
			}
			pOFT->pCell_ = this;			// 空間を登録.

			return true;
		}

		// 削除されるオブジェクトをチェック.
		bool onRemove(ObjectForTree<T>* pRemoveObj)
		{
			if (pLatest_ == pRemoveObj) {			// その空間の最新オブジェクトだったら.
				if (pLatest_ != nullptr) {
					pLatest_ = pLatest_->pNext_;	// 次のオブジェクトに挿げ替え.
				}
			}
			return true;
		}

		// 属性.
	public:
		ObjectForTree<T>* getFirstObj()
		{
			return pLatest_;
		}

	};
}