/**
* @file Texture.cpp
*/
#include "Texture.h"
#include "d3dx12.h"

//namespace Resource
//{

using Microsoft::WRL::ComPtr;

/**
* WICフォーマットから対応するDXGIフォーマットを得る.
*
* @param wicFormat WICフォーマットを示すGUID.
*
* @return wicFormatに対応するDXGIフォーマット.
*         対応するフォーマットが見つからない場合はDXGI_FORMAT_UNKNOWNを返す.
*/
DXGI_FORMAT GetDXGIFormatFromWICFormat(const WICPixelFormatGUID& wicFormat)
{
	static const struct {
		WICPixelFormatGUID guid;
		DXGI_FORMAT format;
	} wicToDxgiList[] = {
		{ GUID_WICPixelFormat128bppRGBAFloat, DXGI_FORMAT_R32G32B32A32_FLOAT },
		{ GUID_WICPixelFormat64bppRGBAHalf, DXGI_FORMAT_R16G16B16A16_FLOAT },
		{ GUID_WICPixelFormat64bppRGBA, DXGI_FORMAT_R16G16B16A16_UNORM },
		{ GUID_WICPixelFormat32bppRGBA, DXGI_FORMAT_R8G8B8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGRA, DXGI_FORMAT_B8G8R8A8_UNORM },
		{ GUID_WICPixelFormat32bppBGR, DXGI_FORMAT_B8G8R8X8_UNORM },
		{ GUID_WICPixelFormat32bppRGBA1010102XR, DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM },
		{ GUID_WICPixelFormat32bppRGBA1010102, DXGI_FORMAT_R10G10B10A2_UNORM },
		{ GUID_WICPixelFormat16bppBGRA5551, DXGI_FORMAT_B5G5R5A1_UNORM },
		{ GUID_WICPixelFormat16bppBGR565, DXGI_FORMAT_B5G6R5_UNORM },
		{ GUID_WICPixelFormat32bppGrayFloat, DXGI_FORMAT_R32_FLOAT },
		{ GUID_WICPixelFormat16bppGrayHalf, DXGI_FORMAT_R16_FLOAT },
		{ GUID_WICPixelFormat16bppGray, DXGI_FORMAT_R16_UNORM },
		{ GUID_WICPixelFormat8bppGray, DXGI_FORMAT_R8_UNORM },
		{ GUID_WICPixelFormat8bppAlpha, DXGI_FORMAT_A8_UNORM },
	};
	for (auto e : wicToDxgiList) {
		if (e.guid == wicFormat) {
			return e.format;
		}
	}
	return DXGI_FORMAT_UNKNOWN;
}

/**
* 任意のWICフォーマットからDXGIフォーマットと互換性のあるWICフォーマットを得る.
*
* @param wicFormat WICフォーマットのGUID.
*
* @return DXGIフォーマットと互換性のあるWICフォーマット.
*         元の形式をできるだけ再現できるようなフォーマットが選ばれる.
*         そのようなフォーマットが見つからない場合はGUID_WICPixelFormatDontCareを返す.
*/
WICPixelFormatGUID GetDXGICompatibleWICFormat(const WICPixelFormatGUID& wicFormat)
{
	static const struct {
		WICPixelFormatGUID guid, compatible;
	} guidToCompatibleList[] = {
		{ GUID_WICPixelFormatBlackWhite, GUID_WICPixelFormat8bppGray },
		{ GUID_WICPixelFormat1bppIndexed, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat2bppIndexed, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat4bppIndexed, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat8bppIndexed, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat2bppGray, GUID_WICPixelFormat8bppGray },
		{ GUID_WICPixelFormat4bppGray, GUID_WICPixelFormat8bppGray },
		{ GUID_WICPixelFormat16bppGrayFixedPoint, GUID_WICPixelFormat16bppGrayHalf },
		{ GUID_WICPixelFormat32bppGrayFixedPoint, GUID_WICPixelFormat32bppGrayFloat },
		{ GUID_WICPixelFormat16bppBGR555, GUID_WICPixelFormat16bppBGRA5551 },
		{ GUID_WICPixelFormat32bppBGR101010, GUID_WICPixelFormat32bppRGBA1010102 },
		{ GUID_WICPixelFormat24bppBGR, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat24bppRGB, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat32bppPBGRA, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat32bppPRGBA, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat48bppRGB, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat48bppBGR, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat64bppBGRA, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat64bppPRGBA, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat64bppPBGRA, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat48bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat48bppBGRFixedPoint, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat64bppRGBAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat64bppBGRAFixedPoint, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat64bppRGBFixedPoint, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat64bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat48bppRGBHalf, GUID_WICPixelFormat64bppRGBAHalf },
		{ GUID_WICPixelFormat128bppPRGBAFloat, GUID_WICPixelFormat128bppRGBAFloat },
		{ GUID_WICPixelFormat128bppRGBFloat, GUID_WICPixelFormat128bppRGBAFloat },
		{ GUID_WICPixelFormat128bppRGBAFixedPoint, GUID_WICPixelFormat128bppRGBAFloat },
		{ GUID_WICPixelFormat128bppRGBFixedPoint, GUID_WICPixelFormat128bppRGBAFloat },
		{ GUID_WICPixelFormat32bppRGBE, GUID_WICPixelFormat128bppRGBAFloat },
		{ GUID_WICPixelFormat32bppCMYK, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat64bppCMYK, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat40bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat80bppCMYKAlpha, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat32bppRGB, GUID_WICPixelFormat32bppRGBA },
		{ GUID_WICPixelFormat64bppRGB, GUID_WICPixelFormat64bppRGBA },
		{ GUID_WICPixelFormat64bppPRGBAHalf, GUID_WICPixelFormat64bppRGBAHalf },
	};
	for (auto e : guidToCompatibleList) {
		if (e.guid == wicFormat) {
			return e.compatible;
		}
	}
	return GUID_WICPixelFormatDontCare;
}

/**
* DXGIフォーマットから1ピクセルのバイト数を得る.
*
* @param dxgiFormat DXGIフォーマット.
*
* @return dxgiFormatに対応するビット数.
*/
int GetDXGIFormatBitesPerPixel(DXGI_FORMAT dxgiFormat)
{
	switch (dxgiFormat) {
	case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
	case DXGI_FORMAT_R16G16B16A16_FLOAT: return 8;
	case DXGI_FORMAT_R16G16B16A16_UNORM: return 8;
	case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
	case DXGI_FORMAT_B8G8R8A8_UNORM: return 4;
	case DXGI_FORMAT_B8G8R8X8_UNORM: return 4;
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM: return 4;
	case DXGI_FORMAT_R10G10B10A2_UNORM: return 4;
	case DXGI_FORMAT_B5G5R5A1_UNORM: return 2;
	case DXGI_FORMAT_B5G6R5_UNORM: return 2;
	case DXGI_FORMAT_R32_FLOAT: return 4;
	case DXGI_FORMAT_R16_FLOAT: return 2;
	case DXGI_FORMAT_R16_UNORM: return 2;
	case DXGI_FORMAT_R8_UNORM: return 1;
	case DXGI_FORMAT_A8_UNORM: return 1;
	default: return 1;
	}
}

/**
* リソース読み込みを開始する.
*
* @param heap テクスチャ用のRTVデスクリプタ取得先のデスクリプタヒープ.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool ResourceLoader::Begin(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap)
{
	descriptorHeap = heap;
	if (FAILED(descriptorHeap->GetDevice(IID_PPV_ARGS(&device)))) {
		return false;
	}
	if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
		return false;
	}
	if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
		return false;
	}
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&imagingFactory)))) {
		return false;
	}
	return true;
}

/**
* リソース読み込みを終了する.
*
* @return コマンドリストへのポインタ.
*/
ID3D12GraphicsCommandList* ResourceLoader::End()
{
	commandList->Close();
	return commandList.Get();
}

/*
* データをデフォルトヒープに転送する.
*/
bool ResourceLoader::Upload(Microsoft::WRL::ComPtr<ID3D12Resource>& defaultHeap, const D3D12_RESOURCE_DESC& desc, D3D12_SUBRESOURCE_DATA data, D3D12_RESOURCE_STATES stateAfter, const wchar_t* name)
{
	// テクスチャ用GPUメモリの確保.
	// デフォルトヒープを作成している.
	// 最も高速にアクセスできるリソース.
	auto herpProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	if (FAILED(device->CreateCommittedResource(
		&herpProp,
		D3D12_HEAP_FLAG_NONE,
		&desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(&defaultHeap)))) {
		return false;
	}
	if (name) {
		defaultHeap->SetName(name);
	}
	
	UINT64 heapSize;
	device->GetCopyableFootprints(&desc, 0, 1, 0, nullptr, nullptr, nullptr, &heapSize);
	auto uploadHerpProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(heapSize);
	ComPtr<ID3D12Resource> uploadHeap;
	if (FAILED(device->CreateCommittedResource(
		&uploadHerpProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&uploadHeap)
	))) {
		return false;
	}
	if (UpdateSubresources<1>(commandList.Get(), defaultHeap.Get(), uploadHeap.Get(), 0, 0, 1, &data) == 0) {
		return false;
	}
	auto resBarrire = CD3DX12_RESOURCE_BARRIER::Transition(defaultHeap.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &resBarrire);
	uploadHeapList.push_back(uploadHeap);

	return true;
}

/**
* バイト列からテクスチャを作成する.
*
* @param texture  作成したテクスチャを管理するオブジェクト.
* @param index    作成したテクスチャ用のRTVデスクリプタのインデックス.
* @param desc     テクスチャの詳細情報.
* @param data     テクスチャ作成に使用するバイト列へのポインタ.
* @param name     テクスチャリソースに付ける名前(デバッグ用). nullptrを渡すと名前を付けない.
*
* @retval true  作成成功.
* @retval false 作成失敗.
*/
bool ResourceLoader::Create(Texture& texture, int index, const D3D12_RESOURCE_DESC& desc, const void* data, const wchar_t* name)
{
	ComPtr<ID3D12Resource> textureBuffer;
	const int bytesPerRow = static_cast<int>(desc.Width * GetDXGIFormatBitesPerPixel(desc.Format));
	D3D12_SUBRESOURCE_DATA subresource = { data, bytesPerRow, static_cast<LONG_PTR>(bytesPerRow * desc.Height) };
	if (!Upload(textureBuffer, desc, subresource, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, name)) {
		return false;
	}

	device->CreateShaderResourceView(textureBuffer.Get(), nullptr, CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), index, descriptorSize));

	texture.resource = textureBuffer;
	texture.format = desc.Format;
	texture.handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), index, descriptorSize);
	
	texture.width = desc.Width;
	texture.height = desc.Height;

	return true;
}

/**
* ファイルからテクスチャを読み込む.
*
* @param texture   読み込んだテクスチャを管理するオブジェクト.
* @param index     読み込んだテクスチャ用のRTVデスクリプタのインデックス.
* @param filename  テクスチャファイル名.
*
* @retval true  読み込み成功.
* @retval false 読み込み失敗.
*/
bool ResourceLoader::LoadFromFile(Texture& texture, int index, const wchar_t* filename)
{
	ComPtr<IWICBitmapDecoder> decoder;
	if (FAILED(imagingFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, decoder.GetAddressOf()))) {
		return false;
	}
	ComPtr<IWICBitmapFrameDecode> frame;
	if (FAILED(decoder->GetFrame(0, frame.GetAddressOf()))) {
		return false;
	}
	WICPixelFormatGUID wicFormat;
	if (FAILED(frame->GetPixelFormat(&wicFormat))) {
		return false;
	}
	UINT width, height;
	if (FAILED(frame->GetSize(&width, &height))) {
		return false;
	}
	DXGI_FORMAT dxgiFormat = GetDXGIFormatFromWICFormat(wicFormat);
	bool imageConverted = false;
	ComPtr<IWICFormatConverter> converter;
	if (dxgiFormat == DXGI_FORMAT_UNKNOWN) {
		const WICPixelFormatGUID compatibleFormat = GetDXGICompatibleWICFormat(wicFormat);
		if (compatibleFormat == GUID_WICPixelFormatDontCare) {
			return false;
		}
		dxgiFormat = GetDXGIFormatFromWICFormat(compatibleFormat);
		if (FAILED(imagingFactory->CreateFormatConverter(converter.GetAddressOf()))) {
			return false;
		}
		BOOL canConvert = FALSE;
		if (FAILED(converter->CanConvert(wicFormat, compatibleFormat, &canConvert))) {
			return false;
		}
		if (!canConvert) {
			return false;
		}
		if (FAILED(converter->Initialize(frame.Get(), compatibleFormat, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeCustom))) {
			return false;
		}
		imageConverted = true;
	}
	const int bytesPerRow = width * GetDXGIFormatBitesPerPixel(dxgiFormat);
	const int imageSize = bytesPerRow * height;
	std::vector<uint8_t> imageData;
	imageData.resize(imageSize);
	if (imageConverted) {
		if (FAILED(converter->CopyPixels(nullptr, bytesPerRow, imageSize, imageData.data()))) {
			return false;
		}
	} else {
		if (FAILED(frame->CopyPixels(nullptr, bytesPerRow, imageSize, imageData.data()))) {
			return false;
		}
	}

	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(dxgiFormat, width, height, 1, 1);
	if (!Create(texture, index, desc, imageData.data(), filename)) {
		return false;
	}
	return true;
}

/**
* 初期化する.
*
* @param heap テクスチャ用のCSUデスクリプタ取得先のデスクリプタヒープ.
*/
void TextureMap::Init(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap)
{
	descriptorHeap = heap;
	size_t numDescriptors = descriptorHeap->GetDesc().NumDescriptors;
	const uint16_t avairableCount = numDescriptors > 4096 ? 4096 : static_cast<uint16_t>(numDescriptors);
	freeIDList.resize(avairableCount);
	uint16_t id = avairableCount - 1;
	for (size_t i = 0; i < avairableCount; ++i) {
		freeIDList[i] = id;
		--id;
	}
}

/**
* リソース読み込みを開始する.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool TextureMap::Begin()
{
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	if (FAILED(descriptorHeap->GetDevice(IID_PPV_ARGS(&device)))) {
		return false;
	}
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	loader.reset(new ResourceLoader);
	loader->Begin(descriptorHeap);
	return true;
}

/**
* リソース読み込みを終了する.
*
* @return コマンドリストへのポインタ.
*/
ID3D12GraphicsCommandList* TextureMap::End()
{
	auto ret = loader->End();
	loader.release();
	return ret;
}

/**
* 名前が一致するテクスチャを検索する.
*
* @param texture  戻り値がtrueの場合に、発見したテクスチャを格納するオブジェクト.
* @param name     検索するテクスチャ名.
*
* @retval true  nameに一致するテクスチャを発見.
* @retval false nameに一致するテクスチャを保持していない.
*/
bool TextureMap::Find(Texture& texture, const wchar_t* name)
{
	auto itr = map.find(name);
	if (itr == map.end()) {
		return false;
	}
	texture = itr->second;
	return true;
}

/**
* バイト列からテクスチャを作成する.
*
* @param texture  作成したテクスチャを管理するオブジェクト.
* @param name     テクスチャリソースに付ける名前.
* @param desc     テクスチャの詳細情報.
* @param data     テクスチャ作成に使用するバイト列へのポインタ.
*
* @retval true  作成成功.
* @retval false 作成失敗.
*/
bool TextureMap::Create(Texture& texture, const wchar_t* name, const D3D12_RESOURCE_DESC& desc, const void* data)
{
	if (Find(texture, name)) {
		return true;
	}

	if (freeIDList.empty()) {
		return false;
	}
	const int index = freeIDList.back();
	if (loader->Create(texture, index, desc, data, name)) {
		freeIDList.pop_back();
		map.insert(std::make_pair(name, texture));
		return true;
	}
	return false;
}

/**
* ファイルからテクスチャを読み込む.
*
* @param texture   読み込んだテクスチャを管理するオブジェクト.
* @param filename  テクスチャファイル名.
*
* @retval true  読み込み成功.
* @retval false 読み込み失敗.
*/
bool TextureMap::LoadFromFile(Texture& texture, const wchar_t* filename)
{
	if (Find(texture, filename)) {
		return true;
	}

	if (freeIDList.empty()) {
		return false;
	}
	const int index = freeIDList.back();
	if (loader->LoadFromFile(texture, index, filename)) {
		freeIDList.pop_back();
		map.insert(std::make_pair(filename, texture));
		return true;
	}
	return false;
}

/**
* 参照されなくなったテクスチャを破棄する.
*/
void TextureMap::GC()
{
	auto itr = map.begin();
	while (itr != map.end()) {	//mapを走査.
		itr->second.resource.Get()->AddRef();	// 一旦参照カウンタをインクリメントしてから.
		const ULONG refCount = itr->second.resource.Get()->Release(); // Release()でデクリメントしつつ参照カウンタを取得.
		if (refCount == 1) {	// 参照カウンタが1だったら解放してよい.
			const int index = static_cast<int>((itr->second.handle.ptr - descriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr) / descriptorSize);	// 解放するインデックスを取得.
			itr = map.erase(itr);	// 解放.
			freeIDList.push_back(index);	// 解放したIDを空きIDリストに登録.
		} else {
			++itr;
		}
	}
}

/*
* 単色の矩形テクスチャを生成します.
*
* @param texture    読み込んだテクスチャを管理するオブジェクト.
* @param index      読み込んだテクスチャ用のRTVデスクリプタのインデックス.
* @param width      生成するテクスチャの幅
* @param height     生成するテクスチャの高さ
* @param r          赤.
* @param g          緑.
* @param b          青.
* @param a          不透明度.
* @param name		テクスチャリソースに付ける名前.
*
* @retval true      生成成功.
* @retval false     生成失敗.
*/

bool TextureMap::CreateRect(Texture& texture, UINT64 width, UINT height, uint8_t r, uint8_t g, uint8_t b, uint8_t a, const wchar_t* name) {
	std::vector<uint8_t> data;
	data.resize(width * height * 4);

	auto end = data.end();

	for (auto itr = data.begin(); itr != end; itr += 4) {
		*itr = r;
		*(itr + 1) = g;
		*(itr + 2) = b;
		*(itr + 3) = a;
	}

	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);
	if (!Create(texture, name, desc, data.data())) {
		return false;
	}

	return true;
}

/*
* 単色の十字のテクスチャを生成します.
*
* @param texture    読み込んだテクスチャを管理するオブジェクト.
* @param index      読み込んだテクスチャ用のRTVデスクリプタのインデックス.
* @param width      生成するテクスチャの幅　偶数の場合左に寄ります.
* @param height     生成するテクスチャの高さ　偶数の場合上に寄ります.
* @param r          赤.
* @param g          緑.
* @param b          青.
* @param a          不透明度.
*
* @retval true      生成成功.
* @retval false     生成失敗.
*/
/*
bool TextureMap::CreateCross(Texture& texture, int index, UINT64 width, UINT height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	std::vector<uint8_t> data;
	data.resize(width * height * 4, 0x00);

	auto end = data.end();
	auto begin = data.begin();
	int dotWidth = width * 4;
	for (auto itr = begin; itr != end; itr += 4) {
		int aa = (itr - begin) / 4 / width;
		int bb = (height - 1) / 2;
		if ((itr - begin) / 4 % width == (width - 1) / 2 || (itr - begin) / 4 / width == (height - 1) / 2) {
			*itr = r;
			*(itr + 1) = g;
			*(itr + 2) = b;
			*(itr + 3) = a;
		}
	}

	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);
	if (!Create(texture, index, desc, data.data(), nullptr)) {
		return false;
	}

	return true;
}
*/

/*
* 枠付き矩形テクスチャを生成します.
*
* @param texture    生成したテクスチャを管理するオブジェクト.
* @param index      生成したテクスチャ用のRTVデスクリプタのインデックス.
* @param width      生成するテクスチャの幅
* @param height     生成するテクスチャの高さ
* @param r          内側の赤.
* @param g          内側の緑.
* @param b          内側の青.
* @param a          内側の不透明度.
* @param fr         枠の赤.
* @param fg         枠の緑.
* @param fb         枠の青.
* @param fa         枠の不透明度.
*
* @retval true      生成成功.
* @retval false     生成失敗.
*/
/*
bool TextureMap::CreateRect(Texture& texture, int index, UINT64 width, UINT height, uint8_t r, uint8_t g, uint8_t b, uint8_t a,
	uint8_t fr, uint8_t fg, uint8_t fb, uint8_t fa) {
	std::vector<uint8_t> data;
	data.resize(width * height * 4);

	auto end = data.end();
	auto begin = data.begin();
	int dotWidth = width * 4;
	for (auto itr = begin; itr != end; itr += 4) {
		if (itr - begin < dotWidth || (itr - begin) % dotWidth == 0 || (itr - begin) % dotWidth == dotWidth - 4 || itr - begin >= dotWidth * (height - 1)) {
			*itr = fr;
			*(itr + 1) = fg;
			*(itr + 2) = fb;
			*(itr + 3) = fa;
		}
		else {
			*itr = r;
			*(itr + 1) = g;
			*(itr + 2) = b;
			*(itr + 3) = a;
		}
	}


	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1);
	if (!Create(texture, index, desc, data.data(), nullptr)) {
		return false;
	}

	return true;
}*/

/*
* 単色の円のテクスチャを生成します.
*
* @param texture    読み込んだテクスチャを管理するオブジェクト.
* @param index      読み込んだテクスチャ用のRTVデスクリプタのインデックス.
* @param radius      生成するテクスチャの円の半径
* @param r          赤.
* @param g          緑.
* @param b          青.
* @param a          不透明度.
*
* @retval true      生成成功.
* @retval false     生成失敗.
*/
/*
bool TextureMap::CreateCircle(Texture& texture, int index, UINT radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	std::vector<uint8_t> data;
	int diameter = radius * 2;
	data.resize(diameter * diameter * 4);
	for (int y = 0; y < diameter; ++y) {
		for (int x = 0; x < diameter; ++x) {
			if ((x - radius + 1) * (x - radius + 1) + (y - radius + 1) * (y - radius + 1) <= ((radius) * (radius)) && (x - radius + 1) * (x - radius + 1) + (y - radius + 1) * (y - radius + 1) >= (radius - 1) * (radius - 1)) {
				data[(x + (y * diameter)) * 4] = r;
				data[(x + (y * diameter)) * 4 + 1] = g;
				data[(x + (y * diameter)) * 4 + 2] = b;
				data[(x + (y * diameter)) * 4 + 3] = a;
			}
		}
	}

	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, diameter, diameter, 1, 1);
	if (!Create(texture, index, desc, data.data(), nullptr)) {
		return false;
	}

	return true;

}*/

/*
* 枠付きの円のテクスチャを生成します.
*
* @param texture    読み込んだテクスチャを管理するオブジェクト.
* @param index      読み込んだテクスチャ用のRTVデスクリプタのインデックス.
* @param radius     生成するテクスチャの円の半径
* @param r          赤.
* @param g          緑.
* @param b          青.
* @param a          不透明度.
* @param fr         枠の赤.
* @param fg         枠の緑.
* @param fb         枠の青.
* @param fa         枠の不透明度.
*
* @retval true      生成成功.
* @retval false     生成失敗.
*/
/*
bool TextureMap::CreateCircle(Texture& texture, int index, UINT radius, uint8_t r, uint8_t g, uint8_t b, uint8_t a, uint8_t fr, uint8_t fg, uint8_t fb, uint8_t fa) {
	std::vector<uint8_t> data;
	int diameter = radius * 2;
	data.resize(diameter * diameter * 4);
	for (int y = 0; y < diameter; ++y) {
		for (int x = 0; x < diameter; ++x) {

			if ((x - radius + 1) * (x - radius + 1) + (y - radius + 1) * (y - radius + 1) < ((radius) * (radius))) {
				if ((x - radius + 1) * (x - radius + 1) + (y - radius + 1) * (y - radius + 1) >= (radius - 1) * (radius - 1)) {
					data[(x + (y * diameter)) * 4] = fr;
					data[(x + (y * diameter)) * 4 + 1] = fg;
					data[(x + (y * diameter)) * 4 + 2] = fb;
					data[(x + (y * diameter)) * 4 + 3] = fa;
				}
				else {
					data[(x + (y * diameter)) * 4] = r;
					data[(x + (y * diameter)) * 4 + 1] = g;
					data[(x + (y * diameter)) * 4 + 2] = b;
					data[(x + (y * diameter)) * 4 + 3] = a;
				}
			}
		}
	}

	const D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM, diameter, diameter, 1, 1);
	if (!Create(texture, index, desc, data.data(), nullptr)) {
		return false;
	}

	return true;
}
*/
//} // namespace Resource