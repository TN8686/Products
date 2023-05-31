#include "Graphics.h"
#include "PSO.h"
#include "d3dx12.h"
#include <cmath>
#include <memory>


using Microsoft::WRL::ComPtr;
using namespace DirectX;

// ���_�f�[�^�^.
struct Vertex
{
	XMFLOAT3 position; // ���W.
	XMFLOAT4 color; // �F.
	XMFLOAT2 texcoord; // �e�N�X�`�����W.
};

/**
* �`����̏�����.
*
* @param hwnd         �E�B���h�E�n���h��.
* @param clientWidth  �E�B���h�E�̕�.
* @param clientHeight �E�B���h�E�̍���.
*
* @retval true  ����������.
* @retval false ���������s.
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

	// �@�\���x��11�𖞂����n�[�h�E�F�A�A�_�v�^���������A���̃f�o�C�X�C���^�[�t�F�C�X���擾����.
	ComPtr<IDXGIAdapter1> dxgiAdapter; // �f�o�C�X�����擾����C���^�[�t�F�C�X.
	int adapterIndex = 0; // �񋓂���f�o�C�X�̃C���f�b�N�X.
	bool adapterFound = false; // �ړI�̃f�o�C�X�������������ǂ���.
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
		// �@�\���x��11�𖞂����n�[�h�E�F�A��������Ȃ��ꍇ�AWARP�f�o�C�X�̍쐬�����݂�.
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

	// �R�}���h�L���[���쐬.
	const D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	if (FAILED(device->CreateCommandQueue(
		&cqDesc,
		IID_PPV_ARGS(&commandQueue)))) {
		return false;
	}

	// �X���b�v�`�F�[�����쐬.
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

	// �E�B���h�E�̃t���X�N���[�����֎~.
	if (FAILED(dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_WINDOW_CHANGES
		| DXGI_MWA_NO_ALT_ENTER))) {
		return false;
	}

	//  RTV�p�̃f�B�X�N���v�^�q�[�v�y�уf�B�X�N���v�^�̍쐬.
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

	// �f�B�X�N���v�^�q�[�v�Ƀf�B�X�N���v�^���i�[
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

	// �[�x�o�b�t�@�̍쐬.
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

	// DSV�p�̃f�X�N���v�^�q�[�v�y�уf�X�N���v�^���쐬.
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (FAILED(device->CreateDescriptorHeap(
		&dsvDesc,
		IID_PPV_ARGS(&dsvDescriptorHeap)))) {
		return false;
	}

	// �[�x�o�b�t�@�r���[�̍쐬.
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

	device->CreateDepthStencilView(
		depthStencilBuffer.Get(),
		&depthStencilDesc,
		dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// �V�F�[�_�[���\�[�X�r���[�p�f�B�X�N���v�^�q�[�v�̍쐬.
	D3D12_DESCRIPTOR_HEAP_DESC csuDesc = {};
	csuDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	csuDesc.NumDescriptors = 1024;
	csuDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	if (FAILED(device->CreateDescriptorHeap(&csuDesc, IID_PPV_ARGS(&csuDescriptorHeap)))) {
		return false;
	}
	csuDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// �R�}���h�A���P�[�^�̍쐬.
	for (int i = 0; i < frameBufferCount; ++i) {
		if (FAILED(device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(&commandAllocator[i])))) {
			return false;
		}
	}

	// �R�}���h���X�g�̍쐬.
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

	// �ŏ��A�Ō�Ɏ��s�����R�}���h���X�g�̍쐬.
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


	// �t�F���X�̍쐬.
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

	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���.
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

	// �r���[�|�[�g�̐ݒ�.
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

	// �r���[�v���W�F�N�V�����s��̏�����.
	const XMMATRIX matOrtho = XMMatrixOrthographicLH(
		static_cast<float>(clientWidth),
		static_cast<float>(clientHeight),
		1.0f,
		1000.0f);
	XMStoreFloat4x4(&matViewProjection, matOrtho);


	return true;
}

/**
* �`����̏I������.
*/
void Graphics::Finalize()
{
	WaitForGpu();
	CloseHandle(fenceEvent);
}

/**
* �`��J�n.
*
* @retval true  ����ɕ`��J�n�������s��ꂽ.
* @retval false �`��J�n�������ɃG���[����������.
*/
bool Graphics::BeginRendering()
{

	// ���������Ď�.
	if (!WaitForPreviousFrame()) {
		return false;
	}

	// �R�}���h�A���P�[�^�����Z�b�g.
	if (FAILED(commandAllocator[currentFrameIndex]->Reset())) {
		return false;
	}

	// �v�����[�O�R�}���h���X�g���쐬.
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

	// �G�s���[�O�R�}���h���X�g���쐬.
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

	// �R�}���h���X�g�̃��Z�b�g.
	if (FAILED(commandList->Reset(
		commandAllocator[currentFrameIndex].Get(), nullptr))) {
		return false;
	}

	// �`�悷��t���[���o�b�t�@���w��.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =	GetRTVHandle();

	// �[�x�o�b�t�@�n���h�����擾.
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetDSVHandle();

	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	commandList->ClearRenderTargetView(	// �h��Ԃ�.
		rtvHandle,
		clearColor,
		0,
		nullptr);
	commandList->ClearDepthStencilView(
		dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);	// �[�x�o�b�t�@���N���A.

	// �f�B�X�N���v�^�q�[�v���O���t�B�b�N�X�p�C�v���C���ɐݒ�.
	ID3D12DescriptorHeap* heapList[] = { csuDescriptorHeap.Get() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	return true;
}

/**
* �`��I��.
*
* @retval true  �`��I������������ɍs��ꂽ.
* @retval false �`��I���������ɃG���[����������.
*/
bool Graphics::EndRendering()
{

	// �R�}���h���X�g���N���[�Y.
	if (FAILED(commandList->Close())) {
		return false;
	}

	// �R�}���h���X�g�����s.
	ID3D12CommandList* ppCommandLists[] = {
	  prologueCommandList.Get(),
	  commandList.Get(),
	  spriteRenderer.GetCommandList(),
	  epilogueCommandList.Get()
	};

	commandQueue->ExecuteCommandLists(
		_countof(ppCommandLists),
		ppCommandLists);

	// �t���[���o�b�t�@�؂�ւ�.
	if (FAILED(swapChain->Present(1, 0))) {
		return false;
	}

	// CPU�֒ʒm.
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
* RTV�n���h�����擾����.
*
* @return RTV�f�X�N���v�^�̈ʒu������D3D12_CPU_DESCRIPTOR_HANDLE�^�̒l.
*/
D3D12_CPU_DESCRIPTOR_HANDLE Graphics::GetRTVHandle() const
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle =
		rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += currentFrameIndex * rtvDescriptorSize;
	return rtvHandle;
}

/**
* DSV�n���h�����擾����.
*
* @return DSV�f�X�N���v�^�̈ʒu������D3D12_CPU_DESCRIPTOR_HANDLE�^�̒l.
*/
D3D12_CPU_DESCRIPTOR_HANDLE Graphics::GetDSVHandle() const
{
	return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}


// ���������Ď��֐�.
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


// GPU�����҂��֐�.
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
* �`��I�u�W�F�N�g���擾����.
*
* @return �`��I�u�W�F�N�g.
*/

Graphics& Graphics::Get()
{
	static std::unique_ptr<Graphics> graphics(new Graphics);
	return *graphics;
}