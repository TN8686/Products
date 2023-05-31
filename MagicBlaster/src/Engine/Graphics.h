#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include "Texture.h"
#include "Sprite.h"

/**
* 描画処理クラス.
*/
class Graphics
{
public:
	
	static Graphics& Get();
	bool Initialize(HWND hwnd, int clientWidth, int clientHeight);
	void Finalize();
	bool BeginRendering();
	bool EndRendering();
	bool WaitForPreviousFrame();
	bool WaitForGpu();
	/*
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
	bool LoadTexture();
	bool CreateCircleTexture(TextureLoader&);
	void DrawTriangle();
	void DrawRectangles();
	void DrawRectangles2();
	void CreateAnimationList();
	*/

	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVHandle() const;

public:
	static const int frameBufferCount = 2;
	static const int RenderingTextureCount = 2;	// メイン画面、UI画面.
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetList[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> csuDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetTextureList[RenderingTextureCount];
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> prologueCommandList; // 最初に実行されるコマンドリスト.
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> epilogueCommandList; // 最後に実行されるコマンドリスト.
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 fenceValue[frameBufferCount];
	UINT64 masterFenceValue;
	int currentFrameIndex;
	int rtvDescriptorSize;
	int csuDescriptorSize;
	bool warp;
	
	// 仮のテクスチャ群.
	Texture texCircle;
	Texture texImage;
	Texture texImage2;
	Texture texImage3;
	Texture texSprite;
	Texture texWitch;

	std::vector<Sprite> spriteList;
	std::vector<Sprite> witchSpriteList;
	SpriteRenderer spriteRenderer;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	DirectX::XMFLOAT4X4 matViewProjection;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	AnimationList tempAnimList;
	AnimationList witchAnimationList;

	TextureMap texMap;
	const unsigned int* frameCount;
private:
	Graphics() = default;
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
};
