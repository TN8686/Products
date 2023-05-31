#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>

/**
* ルートシグネチャとPSOをまとめた構造体.
*/
struct PSO
{
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
};

/**
* PSOの種類.
*/
enum PSOType {
	PSOType_Simple,
	PSOType_CircleTexture,
	PSOType_Sprite,
	PSOType_NoiseTexture,
	PSOType_Test,
	countof_PSOType
};

bool CreatePSOList(Microsoft::WRL::ComPtr<ID3D12Device> device, bool warp);
const PSO& GetPSO(PSOType);
