#pragma once

// �T���v����sscanf���ЂƂ܂�����������.
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
* �X�v���C�g�Ƃ��ĕ\������摜�͈̔�.
*/
struct Cell {
	DirectX::XMFLOAT2 uv; ///< �e�N�X�`����̍�����W.
	DirectX::XMFLOAT2 tsize; ///< �e�N�X�`����̏c���T�C�Y.
	DirectX::XMFLOAT2 ssize; ///< �X�N���[�����W��̏c���T�C�Y
	DirectX::XMFLOAT2 offset; ///< �X�N���[�����W��̕\���ʒu�␳.
	float xadvance; ///< ���̃Z����X���W.
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
	  DirectX::XMFLOAT4 color[2]; /// �摜�̐F�i��Z�j.
	  DirectX::XMFLOAT4 addColor; /// �摜�̐F�i���Z�j.

	  bool onActive = true;
	  bool isUsed = true;
	  bool flipHorizontal = false;	// ���E���].
	  bool flipVertical = false;	// �㉺���].
	  bool cameraRelative = true;	// �J�����̉e���L��.
	  rect rectOffset;

	  // �ȉ��̓�̓T���v���̃V�[���Ŏg�p����Ă���@�폜�\��.
	  DirectX::XMFLOAT2 velocity; /// �ړ����x.
	  int param[4];
};

/*
* �X�v���C�g�`����.
*/
struct RenderingInfo
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle; ///< �X�v���C�g�`��惌���_�[�^�[�Q�b�g.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle; ///< �X�v���C�g�`���[�x�o�b�t�@.
	D3D12_VIEWPORT viewport; ///< �`��p�r���[�|�[�g.
	D3D12_RECT scissorRect; ///< �`��p�V�U�����O��`.
	ID3D12DescriptorHeap* texDescHeap; ///< �e�N�X�`���p�̃f�X�N���v�^�q�[�v.
	DirectX::XMFLOAT4X4 matViewProjection; ///< �`��Ɏg�p������W�ϊ��s��.
};

/**
* �X�v���C�g�`��N���X.
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

	// ����������ǁASprite���p�������N���X�p.
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
* �Z���f�[�^�̔z��.
*/
struct CellList
{
	std::string name; ///< ���X�g��.
	std::vector<Cell> list; ///< �Z���f�[�^�̔z��.
};

CellList LoadFontFromFile(const wchar_t* filename);
float GetTextWidth(const CellList& cellList, const char* text);

// �ȉ��́A1���玩�삵������.

// ��CreateCellList�@��{�I��SpriteResource�ł��g�p���Ă�������.
int CreateCellList(int AllNum, int XNum, int YNum, int XSize, int YSize, CellList& CellListBuf);


// TODO ����̎d�l���ƃR�s�[�֎~������Ȃ�炩�̓��I�z��̓����Œ���SpriteHandle�������Ȃ����ɋC�t�����c�S�R�_�������I
// �Ƃ肠������std::unique_ptr�̔z��ɂ��Ē��ڈ���Ȃ���Ύg����.
// �`��֐���Sprite�̕��𒲐�������ŁAAABB2DObj�Ɠ��l�̊Ǘ��ɂ���΍s���邩��.

// TODO SpriteResource��SpriteHandle�œ����\�[�X���̕`�揇�̊Ǘ����ł���悤�ɂ���.
// TODO SpriteResource�ɐV���ȃX�v���C�g���擾�������̏����l�Ƃ��ẴX�v���C�g���������A�ݒ�ł���悤�ɂ���.
// TODO �����SpriteHandle�̓o�^���������Ȃ��܂�SpriteResource���j�������ƃG���[���N�����̂ŁA�ł���΂ǂ��炪��ɔj������Ă���薳���悤�ɂ���.

/// <summary>
/// SpriteHandle�ƃZ�b�g�Ŏg�p����A�`�������Z�߂ɂ������N���X.
/// </summary>
class SpriteResource {
private:
	std::vector<Sprite> _spriteList;		// �X�v���C�g���X�g.
	Texture _texture;						// �e�N�X�`�����.
	std::vector<Cell> _cellList;			// �Z�����X�g.
	AnimationList _animationList;			// �A�j���[�V�������X�g.
	std::vector<size_t> _freeSpriteIDList;	// �X�v���C�g���X�g�̖��g�p�C���f�b�N�X���X�g.


public:
	SpriteResource() = default;
	SpriteResource(const AnimationList& animationList)
		: _animationList(animationList)
	{}
	// �������֎~.
	SpriteResource(const SpriteResource&) = delete;
	const SpriteResource& operator=(const SpriteResource&) = delete;

	~SpriteResource() = default;

	// �e��Q�b�^�[.
	std::vector<Sprite>* pGetSpriteList() { return &_spriteList; }
	Texture& GetTexture() { return _texture; }
	const std::vector<Cell>* pGetCellList() const { return &_cellList; }

	// �`�掞�p��const�����t���Q�b�^�[.
	const std::vector<Sprite>* pGetSpriteList() const { return &_spriteList; }
	const Texture& GetTexture() const { return _texture; }

	// �Z�b�^�[.
	void SetAnimationList(const AnimationList& animationList) { _animationList = animationList; }
	void SetCellList(const std::vector<Cell>& animationList) { _cellList = animationList; }

	/// <summary>
	/// �Z�����X�g�̍쐬.
	/// </summary>
	/// <param name="xNum">X������</param>
	/// <param name="yNum">Y������</param>
	/// <param name="xSize">�Z���������̕�</param>
	/// <param name="ySize">�Z���������̍���</param>
	/// <returns>true:���� false:���s</returns>
	bool CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize);

	// SpriteHandle�֘A.

	/// <summary>
	/// SpriteHandle�Ƃ̊֐��t�����s���܂�.
	/// ���ɓo�^����Ă���I�u�W�F�N�g�̏ꍇ�A�o�^�͉�������A�V���ȓo�^�ɏ㏑������܂�.
	/// </summary>
	/// <param name="spriteHandrer">�o�^����SpriteHandrer</param>
	/// <returns>true:���� false:���s</returns>
	bool Register(SpriteHandle& spriteHandrer);

	/// <summary>
	/// ������SpriteHandle���ۗL���Ă���X�v���C�g��������܂�.
	/// �X�v���C�g��ۗL���Ă��Ȃ��A�܂��͂��̃I�u�W�F�N�g���o�^����Ă��Ȃ��ꍇ�Ȃɂ����܂���.
	/// </summary>
	/// <param name="spriteHandrer">������s��SpriteHandrer</param>
	/// <returns>true:���� false:���s</returns>
	bool Release(SpriteHandle& spriteHandrer);
};

/// <summary>
/// SpriteResource�ƃZ�b�g�Ŏg�p����A�X�v���C�g���p�⏕�I�u�W�F�N�g�ł�.
/// 
/// ���̃I�u�W�F�N�g�𐶐�������A�o�^������SpriteResource��Register�֐����g�p���āA���̃I�u�W�F�N�g��o�^���Ă�������.
/// �o�^��́ApGetSprite�֐��ŃX�v���C�g�̃|�C���^���擾�ł��܂�.
/// �ۗL���Ă���X�v���C�g������������ꍇ�́ARelease�֐��A�܂���Unregister�֐����g�p���邩�A���̃I�u�W�F�N�g���̂�j�����Ă��������B
/// ���ɓo�^���Ă���SpriteResource�����̃I�u�W�F�N�g����ɔj������ꍇ�A�j������O�ɕK��Unregister�֐����g�p���ēo�^���������Ă�������.
/// </summary>
class SpriteHandle {
	friend class SpriteResource;

private:

	bool _hasIndex;							// �X�v���C�g�̃C���f�b�N�X��ۗL���Ă��邩�ۂ�.
	size_t _index;							// �ۗL���Ă���X�v���C�g�̃C���f�b�N�X.
	SpriteResource* _pRegisteredResource;	// �o�^���Ă���SpriteResource�̃|�C���^.

public:
	SpriteHandle()
		: _hasIndex(false)
		, _index(0)
		, _pRegisteredResource(nullptr)
	{}
	~SpriteHandle() {
		Release();	// �ۗL���Ă���X�v���C�g�����.
	};

	// �������֎~.
	// TODO ��蒼�����I���܂ŁA��U���̓R�s�[�R���X�g���N�^�͋��� �������A�R�s�[������ŃI�u�W�F�N�g���j�����ꂽ���ɐ������������̂ő��}�ɏC����.
	SpriteHandle(const SpriteHandle&) = delete;
	const SpriteHandle& operator=(const SpriteHandle&) = delete;

	/// <summary>
	/// �X�v���C�g�̃|�C���^���擾���܂�.
	/// �K�����SpriteResource��Register�֐����g�p���āA���̃I�u�W�F�N�g��o�^���Ă���g�p���Ă�������.
	/// �擾�����|�C���^�́ASpriteResource���Ŕz��̗̈�m�ۂ��s����Ɩ����ɂȂ邽�߁A.
	/// �ێ��͂����A�K�v�Ȏ��ɂ��̊֐�����Ďg�p���ĉ�����.
	/// </summary>
	/// <returns>�X�v���C�g�̃|�C���^</returns>
	Sprite* pGetSprite();

	/// <summary>
	/// �ۗL���Ă���X�v���C�g��������܂�.
	/// �X�v���C�g��ۗL���Ă��Ȃ��A�܂���SpriteResource�Ɗ֘A�t������Ă��Ȃ��ꍇ�Ȃɂ����܂���.
	/// </summary>
	void Release();

	/// <summary>
	/// �ۗL���Ă���X�v���C�g��������ASpriteResource�Ƃ̊֘A�t�����������܂�.
	/// �X�v���C�g��ۗL���Ă��Ȃ��ꍇ�A�֘A�t���̉����̂ݍs���܂�.
	/// �֘A�t���Ă���SpriteResource�����̃I�u�W�F�N�g����ɔj������ꍇ�A�K���j������O�Ɏg�p���Ċ֘A�t�����������Ă�������.
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
	AnimationController animeController;	// �A�j���[�V�����R���g���[���[.
	float rotation;							// ��].
	DirectX::XMFLOAT2 scale;				// �X�P�[��.
	DirectX::XMFLOAT4 color;				// �摜�̐F�i��Z�j.
	DirectX::XMFLOAT4 addColor;				// �摜�̐F�i���Z�j.
	rect rectOffset;						// �㉺���E�ւ̈����L�΂�.
	bool flipHorizontal;					// ���E���].
	bool flipVertical;						// �㉺���].
	bool cameraRelative;					// �J�����̉e���L��.
};

class SpriteResource2 : public PtrVector::PtrVectorMgr<SpriteObj>{
private:
	Texture _texture;						// �e�N�X�`�����.
	std::vector<Cell> _cellList;			// �Z�����X�g.
	AnimationList _animationList;			// �A�j���[�V�������X�g.
public:
	SpriteResource2() = default;
	SpriteResource2(const AnimationList& animationList)
		: _animationList(animationList)
	{}

	// �������֎~.
	SpriteResource2(const SpriteResource2&) = delete;
	const SpriteResource2& operator=(const SpriteResource2&) = delete;

	// �e��Q�b�^�[.
	//std::vector<Sprite>* pGetSpriteList() { return &_spriteList; }
	Texture& GetTexture() { return _texture; }

	// �`�掞�p��const�����t���Q�b�^�[.
	//const std::vector<Sprite>* pGetSpriteList() const { return &_spriteList; }
	const Texture& GetTexture() const { return _texture; }
	const std::vector<Cell>* pGetCellList() const { return &_cellList; }

	// �Z�b�^�[.
	void SetAnimationList(const AnimationList& animationList) { _animationList = animationList; }
	void SetCellList(const std::vector<Cell>& animationList) { _cellList = animationList; }

	/// <summary>
	/// �Z�����X�g���쐬���܂�.
	/// </summary>
	/// <param name="xNum">X������</param>
	/// <param name="yNum">Y������</param>
	/// <param name="xSize">�Z���������̕�</param>
	/// <param name="ySize">�Z���������̍���</param>
	/// <returns>true:���� false:���s</returns>
	bool CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize);
};