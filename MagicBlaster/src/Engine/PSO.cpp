/**
* @file PSO.cpp
*/
#include "PSO.h"
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <vector>
#include <Windows.h>

using Microsoft::WRL::ComPtr;
std::vector<PSO> psoList;

// ���_�f�[�^�̃C���v�b�g���C�A�E�g TODO.
const D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
		0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* �V�F�[�_��ǂݍ���.
*/
bool LoadShader(
	const wchar_t* filename, // �V�F�[�_�t�@�C���̃p�X
	const char* target, // ���_�V�F�[�_�Ȃ�hvs_5_0�h, �s�N�Z���V�F�[�_�Ȃ�hps_5_0�h
	ID3DBlob** blob) // �V�F�[�_���i�[����ID3DBlob�C���^�[�t�F�C�X�̃|�C���^
{
	ComPtr<ID3DBlob> errorBuffer;
	if (FAILED(D3DCompileFromFile(
		filename,
		nullptr,
		nullptr,
		"main",
		target,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		blob,
		&errorBuffer))) {
		if (errorBuffer) {
			OutputDebugStringA(
				static_cast<char*>(errorBuffer->GetBufferPointer()));
		}
		return false;
	}
	return true;
}

/**
* ���[�g�V�O�l�`����PSO���쐬����.
*
* @param pso �쐬����PSO�I�u�W�F�N�g.
* @param vs  �쐬����PSO�ɐݒ肷�钸�_�V�F�[�_�t�@�C����.
* @param ps  �쐬����PSO�ɐݒ肷��s�N�Z���V�F�[�_�t�@�C����.
*
* @retval true  �쐬����.
* @retval false �쐬���s.
*/
bool CreatePSO(PSO& pso, Microsoft::WRL::ComPtr<ID3D12Device> device, bool warp, const wchar_t* vs, const wchar_t* ps, const D3D12_BLEND_DESC& blendDesc)

{
	ComPtr<ID3DBlob> vertexShaderBlob;
	if (!LoadShader(vs, "vs_5_0", &vertexShaderBlob)) {
		return false;
	}
	ComPtr<ID3DBlob> pixelShaderBlob;
	if (!LoadShader(ps, "ps_5_0", &pixelShaderBlob)) {
		return false;
	}


	// ���[�g�V�O�l�`�����쐬.
	// ���[�g�p�����[�^��ShaderVisibility�͓K�؂ɐݒ肷��K�v������.
	// ���[�g�V�O�l�`�����������ݒ肳��Ă��Ȃ��ꍇ�ł��A�V�O�l�`���̍쐬�ɂ͐������邱�Ƃ�����.
	// ���������̏ꍇ�APSO�쐬���ɃG���[����������.
	{
		D3D12_DESCRIPTOR_RANGE descRange[] = {
			CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0) };
		CD3DX12_ROOT_PARAMETER rootParameters[3];
		rootParameters[0].InitAsDescriptorTable(_countof(descRange), descRange);
		rootParameters[1].InitAsConstants(16, 0);
		rootParameters[2].InitAsConstants(4, 1);	// �V�F�[�_�[�ɒl�𑗂�e�X�g.
		D3D12_STATIC_SAMPLER_DESC staticSampler[] = { CD3DX12_STATIC_SAMPLER_DESC(0) };
		//	�X�v���C�g�I�[�̃o�O�AWRAP����������Ȃ������c�@�ݒ�͎c���Ă���.
		staticSampler[0].AddressU = staticSampler[0].AddressV = staticSampler[0].AddressW
			= D3D12_TEXTURE_ADDRESS_MODE_BORDER;	// �͈͊O�e�N�X�`���̐ݒ� �͈͊O�e�N�X�`����BorderColor�ݒ�F.
		staticSampler[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;	// �͈͊O�e�N�X�`���͊��S�ɓ����ȍ�.
		
		staticSampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;	// �j�A���X�g�l�C�o�[�@.
		D3D12_ROOT_SIGNATURE_DESC rsDesc = {
			_countof(rootParameters),
			rootParameters,
			_countof(staticSampler),
			staticSampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		};

		ComPtr<ID3DBlob> signatureBlob;
		if (FAILED(D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, nullptr))) {
			return false;
		}
		if (FAILED(device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&pso.rootSignature)))) {
			return false;
		}
	}

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = pso.rootSignature.Get();
	psoDesc.VS = {
		vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };
	psoDesc.PS = {
		pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };
	psoDesc.BlendState = blendDesc;
	//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.InputLayout.pInputElementDescs = vertexLayout;
	psoDesc.InputLayout.NumElements =
		sizeof(vertexLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	if (warp) {
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
	}
	if (FAILED(device->CreateGraphicsPipelineState(
		&psoDesc,
		IID_PPV_ARGS(&pso.pso)))) {
		return false;
	}

	return true;
}

/**
* PSO���쐬����.
*/
bool CreatePSOList(Microsoft::WRL::ComPtr<ID3D12Device> device, bool warp)
{
	CD3DX12_BLEND_DESC blendDesc(D3D12_DEFAULT);
	psoList.resize(countof_PSOType);

	
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;

	if (!CreatePSO(
		psoList[PSOType_Simple],
		device,
		warp,
		L"Res/VertexShader.hlsl",
		L"Res/PixelShader.hlsl",
		blendDesc)) {
		return false;
	}

	if (!CreatePSO(
		psoList[PSOType_CircleTexture],
		device,
		warp,
		L"Res/VertexShader.hlsl",
		L"Res/PixelShader.hlsl",
		blendDesc)) {
		return false;
	}

	if (!CreatePSO(
		psoList[PSOType_Sprite],
		device,
		warp,
		L"Res/VertexShader.hlsl",
		L"Res/PixelShader.hlsl",
		blendDesc)) {
		return false;
	}

	if (!CreatePSO(
		psoList[PSOType_NoiseTexture],
		device,
		warp,
		L"Res/VertexShader.hlsl",
		L"Res/NoiseTexture.hlsl",
		blendDesc)) {
		return false;
	}

	if (!CreatePSO(
		psoList[PSOType_Test],
		device,
		warp,
		L"Res/VertexShader.hlsl",
		L"Res/testPixelShader.hlsl",
		blendDesc)) {
		return false;
	}

	return true;
}

/**
* PSO���擾����.
*
* @param type �擾����PSO�̎��.
*
* @return type�ɑΉ�����PSO.
*/
const PSO& GetPSO(PSOType type)
{
	return psoList[type];
}
