#include "Graphics.h"
#include "PSO.h"
#include "d3dx12.h"
#include <cmath>
#include <memory>


using Microsoft::WRL::ComPtr;
using namespace DirectX;

// 頂点データ型.
struct Vertex
{
	XMFLOAT3 position; // 座標.
	XMFLOAT4 color; // 色.
	XMFLOAT2 texcoord; // テクスチャ座標.
};

/**
* 描画環境の初期化.
*
* @param hwnd         ウィンドウハンドル.
* @param clientWidth  ウィンドウの幅.
* @param clientHeight ウィンドウの高さ.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool Graphics::Initialize(HWND hwnd, int clientWidth, int clientHeight)
{
#ifndef NDEBUG
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif

	ComPtr<IDXGIFactory4> dxgiFactory;
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)))) {
		return false;
	}

	// 機能レベル11を満たすハードウェアアダプタを検索し、そのデバイスインターフェイスを取得する.
	ComPtr<IDXGIAdapter1> dxgiAdapter; // デバイス情報を取得するインターフェイス.
	int adapterIndex = 0; // 列挙するデバイスのインデックス.
	bool adapterFound = false; // 目的のデバイスが見つかったかどうか.
	while (dxgiFactory->EnumAdapters1(
		adapterIndex, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter->GetDesc1(&desc);
		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
			if (SUCCEEDED(D3D12CreateDevice(
				dxgiAdapter.Get(),
				D3D_FEATURE_LEVEL_11_0,
				__uuidof(ID3D12Device),
				nullptr))) {
				adapterFound = true;
				break;
			}
		}
		++adapterIndex;
	}
	if (!adapterFound) {
		// 機能レベル11を満たすハードウェアが見つからない場合、WARPデバイスの作成を試みる.
		if (FAILED(dxgiFactory->EnumWarpAdapter(
			IID_PPV_ARGS(&dxgiAdapter)))) {
			return false;
		}
		warp = true;
	}
	if (FAILED(D3D12CreateDevice(
		dxgiAdapter.Get(),
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&device)))) {
		return false;
	}

	// コマンドキューを作成.
	const D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	if (FAILED(device->CreateCommandQueue(
		&cqDesc,
		IID_PPV_ARGS(&commandQueue)))) {
		return false;
	}

	// スワップチェーンを作成.
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = clientWidth;
	scDesc.Height = clientHeight;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = frameBufferCount;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	ComPtr<IDXGISwapChain1> tmpSwapChain;

	if (FAILED(dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(),
		hwnd,
		&scDesc,
		nullptr,
		nullptr,
		&tmpSwapChain))) {
		return false;
	}

	tmpSwapChain.As(&swapChain);
	currentFrameIndex = swapChain->GetCurrentBackBufferIndex();

	// ウィンドウのフルスクリーンを禁止.
	if (FAILED(dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES
		| DXGI_MWA_NO_ALT_ENTER))) {
		return false;
	}

	//  RTV用のディスクリプタヒープ及びディスクリプタの作成.
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = frameBufferCount;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	if (FAILED(device->CreateDescriptorHeap(
		&rtvDesc,
		IID_PPV_ARGS(&rtvDescriptorHeap)))) {
		return false;
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(
		D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// ディスクリプタヒープにディスクリプタを格納
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < frameBufferCount; ++i) {
		if (FAILED(swapChain->GetBuffer(
			i,
			IID_PPV_ARGS(&renderTargetList[i])))) {
			return false;
		}
		device->CreateRenderTargetView(
			renderTargetList[i].Get(),
			nullptr,
			rtvHandle);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	// 深度バッファの作成.
	D3D12_CLEAR_VALUE dsClearValue = {};
	dsClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsClearValue.DepthStencil.Depth = 1.0f;
	dsClearValue.DepthStencil.Stencil = 0;

	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto resDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		clientWidth, clientHeight,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	if (FAILED(device->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsClearValue,
		IID_PPV_ARGS(&depthStencilBuffer)))) {
		return false;
	}

	// DSV用のデスクリプタヒープ及びデスクリプタを作成.
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device->CreateDescriptorHeap(
		&dsvDesc,
		IID_PPV_ARGS(&dsvDescriptorHeap)))) {
		return false;
	}

	// 深度バッファビューの作成.
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(
		depthStencilBuffer.Get(),
		&depthStencilDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// シェーダーリソースビュー用ディスクリプタヒープの作成.
	D3D12_DESCRIPTOR_HEAP_DESC csuDesc = {};
	csuDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	csuDesc.NumDescriptors = 1024;
	csuDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&csuDesc, IID_PPV_ARGS(&csuDescriptorHeap)))) {
		return false;
	}
	csuDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// コマンドアロケータの作成.
	for (int i = 0; i < frameBufferCount; ++i) {
		if (FAILED(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator[i])))) {
			return false;
		}
	}

	// コマンドリストの作成.
	if (FAILED(device->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator[currentFrameIndex].Get(),
		nullptr,
		IID_PPV_ARGS(&commandList)))) {
		return false;
	}
	if (FAILED(commandList->Close())) {
		return false;
	}

	// 最初、最後に実行されるコマンドリストの作成.
	if (FAILED(device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator[currentFrameIndex].Get(),
		nullptr,
		IID_PPV_ARGS(&prologueCommandList)))) {
		return false;
	}
	if (FAILED(prologueCommandList->Close())) {
		return false;
	}

	if (FAILED(device->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator[currentFrameIndex].Get(),
		nullptr,
		IID_PPV_ARGS(&epilogueCommandList)))) {
		return false;
	}
	if (FAILED(epilogueCommandList->Close())) {
		return false;
	}


	// フェンスの作成.
	if (FAILED(device->CreateFence(
		0,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence)))) {
		return false;
	}
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent) {
		return false;
	}
	for (int i = 0; i < frameBufferCount; ++i) {
		fenceValue[i] = 0;
	}
	masterFenceValue = 1;

	// パイプラインステートオブジェクトの生成.
	if (!CreatePSOList(device, warp)) {
		return false;
	}

	ResourceLoader loader;
	if (!loader.Begin(csuDescriptorHeap)) {
		return false;
	}
	if (!spriteRenderer.Init(device, frameBufferCount, 10000)) {
		return false;
	}
	ID3D12CommandList* ppCommandLists[] = { loader.End() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
	WaitForGpu();

	texMap.Init(csuDescriptorHeap);

	// ビューポートの設定.
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = clientWidth;
	viewport.Height = clientHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = clientWidth;
	scissorRect.bottom = clientHeight;

	// ビュープロジェクション行列の初期化.
	const XMMATRIX matOrtho = XMMatrixOrthographicLH(
		static_cast<float>(clientWidth),
		static_cast<float>(clientHeight),
		1.0f,
		1000.0f);
	XMStoreFloat4x4(&matViewProjection, matOrtho);


	return true;
}

/**
* 描画環境の終了処理.
*/
void Graphics::Finalize()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

/**
* 描画開始.
*
* @retval true  正常に描画開始処理が行われた.
* @retval false 描画開始処理中にエラーが発生した.
*/
bool Graphics::BeginRendering()
{

	// 書き換え監視.
	if (!WaitForPreviousFrame()) {
		return false;
	}

	// コマンドアロケータをリセット.
	if (FAILED(commandAllocator[currentFrameIndex]->Reset())) {
		return false;
	}

	// プロローグコマンドリストを作成.
	if (FAILED(prologueCommandList->Reset(
		commandAllocator[currentFrameIndex].Get(), nullptr))) {
		return false;
	}

	auto resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTargetList[currentFrameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	prologueCommandList->ResourceBarrier(1, &resBarrier);
	if (FAILED(prologueCommandList->Close())) {
		return false;
	}

	// エピローグコマンドリストを作成.
	if (FAILED(epilogueCommandList->Reset(
		commandAllocator[currentFrameIndex].Get(), nullptr))) {
		return false;
	}
	resBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		renderTargetList[currentFrameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	epilogueCommandList->ResourceBarrier(1, &resBarrier);
	if (FAILED(epilogueCommandList->Close())) {
		return false;
	}

	// コマンドリストのリセット.
	if (FAILED(commandList->Reset(
		commandAllocator[currentFrameIndex].Get(), nullptr))) {
		return false;
	}

	// 描画するフレームバッファを指定.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =	GetRTVHandle();

	// 深度バッファハンドルを取得.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDSVHandle();

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(	// 塗りつぶし.
		rtvHandle,
		clearColor,
		0,
		nullptr);
	commandList->ClearDepthStencilView(
		dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);	// 深度バッファをクリア.

	// ディスクリプタヒープをグラフィックスパイプラインに設定.
	ID3D12DescriptorHeap* heapList[] = { csuDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	return true;
}

/**
* 描画終了.
*
* @retval true  描画終了処理が正常に行われた.
* @retval false 描画終了処理中にエラーが発生した.
*/
bool Graphics::EndRendering()
{

	// コマンドリストをクローズ.
	if (FAILED(commandList->Close())) {
		return false;
	}

	// コマンドリストを実行.
	ID3D12CommandList* ppCommandLists[] = {
	  prologueCommandList.Get(),
	  commandList.Get(),
	  spriteRenderer.GetCommandList(),
	  epilogueCommandList.Get()
	};

	commandQueue->ExecuteCommandLists(
		_countof(ppCommandLists),
		ppCommandLists);

	// フレームバッファ切り替え.
	if (FAILED(swapChain->Present(1, 0))) {
		return false;
	}

	// CPUへ通知.
	fenceValue[currentFrameIndex] = masterFenceValue;
	if (FAILED(commandQueue->Signal(
		fence.Get(),
		fenceValue[currentFrameIndex]))) {
		return false;
	}
	++masterFenceValue;

	return true;
}

/**
* RTVハンドルを取得する.
*
* @return RTVデスクリプタの位置を示すD3D12_CPU_DESCRIPTOR_HANDLE型の値.
*/
D3D12_CPU_DESCRIPTOR_HANDLE Graphics::GetRTVHandle() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += currentFrameIndex * rtvDescriptorSize;
	return rtvHandle;
}

/**
* DSVハンドルを取得する.
*
* @return DSVデスクリプタの位置を示すD3D12_CPU_DESCRIPTOR_HANDLE型の値.
*/
D3D12_CPU_DESCRIPTOR_HANDLE Graphics::GetDSVHandle() const
{
	return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}


// 書き換え監視関数.
bool Graphics::WaitForPreviousFrame()
{
	if (fenceValue[currentFrameIndex] &&
		fence->GetCompletedValue() < fenceValue[currentFrameIndex]) {
		if (FAILED(fence->SetEventOnCompletion(
			fenceValue[currentFrameIndex],
			fenceEvent))) {
			return false;
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	//++fenceValue[currentFrameIndex];
	currentFrameIndex = swapChain->GetCurrentBackBufferIndex();
	return true;
}


// GPU処理待ち関数.
bool Graphics::WaitForGpu()
{
	const UINT64 currentFenceValue = masterFenceValue;
	if (FAILED(commandQueue->Signal(fence.Get(), currentFenceValue))) {
		return false;
	}
	++masterFenceValue;
	if (FAILED(fence->SetEventOnCompletion(
		currentFenceValue,
		fenceEvent))) {
		return false;
	}
	WaitForSingleObject(fenceEvent, INFINITE);
	return true;
}

/**
* 描画オブジェクトを取得する.
*
* @return 描画オブジェクト.
*/

Graphics& Graphics::Get()
{
	static std::unique_ptr<Graphics> graphics(new Graphics);
	return *graphics;
}