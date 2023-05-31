#pragma once

// サンプルのsscanfをひとまず動かすため.
#define _CRT_SECURE_NO_WARNINGS
#include <DirectXMath.h>
#include <d3d12.h>
#include <memory>
#include <wrl/client.h>
#include <vector>
#include <string>
#include "Animation.h"
#include "Texture.h"
//#include "../UniquePtrTemplate.h"
#include "../GameObj.h"
#include "../PtrVectorMgr.h"

class Camera;
class SpriteResource;
class SpriteHandle;
struct PSO;
class SpriteObj;
class SpriteResource2;

/*
* スプライトとして表示する画像の範囲.
*/
struct Cell {
	DirectX::XMFLOAT2 uv; ///< テクスチャ上の左上座標.
	DirectX::XMFLOAT2 tsize; ///< テクスチャ上の縦横サイズ.
	DirectX::XMFLOAT2 ssize; ///< スクリーン座標上の縦横サイズ
	DirectX::XMFLOAT2 offset; ///< スクリーン座標上の表示位置補正.
	float xadvance; ///< 次のセルのX座標.
};

struct rect{
	float top = 0, bottom = 0, left = 0, right = 0;
};

struct Sprite
{
	//Sprite() = default;
	Sprite(const AnimationList* al = nullptr, DirectX::XMFLOAT3 p = {0, 0, 0 }, float r = 0,
		DirectX::XMFLOAT2 s = { 1, 1 }, DirectX::XMFLOAT4 c = { 1, 1, 1, 1 },
		DirectX::XMFLOAT4 ac = { 0, 0, 0, 0 },
		bool fh = false, bool fv = false, bool cr = true);
	Sprite(DirectX::XMFLOAT3 p, const AnimationList* al = nullptr, float r = 0,
		DirectX::XMFLOAT2 s = { 1, 1 }, DirectX::XMFLOAT4 c = { 1, 1, 1, 1 },
		DirectX::XMFLOAT4 ac = { 0, 0, 0, 0 },
		bool fh = false, bool fv = false, bool cr = true);

	Sprite(const SpriteObj& spriteObj);
	Sprite& operator=(const SpriteObj& obj);

	void SetCellIndex(uint32_t index) { animeController.SetCellIndex(index); }
	uint32_t GetCellIndex() const { return animeController.GetAnimationData().cellIndex; }
	  AnimationController animeController;
	  DirectX::XMFLOAT3 pos;
	  float rotation;
	  DirectX::XMFLOAT2 scale = { 1.0f, 1.0f };
	  DirectX::XMFLOAT4 color[2]; /// 画像の色（乗算）.
	  DirectX::XMFLOAT4 addColor; /// 画像の色（加算）.

	  bool onActive = true;
	  bool isUsed = true;
	  bool flipHorizontal = false;	// 左右反転.
	  bool flipVertical = false;	// 上下反転.
	  bool cameraRelative = true;	// カメラの影響有無.
	  rect rectOffset;

	  // 以下の二つはサンプルのシーンで使用されている　削除予定.
	  DirectX::XMFLOAT2 velocity; /// 移動速度.
	  int param[4];
};

/*
* スプライト描画情報.
*/
struct RenderingInfo
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle; ///< スプライト描画先レンダーターゲット.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle; ///< スプライト描画先深度バッファ.
	D3D12_VIEWPORT viewport; ///< 描画用ビューポート.
	D3D12_RECT scissorRect; ///< 描画用シザリング矩形.
	ID3D12DescriptorHeap* texDescHeap; ///< テクスチャ用のデスクリプタヒープ.
	DirectX::XMFLOAT4X4 matViewProjection; ///< 描画に使用する座標変換行列.
};

/**
* スプライト描画クラス.
*/
class SpriteRenderer
{
public:
	SpriteRenderer();
	~SpriteRenderer() = default;

	bool Init(
		Microsoft::WRL::ComPtr<ID3D12Device> device, int numFrameBuffer, unsigned int maxSprite);
	bool Begin(int frameIndex);
	bool Draw(const std::vector<Sprite>& spriteList, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info, const Camera* camera = nullptr);
	bool Draw(const Sprite* sprite, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info, const Camera* camera = nullptr);
	bool Draw(const Sprite* first, const Sprite* last, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info, const Camera* camera = nullptr);
	bool Draw(const SpriteResource* sprRes, const PSO& pso, RenderingInfo& info, const Camera* camera = nullptr);
	bool Draw(const SpriteResource2* sprRes, const PSO& pso, RenderingInfo& info, const Camera* camera = nullptr);

	// 無理矢理だけど、Spriteを継承したクラス用.
	bool Draw(const SpriteObj* const* first, const SpriteObj* const* last, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info,	const Camera* camera);
	bool End();

	ID3D12GraphicsCommandList* GetCommandList();
private:
	int spriteCount;
	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		void* vertexBufferGPUAddress;
	};
	std::vector<FrameResource> frameResourceList;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
	size_t maxSpriteCount;
	int currentFrameIndex;
};

/**
* セルデータの配列.
*/
struct CellList
{
	std::string name; ///< リスト名.
	std::vector<Cell> list; ///< セルデータの配列.
};

CellList LoadFontFromFile(const wchar_t* filename);
float GetTextWidth(const CellList& cellList, const char* text);

// 以下は、1から自作したもの.

// 旧CreateCellList　基本的にSpriteResource版を使用してください.
int CreateCellList(int AllNum, int XNum, int YNum, int XSize, int YSize, CellList& CellListBuf);


// TODO 現状の仕様だとコピー禁止だからなんらかの動的配列の内部で直接SpriteHandleが扱えない事に気付いた…全然ダメじゃん！
// とりあえずはstd::unique_ptrの配列にして直接扱わなければ使える.
// 描画関数とSpriteの方を調整した上で、AABB2DObjと同様の管理にすれば行けるかも.

// TODO SpriteResourceとSpriteHandleで同リソース内の描画順の管理をできるようにする.
// TODO SpriteResourceに新たなスプライトを取得した時の初期値としてのスプライト情報を持ち、設定できるようにする.
// TODO 現状はSpriteHandleの登録を解除しないままSpriteResourceが破棄されるとエラーを起こすので、できればどちらが先に破棄されても問題無いようにする.

/// <summary>
/// SpriteHandleとセットで使用する、描画情報を一纏めにし仲介するクラス.
/// </summary>
class SpriteResource {
private:
	std::vector<Sprite> _spriteList;		// スプライトリスト.
	Texture _texture;						// テクスチャ情報.
	std::vector<Cell> _cellList;			// セルリスト.
	AnimationList _animationList;			// アニメーションリスト.
	std::vector<size_t> _freeSpriteIDList;	// スプライトリストの未使用インデックスリスト.


public:
	SpriteResource() = default;
	SpriteResource(const AnimationList& animationList)
		: _animationList(animationList)
	{}
	// 複製を禁止.
	SpriteResource(const SpriteResource&) = delete;
	const SpriteResource& operator=(const SpriteResource&) = delete;

	~SpriteResource() = default;

	// 各種ゲッター.
	std::vector<Sprite>* pGetSpriteList() { return &_spriteList; }
	Texture& GetTexture() { return _texture; }
	const std::vector<Cell>* pGetCellList() const { return &_cellList; }

	// 描画時用のconst装飾付きゲッター.
	const std::vector<Sprite>* pGetSpriteList() const { return &_spriteList; }
	const Texture& GetTexture() const { return _texture; }

	// セッター.
	void SetAnimationList(const AnimationList& animationList) { _animationList = animationList; }
	void SetCellList(const std::vector<Cell>& animationList) { _cellList = animationList; }

	/// <summary>
	/// セルリストの作成.
	/// </summary>
	/// <param name="xNum">X分割数</param>
	/// <param name="yNum">Y分割数</param>
	/// <param name="xSize">セル一つあたりの幅</param>
	/// <param name="ySize">セル一つあたりの高さ</param>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize);

	// SpriteHandle関連.

	/// <summary>
	/// SpriteHandleとの関数付けを行います.
	/// 既に登録されているオブジェクトの場合、登録は解除され、新たな登録に上書きされます.
	/// </summary>
	/// <param name="spriteHandrer">登録するSpriteHandrer</param>
	/// <returns>true:成功 false:失敗</returns>
	bool Register(SpriteHandle& spriteHandrer);

	/// <summary>
	/// 引数のSpriteHandleが保有しているスプライトを解放します.
	/// スプライトを保有していない、またはこのオブジェクトが登録されていない場合なにもしません.
	/// </summary>
	/// <param name="spriteHandrer">解放を行うSpriteHandrer</param>
	/// <returns>true:成功 false:失敗</returns>
	bool Release(SpriteHandle& spriteHandrer);
};

/// <summary>
/// SpriteResourceとセットで使用する、スプライト利用補助オブジェクトです.
/// 
/// このオブジェクトを生成したら、登録したいSpriteResourceのRegister関数を使用して、このオブジェクトを登録してください.
/// 登録後は、pGetSprite関数でスプライトのポインタを取得できます.
/// 保有しているスプライトを解放したい場合は、Release関数、またはUnregister関数を使用するか、このオブジェクト自体を破棄してください。
/// 既に登録しているSpriteResourceをこのオブジェクトより先に破棄する場合、破棄する前に必ずUnregister関数を使用して登録を解除してください.
/// </summary>
class SpriteHandle {
	friend class SpriteResource;

private:

	bool _hasIndex;							// スプライトのインデックスを保有しているか否か.
	size_t _index;							// 保有しているスプライトのインデックス.
	SpriteResource* _pRegisteredResource;	// 登録しているSpriteResourceのポインタ.

public:
	SpriteHandle()
		: _hasIndex(false)
		, _index(0)
		, _pRegisteredResource(nullptr)
	{}
	~SpriteHandle() {
		Release();	// 保有しているスプライトを解放.
	};

	// 複製を禁止.
	// TODO 作り直しが終わるまで、一旦今はコピーコンストラクタは許可 ただし、コピーした上でオブジェクトが破棄された時に整合性が崩れるので早急に修正を.
	SpriteHandle(const SpriteHandle&) = delete;
	const SpriteHandle& operator=(const SpriteHandle&) = delete;

	/// <summary>
	/// スプライトのポインタを取得します.
	/// 必ず先にSpriteResourceのRegister関数を使用して、このオブジェクトを登録してから使用してください.
	/// 取得したポインタは、SpriteResource内で配列の領域確保が行われると無効になるため、.
	/// 保持はせず、必要な時にこの関数を介して使用して下さい.
	/// </summary>
	/// <returns>スプライトのポインタ</returns>
	Sprite* pGetSprite();

	/// <summary>
	/// 保有しているスプライトを解放します.
	/// スプライトを保有していない、またはSpriteResourceと関連付けされていない場合なにもしません.
	/// </summary>
	void Release();

	/// <summary>
	/// 保有しているスプライトを解放し、SpriteResourceとの関連付けを解除します.
	/// スプライトを保有していない場合、関連付けの解除のみ行います.
	/// 関連付けているSpriteResourceをこのオブジェクトより先に破棄する場合、必ず破棄する前に使用して関連付けを解除してください.
	/// </summary>
	void Unregister();
};

class SpriteObj : public GameObj, public PtrVector::IManaged{
public:
	SpriteObj(const AnimationList* pAnimationList = nullptr, Point2D position = { 0, 0 })
		: GameObj(position)
		, animeController(pAnimationList)
		, rotation(0)
		, scale({ 1.0f, 1.0f })
		, color({ 1.0f, 1.0f, 1.0f, 1.0f })
		, addColor({ 0, 0, 0, 0 })
		, rectOffset({ 0, 0, 0, 0 })
		, flipHorizontal(false)
		, flipVertical(false)
		, cameraRelative(true)
	{}

	void SetCellIndex(uint32_t index) { animeController.SetCellIndex(index); }
	uint32_t GetCellIndex() const { return animeController.GetAnimationData().cellIndex; }
	AnimationController animeController;	// アニメーションコントローラー.
	float rotation;							// 回転.
	DirectX::XMFLOAT2 scale;				// スケール.
	DirectX::XMFLOAT4 color;				// 画像の色（乗算）.
	DirectX::XMFLOAT4 addColor;				// 画像の色（加算）.
	rect rectOffset;						// 上下左右への引き伸ばし.
	bool flipHorizontal;					// 左右反転.
	bool flipVertical;						// 上下反転.
	bool cameraRelative;					// カメラの影響有無.
};

class SpriteResource2 : public PtrVector::PtrVectorMgr<SpriteObj>{
private:
	Texture _texture;						// テクスチャ情報.
	std::vector<Cell> _cellList;			// セルリスト.
	AnimationList _animationList;			// アニメーションリスト.
public:
	SpriteResource2() = default;
	SpriteResource2(const AnimationList& animationList)
		: _animationList(animationList)
	{}

	// 複製を禁止.
	SpriteResource2(const SpriteResource2&) = delete;
	const SpriteResource2& operator=(const SpriteResource2&) = delete;

	// 各種ゲッター.
	//std::vector<Sprite>* pGetSpriteList() { return &_spriteList; }
	Texture& GetTexture() { return _texture; }

	// 描画時用のconst装飾付きゲッター.
	//const std::vector<Sprite>* pGetSpriteList() const { return &_spriteList; }
	const Texture& GetTexture() const { return _texture; }
	const std::vector<Cell>* pGetCellList() const { return &_cellList; }

	// セッター.
	void SetAnimationList(const AnimationList& animationList) { _animationList = animationList; }
	void SetCellList(const std::vector<Cell>& animationList) { _cellList = animationList; }

	/// <summary>
	/// セルリストを作成します.
	/// </summary>
	/// <param name="xNum">X分割数</param>
	/// <param name="yNum">Y分割数</param>
	/// <param name="xSize">セル一つあたりの幅</param>
	/// <param name="ySize">セル一つあたりの高さ</param>
	/// <returns>true:成功 false:失敗</returns>
	bool CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize);
};