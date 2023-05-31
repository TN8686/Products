#include "../Reference.h"
#include "Sprite.h"
#include "Texture.h"
#include "PSO.h"
#include "d3dx12.h"
#include "../Camera.h"
#include <iostream>

using Microsoft::WRL::ComPtr;
using namespace DirectX;

/**
* スプライト描画用頂点データ型.
*/
struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
	XMFLOAT2 texcoord;
    XMFLOAT4 addColor;
};

XMFLOAT3 RotateZ(XMVECTOR c, float x, float y, float r)
{
    float fs, fc;
    XMScalarSinCos(&fs, &fc, r);    // 角度から余弦と正弦を出す.
    const float rx = fc * x + fs * y;   // 座標の回転の公式.
    const float ry = -fs * x + fc * y;
    const XMVECTORF32 tmp{ rx, ry, 0.0f, 0.0f };    // 結果を格納.
    XMFLOAT3 ret;
    XMStoreFloat3(&ret, XMVectorAdd(c, tmp));   // 元のデータに結果のベクトルを加算し、XMFLOAT3に変換して返す.
    return ret;
}

/**
* ひとつのスプライトデータを頂点バッファに設定.
*
* @param sprite スプライトデータ.
* @param v      頂点データを描き込むアドレス.
* @param offset スクリーン左上座標.
*/
void AddVertex(const Sprite& sprite, const Cell* cell, const AnimationData animeData, Vertex* v, XMFLOAT2 offset, const Camera* camera = nullptr)
{
    const XMVECTORF32 center{ offset.x + sprite.pos.x + cell->offset.x * sprite.scale.x, 
                              offset.y - sprite.pos.y - cell->offset.y * sprite.scale.y, sprite.pos.z, 0.0f };

    const XMFLOAT2 halfSize{ cell->ssize.x * 0.5f * sprite.scale.x * animeData.scale.x, cell->ssize.y * 0.5f * sprite.scale.y * animeData.scale.y };

    const XMVECTOR vcolor0 = XMVectorMultiply(XMLoadFloat4(&sprite.color[0]), XMLoadFloat4(&animeData.color));
    const XMVECTOR vcolor1 = XMVectorMultiply(XMLoadFloat4(&sprite.color[1]), XMLoadFloat4(&animeData.color));

    // 反転フラグ　スプライトとアニメーションデータで二つ反転フラグが存在するので、排他的論理和でフラグを決定.
    bool flipHorizontal = sprite.flipHorizontal ^ animeData.flipHorizontal;
    bool flipVertical = sprite.flipVertical ^ animeData.flipVertical;

    // 回転 反転していたら回転方向も反転させる.
    float rotation;
    if (flipHorizontal ^ flipVertical) {
        rotation = sprite.rotation + animeData.rotation;
    }
    else {
        rotation = -(sprite.rotation + animeData.rotation);
    }

    // 上下左右反転処理　三項演算子にしようかと思ったけど可読性落ちる気がしてやめた.
    float top, bottom, right, left;
    if (flipHorizontal) {
        left = cell->uv.x + cell->tsize.x;
        right = cell->uv.x;
    }
    else {
        left = cell->uv.x;
        right = cell->uv.x + cell->tsize.x;
    }

    if (flipVertical) {
        top = cell->uv.y + cell->tsize.y;
        bottom = cell->uv.y;
    }
    else {
        top = cell->uv.y;
        bottom = cell->uv.y + cell->tsize.y;
    }

    // 各頂点を処理　サンプルを元にしてこうなっているけど、本当は行列で扱えるようにした方がよい？.
    v[0].position = RotateZ(center, -halfSize.x + sprite.rectOffset.left * sprite.scale.x, halfSize.y - sprite.rectOffset.top * sprite.scale.y, rotation);
    v[0].texcoord.x = left;
    v[0].texcoord.y = top;
    v[0].addColor = sprite.addColor;
    XMStoreFloat4(&v[0].color, vcolor0);

    v[1].position = RotateZ(center, halfSize.x + sprite.rectOffset.right * sprite.scale.x, halfSize.y - sprite.rectOffset.top * sprite.scale.y, rotation);
    v[1].texcoord.x = right;
    v[1].texcoord.y = top;
    v[1].addColor = sprite.addColor;
    XMStoreFloat4(&v[1].color, vcolor0);

    v[2].position = RotateZ(center, halfSize.x + sprite.rectOffset.right * sprite.scale.x, -halfSize.y - sprite.rectOffset.bottom * sprite.scale.y, rotation);
    v[2].texcoord.x = right;
    v[2].texcoord.y = bottom;
    v[2].addColor = sprite.addColor;
    XMStoreFloat4(&v[2].color, vcolor1);

    v[3].position = RotateZ(center, -halfSize.x + sprite.rectOffset.left * sprite.scale.x, -halfSize.y - sprite.rectOffset.bottom * sprite.scale.y, rotation);
    v[3].texcoord.x = left;
    v[3].texcoord.y = bottom;
    v[3].addColor = sprite.addColor;
    XMStoreFloat4(&v[3].color, vcolor1);

    // カメラ基準での回転.
    if (camera != nullptr && camera->GetRotate() && sprite.cameraRelative) {
        const XMVECTORF32 cCenter = { 0, 0, sprite.pos.z, 0 };
        float cRotate = -camera->GetRotate();

        v[0].position = RotateZ(cCenter, v[0].position.x, v[0].position.y, cRotate);
        v[1].position = RotateZ(cCenter, v[1].position.x, v[1].position.y, cRotate);
        v[2].position = RotateZ(cCenter, v[2].position.x, v[2].position.y, cRotate);
        v[3].position = RotateZ(cCenter, v[3].position.x, v[3].position.y, cRotate);
    }
    

}

/**
* コンストラクタ.
*/
Sprite::Sprite(const AnimationList* al, XMFLOAT3 p, float r, XMFLOAT2 s, XMFLOAT4 c, XMFLOAT4 ac, bool fh, bool fv, bool cr) :
    animeController(al), pos(p), rotation(r), scale(s), color{ c, c }, addColor(ac), velocity{ 0, 0 }, flipHorizontal(fh), flipVertical(fv), cameraRelative(cr)
{
}

Sprite::Sprite(XMFLOAT3 p, const AnimationList* al, float r, XMFLOAT2 s, XMFLOAT4 c, XMFLOAT4 ac, bool fh, bool fv, bool cr) :
    animeController(al), pos(p), rotation(r), scale(s), color{ c, c }, addColor(ac), velocity{ 0, 0 }, flipHorizontal(fh), flipVertical(fv), cameraRelative(cr)
{
}


// spriteObjの変換用.
Sprite::Sprite(const SpriteObj& spriteObj)
{
    auto objpos = spriteObj.GetWorldPosition();
    pos = { objpos.x, objpos.y, 0.0f };
    rotation = spriteObj.rotation;
    onActive = spriteObj.GetOnActive();
    scale = spriteObj.scale;
    animeController = spriteObj.animeController;
    addColor = spriteObj.addColor;
    cameraRelative = spriteObj.cameraRelative;
    color[0] = color[1] = spriteObj.color;
    flipHorizontal = spriteObj.flipHorizontal;
    flipVertical = spriteObj.flipVertical;
    rectOffset = spriteObj.rectOffset;
}

// spriteObjの変換用.
Sprite& Sprite::operator=(const SpriteObj& spriteObj) {

    animeController = spriteObj.animeController;
    auto objpos = spriteObj.GetWorldPosition();
    pos = { objpos.x, objpos.y, 0.0f };
    rotation = spriteObj.rotation;
    onActive = spriteObj.GetOnActive();
    scale = spriteObj.scale;
    color[0] = color[1] = spriteObj.color;
    addColor = spriteObj.addColor;
    cameraRelative = spriteObj.cameraRelative;
    flipHorizontal = spriteObj.flipHorizontal;
    flipVertical = spriteObj.flipVertical;
    rectOffset = spriteObj.rectOffset;
    return *this;
}

SpriteRenderer::SpriteRenderer() :
    maxSpriteCount(0),
    currentFrameIndex(-1),
    spriteCount(0)
{
}

/**
* Rendererを初期化する.
*
* @param device           D3Dデバイス.
* @param frameBufferCount フレームバッファの数.
* @param maxSprite        描画できる最大スプライト数.
*
* @retval true  初期化成功.
* @retval false 初期化失敗.
*/
bool SpriteRenderer::Init(ComPtr<ID3D12Device> device, int frameBufferCount, unsigned int maxSprite)
{
    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(maxSprite * sizeof(Vertex));

    frameResourceList.resize(frameBufferCount);
    for (int i = 0; i < frameBufferCount; ++i) {
        if (FAILED(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&frameResourceList[i].commandAllocator)))) {
            return false;
        }
        if (FAILED(device->CreateCommittedResource(
            &heapProp,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&frameResourceList[i].vertexBuffer)))) {
            return false;
        }
        frameResourceList[i].vertexBuffer->SetName(L"Sprite Vertex Buffer");
        CD3DX12_RANGE range(0, 0);
        if (FAILED(frameResourceList[i].vertexBuffer->Map(
            0,
            &range,
            &frameResourceList[i].vertexBufferGPUAddress))) {
            return false;
        }
        frameResourceList[i].vertexBufferView.BufferLocation =
            frameResourceList[i].vertexBuffer->GetGPUVirtualAddress();
        frameResourceList[i].vertexBufferView.StrideInBytes = 
            sizeof(Vertex);
        frameResourceList[i].vertexBufferView.SizeInBytes =
            static_cast<UINT>(maxSprite * sizeof(Vertex));
    }

    if (FAILED(device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        frameResourceList[0].commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&commandList)))) {
        return false;
    }
    if (FAILED(commandList->Close())) {
        return false;
    }

    heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    resDesc = CD3DX12_RESOURCE_DESC::Buffer(maxSprite * 6 * sizeof(DWORD));
    if (FAILED(device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&indexBuffer)))) 
    {
        return false;
    }
    indexBuffer->SetName(L"Sprite Index Buffer");
    CD3DX12_RANGE range(0, 0);
    void* tmpIndexBufferAddress;
    if (FAILED(indexBuffer->Map(0, &range, &tmpIndexBufferAddress))) {
        return false;
    }
    DWORD* pIndexBuffer = static_cast<DWORD*>(tmpIndexBufferAddress);
    for (size_t i = 0; i < maxSprite; ++i) {
        pIndexBuffer[i * 6 + 0] = i * 4 + 0;
        pIndexBuffer[i * 6 + 1] = i * 4 + 1;
        pIndexBuffer[i * 6 + 2] = i * 4 + 2;
        pIndexBuffer[i * 6 + 3] = i * 4 + 2;
        pIndexBuffer[i * 6 + 4] = i * 4 + 3;
        pIndexBuffer[i * 6 + 5] = i * 4 + 0;
    }
    indexBuffer->Unmap(0, nullptr);
    indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
    indexBufferView.SizeInBytes = maxSprite * 6 * sizeof(DWORD);
    indexBufferView.Format = DXGI_FORMAT_R32_UINT;

    return true;
}


// 参考にしてる教材のサンプルソースがそれぞれバージョンが滅茶苦茶なのでその対応.
/**
* スプライトの描画開始.
* @param frameIndex 現在のフレームバッファのインデックス.
*
* @retval true 描画可能な状態になった.
* @retval false 描画可能な状態への遷移に失敗.
*/
bool SpriteRenderer::Begin(int frameIndex)
{
    if (currentFrameIndex >= 0) {
        return false;
    }

    currentFrameIndex = frameIndex;
    FrameResource& fr = frameResourceList[currentFrameIndex];

    if (FAILED(fr.commandAllocator->Reset())) {
        return false;
    }
    if (FAILED(commandList->Reset(fr.commandAllocator.Get(), nullptr))) {
        return false;
    }
    spriteCount = 0;
    return true;
}



/**
* スプライトを描画する.
*
* @param spriteList 描画するスプライトのリスト.
* @param cellList   描画に使用するセルのリスト　nullptrを渡すと画像全体を一つのセルとして扱う.
* @param pso        描画に使用するPSO.
* @param texture    描画に使用するテクスチャ.
* @param info       描画情報.
* @param camera     描画に使用するカメラ.
*
* @retval true  コマンドリスト作成成功.
* @retval false コマンドリスト作成失敗.
*/
bool SpriteRenderer::Draw(const std::vector<Sprite>& spriteList, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info, const Camera* camera)
{
    if (spriteList.empty()) {
        return true;
    }
    return Draw(&*spriteList.begin(), (&*spriteList.begin()) + spriteList.size(), cellList, pso, texture, info, camera);
}

bool SpriteRenderer::Draw(const Sprite* sprite, const Cell* cellList, const PSO& pso, const Texture& texture, RenderingInfo& info, const Camera* camera)
{
    return Draw(sprite, sprite + 1, cellList, pso, texture, info, camera);
}

bool SpriteRenderer::Draw(const SpriteResource* sprRes, const PSO& pso, RenderingInfo& info, const Camera* camera)
{
    auto list = sprRes->pGetSpriteList();
    if (list->empty()) {    // スプライトが空だったか正常終了.
        return true;
    }

    const Cell* pCellList = nullptr;
    if (!sprRes->pGetCellList()->empty()) { // セルリストが空の場合はnullptr、そうでない場合は先頭のアドレスを渡す.
        pCellList = sprRes->pGetCellList()->data();
    }

    return Draw(&*list->begin(), (&*list->begin()) + list->size(), pCellList, pso, sprRes->GetTexture(), info, camera);
}

bool SpriteRenderer::Draw(const SpriteResource2* sprRes, const PSO& pso, RenderingInfo& info, const Camera* camera) {
    auto list = sprRes->pGetList();
    if (list->empty()) {    // スプライトが空だったか正常終了.
        return true;
    }

    const Cell* pCellList = nullptr;
    if (!sprRes->pGetCellList()->empty()) { // セルリストが空の場合はnullptr、そうでない場合は先頭のアドレスを渡す.
        pCellList = sprRes->pGetCellList()->data();
    }

 

    return Draw(&*list->begin(), &*(list->begin()) + list->size(), pCellList, pso, sprRes->GetTexture(), info, camera);
}

bool SpriteRenderer::Draw(
    const Sprite* first, const Sprite* last,
    const Cell* cellList,
    const PSO& pso,
    const Texture& texture,
    RenderingInfo& info,
    const Camera* camera)

{
    if (currentFrameIndex < 0) {
        return false;
    }

    if (first == last) {
        return true;
    }
    if (cellList == nullptr) {
        const Cell tempCell = Cell({ { 0.0f, 0.0f }, { 1.0f, 1.0f }, { static_cast<float>(texture.width), static_cast<float>(texture.height) }, { 0, 0 }, 0 } );
        cellList = &tempCell;
    };

    FrameResource& fr = frameResourceList[currentFrameIndex];

    //無駄削減test.
    if (spriteCount == 0) {
        commandList->SetGraphicsRootSignature(pso.rootSignature.Get());
        commandList->SetPipelineState(pso.pso.Get());
        ID3D12DescriptorHeap* heapList[] = { info.texDescHeap };
        commandList->SetDescriptorHeaps(_countof(heapList), heapList);

        commandList->SetGraphicsRoot32BitConstants(1, 16, &info.matViewProjection, 0);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &fr.vertexBufferView);
        commandList->IASetIndexBuffer(&indexBufferView);
        commandList->OMSetRenderTargets(1, &info.rtvHandle, FALSE, &info.dsvHandle);
        commandList->RSSetViewports(1, &info.viewport);
        commandList->RSSetScissorRects(1, &info.scissorRect);
    }

    commandList->SetGraphicsRootDescriptorTable(0, texture.handle);
    
    const XMFLOAT2 offset(-(info.viewport.Width * 0.5f), info.viewport.Height * 0.5f);  // 左上座標
    const int maxSprite =
        (fr.vertexBufferView.SizeInBytes / fr.vertexBufferView.StrideInBytes / 4) - spriteCount;
    int numSprite = 0;
    Vertex* v = static_cast<Vertex*>(fr.vertexBufferGPUAddress) + (spriteCount * 4);
    for (const auto* sprite = first; sprite != last; ++sprite) {
        auto tempSprite = *sprite;

        if (!tempSprite.onActive) {
            continue;
        }

        // commandList->SetGraphicsRoot32BitConstants(2, 4, &tempSprite.addColor, 0);  // 加算色補正情報をシェーダーに送る.
        // はずだったけどコレだと複数のスプライトを一度に描画した時に最後に送られた値になっちゃうからダメだった.
        // 頂点情報に持たせてシェーダーに渡す方式に変更.

        // 2Dゲーム前提で、描画する毎にZバッファをズラしていきZバッファと描画順を強制的に一致させる.
        // スプライト終端の表示がおかしくなるバグにZバッファが関係している模様.
        // なぜかZバッファの値によってスプライトの端に異常が出る座標が変わり、0に近いほど座標値に対しての発生頻度が低いので.
        // 対処療法だがひとまず非常に小さな値にしている.
        tempSprite.pos.z = 0.001 - static_cast<float>(spriteCount + numSprite) / 10485760;

        // どうやらZバッファに関する設定を何か間違えている模様　値が大きいほど拡大され、500程度の値以下から表示されている.
        // TODO 要検証、修正.
        //static int a = 0;
        //tempSprite.pos.z = 500 - static_cast<float>(spriteCount + numSprite + a) / 65536;
        //a+= 100;

        if (tempSprite.scale.x == 0 || tempSprite.scale.y == 0) {
            continue;
        }
        const Cell* cell = cellList + tempSprite.GetCellIndex();

        // カメラ座標に合わせる.
        if (camera != nullptr && tempSprite.cameraRelative) {
            tempSprite.pos.x = ((tempSprite.pos.x - camera->GetWorldPosition().x) * camera->GetZoomFactor() + clientWidth / 2);
            tempSprite.pos.y = ((tempSprite.pos.y - camera->GetWorldPosition().y) * camera->GetZoomFactor() + clientHeight / 2);
            tempSprite.scale.x *= camera->GetZoomFactor();
            tempSprite.scale.y *= camera->GetZoomFactor();
        }

        // 頂点バッファに設定.
        AddVertex(tempSprite, cell, tempSprite.animeController.GetAnimationData(), v, offset, camera);
        ++numSprite;
        if (numSprite >= maxSprite) {
            break;
        }
        v += 4;

    }
    commandList->DrawIndexedInstanced(numSprite * 6, 1, 0, spriteCount * 4, 0);
    spriteCount += numSprite;

    return true;
}

// 無理矢理だけど、SpriteObj用Draw関数　SpriteObjのポインタのリストを元に渡す.
bool SpriteRenderer::Draw(
    const SpriteObj* const* first, const SpriteObj* const* last,
    const Cell* cellList,
    const PSO& pso,
    const Texture& texture,
    RenderingInfo& info,
    const Camera* camera)

{
    if (currentFrameIndex < 0) {
        return false;
    }

    if (first == last) {
        return true;
    }
    if (cellList == nullptr) {
        const Cell tempCell = Cell({ { 0.0f, 0.0f }, { 1.0f, 1.0f }, { static_cast<float>(texture.width), static_cast<float>(texture.height) }, { 0, 0 }, 0 });
        cellList = &tempCell;
    };

    FrameResource& fr = frameResourceList[currentFrameIndex];

    //無駄削減test.
    if (spriteCount == 0) {
        commandList->SetGraphicsRootSignature(pso.rootSignature.Get());
        commandList->SetPipelineState(pso.pso.Get());
        ID3D12DescriptorHeap* heapList[] = { info.texDescHeap };
        commandList->SetDescriptorHeaps(_countof(heapList), heapList);

        commandList->SetGraphicsRoot32BitConstants(1, 16, &info.matViewProjection, 0);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->IASetVertexBuffers(0, 1, &fr.vertexBufferView);
        commandList->IASetIndexBuffer(&indexBufferView);
        commandList->OMSetRenderTargets(1, &info.rtvHandle, FALSE, &info.dsvHandle);
        commandList->RSSetViewports(1, &info.viewport);
        commandList->RSSetScissorRects(1, &info.scissorRect);
    }

    commandList->SetGraphicsRootDescriptorTable(0, texture.handle);

    const XMFLOAT2 offset(-(info.viewport.Width * 0.5f), info.viewport.Height * 0.5f);  // 左上座標
    const int maxSprite =
        (fr.vertexBufferView.SizeInBytes / fr.vertexBufferView.StrideInBytes / 4) - spriteCount;
    int numSprite = 0;
    Vertex* v = static_cast<Vertex*>(fr.vertexBufferGPUAddress) + (spriteCount * 4);
    for (const auto* sprite = first; sprite != last; ++sprite) {
        if (!(**sprite).GetOnActive()) {
            continue;
        }

        Sprite tempSprite = **sprite;

        // commandList->SetGraphicsRoot32BitConstants(2, 4, &tempSprite.addColor, 0);  // 加算色補正情報をシェーダーに送る.
        // はずだったけどコレだと複数のスプライトを一度に描画した時に最後に送られた値になっちゃうからダメだった.
        // 頂点情報に持たせてシェーダーに渡す方式に変更.

        // 2Dゲーム前提で、描画する毎にZバッファをズラしていきZバッファと描画順を強制的に一致させる.
        // スプライト終端の表示がおかしくなるバグにZバッファが関係している模様.
        // なぜかZバッファの値によってスプライトの端に異常が出る座標が変わり、0に近いほど座標値に対しての発生頻度が低いので.
        // 対処療法だがひとまず非常に小さな値にしている.
        tempSprite.pos.z = 0.001 - static_cast<float>(spriteCount + numSprite) / 10485760;

        // どうやらZバッファに関する設定を何か間違えている模様　値が大きいほど拡大され、500程度の値以下から表示されている.
        // TODO 要検証、修正.
        //static int a = 0;
        //tempSprite.pos.z = 500 - static_cast<float>(spriteCount + numSprite + a) / 65536;
        //a+= 100;

        if (tempSprite.scale.x == 0 || tempSprite.scale.y == 0) {
            continue;
        }
        const Cell* cell = cellList + tempSprite.GetCellIndex();

        // カメラ座標に合わせる.
        if (camera != nullptr && tempSprite.cameraRelative) {
            tempSprite.pos.x = ((tempSprite.pos.x - camera->GetWorldPosition().x) * camera->GetZoomFactor() + clientWidth / 2);
            tempSprite.pos.y = ((tempSprite.pos.y - camera->GetWorldPosition().y) * camera->GetZoomFactor() + clientHeight / 2);
            tempSprite.scale.x *= camera->GetZoomFactor();
            tempSprite.scale.y *= camera->GetZoomFactor();
        }

        // 頂点バッファに設定.
        AddVertex(tempSprite, cell, tempSprite.animeController.GetAnimationData(), v, offset, camera);
        ++numSprite;
        if (numSprite >= maxSprite) {
            break;
        }
        v += 4;

    }
    commandList->DrawIndexedInstanced(numSprite * 6, 1, 0, spriteCount * 4, 0);
    spriteCount += numSprite;

    return true;
}

/**
* スプライトの描画終了.
*
* @retval true  コマンドリスト作成成功.
* @retval false コマンドリスト作成失敗.
*/
bool SpriteRenderer::End()
{
    currentFrameIndex = -1;
    if (FAILED(commandList->Close())) {
        return false;
    }
    return true;
}

/**
* コマンドリストを取得する.
*
* @return ID3D12GraphicsCommandListインターフェイスへのポインタ.
*/
ID3D12GraphicsCommandList* SpriteRenderer::GetCommandList()
{
    return commandList.Get();
}

/**
* フォントファイルを読み込む.
*
* @param filename フォントファイル名.
*
* @retval true  読み込み成功.
* @retval false 読み込み失敗.
*/
CellList LoadFontFromFile(const wchar_t* filename)
{
    std::unique_ptr<std::remove_pointer<HANDLE>::type, decltype(&CloseHandle)> h(
        CreateFileW(filename, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr),
        CloseHandle
    );
    if (h.get() == INVALID_HANDLE_VALUE) {
        return {};
    }
    LARGE_INTEGER size;
    if (!GetFileSizeEx(h.get(), &size)) {
        return {};
    }
    std::string buffer;
    buffer.resize(static_cast<size_t>(size.QuadPart));
    DWORD readBytes;
    if (!ReadFile(h.get(), &buffer[0], buffer.size(), &readBytes, nullptr)) {
        return {};
    }

    int line = 1;
    char name[128];
    size_t nextOffset = 0;
    int ret = sscanf(&buffer[nextOffset], "info face=%127s size=%*d bold=%*d italic=%*d charset=%*s"
        " unicode=%*d stretchH=%*d smooth=%*d aa=%*d padding=%*d,%*d,%*d,%*d spacing=%*d,%*d", name);
    if (ret < 1) {
        std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line << ")" << std::endl;
        return {};
    }
    ++line;

    nextOffset = buffer.find('\n', nextOffset) + 1;
    XMFLOAT2 scale;
    ret = sscanf(&buffer[nextOffset], " common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d", &scale.x, &scale.y);
    if (ret < 2) {
        std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line << ")" << std::endl;
        return {};
    }
    const XMFLOAT2 reciprocalScale(1.0f / scale.x, 1.0f / scale.y);
    ++line;

    nextOffset = buffer.find('\n', nextOffset) + 1;
    ret = sscanf(&buffer[nextOffset], " page id=%*d file=%*s");
    ++line;

    nextOffset = buffer.find('\n', nextOffset) + 1;
    int charCount;
    ret = sscanf(&buffer[nextOffset], " chars count=%d", &charCount);
    if (ret < 1) {
        std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line << ")" << std::endl;
        return {};
    }
    ++line;

    CellList fontList;
    fontList.list.resize(256);
    for (int i = 0; i < charCount; ++i) {
        nextOffset = buffer.find('\n', nextOffset);
        if (nextOffset == std::string::npos) {
            break;
        }
        ++nextOffset;

        unsigned int charCode;
        Cell cell;
        XMFLOAT2 uv, size, offset;
        float xadvance;
        ret = sscanf(&buffer[nextOffset], " char id=%u x=%f y=%f width=%f height=%f xoffset=%f yoffset=%f xadvance=%f page=%*d chnl=%*d", &charCode, &uv.x, &uv.y, &size.x, &size.y, &offset.x, &offset.y, &xadvance);
        if (ret < 8) {
            std::cerr << "ERROR: '" << filename << "'の読み込みに失敗(line=" << line << ")" << std::endl;
            return {};
        }
        cell.uv = XMFLOAT2(uv.x * reciprocalScale.x, uv.y * reciprocalScale.y);
        cell.tsize = XMFLOAT2(size.x * reciprocalScale.x, size.y * reciprocalScale.y);
        cell.ssize = XMFLOAT2(size.x, size.y);
        cell.offset = XMFLOAT2(size.x * 0.5f + offset.x, size.y * 0.5f + offset.y);
        cell.xadvance = xadvance;
        if (charCode >= 0 && charCode < fontList.list.size()) {
            fontList.list[charCode] = cell;
        }
        ++line;
    }
    fontList.name.assign(name + 1);
    fontList.name.pop_back();
    return fontList;
}

/**
* 文字列の描画範囲の横幅を取得する.
*
* @param cellList 文字フォント情報を格納しているセルリスト.
* @param text     文字列.
*
* @return 文字列の横幅.
*/
float GetTextWidth(const CellList& cellList, const char* text)
{
    float width = 0;
    while (*text != '\0') {
        width += cellList.list[*text].xadvance;
        ++text;
    }
    return width;
}

// 以下は、1から自作したもの.

// 旧CreateCellList関数　基本的にSpriteResource版を使用してください.
int CreateCellList(int AllNum, int XNum, int YNum, int XSize, int YSize, CellList& CellListBuf) {
    assert(CellListBuf.list.size() < AllNum || CellListBuf.list.size() >= XNum * YNum);
    if (CellListBuf.list.size() < AllNum || CellListBuf.list.size() < XNum * YNum) { // 配列の要素数が不足していたらエラー　勝手にresizeはしない.
        return -1;
    }

    XMFLOAT2 size = { static_cast<float>(XSize), static_cast<float>(YSize) };
    for (int y = 0; y < YNum; y++) {
        for (int x = 0; x < XNum; x++) {
            CellListBuf.list[y * XNum + x] = { XMFLOAT2{static_cast<float>(x) / XNum, static_cast<float>(y) / YNum}, XMFLOAT2{1.0f / XNum, 1.0f / YNum}, size };
        }
    }

    return 0;
};


// SpriteResource

bool SpriteResource::CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize)
{
    size_t allNum = xNum * yNum;    // 全要素数を取得.
    _cellList.resize(allNum);       // 要素数でresize.
    assert(_cellList.size() >= allNum);
    if (_cellList.size() < allNum) { // 一応配列の要素数が不足していたらエラー.
        return false;
    }

    // セルリストを生成.
    XMFLOAT2 size = { static_cast<float>(xSize), static_cast<float>(ySize) };   // セルの構造情報.
    size_t index = 0;                                                           // 要素番号.
    for (int y = 0; y < yNum; ++y) {                                            // 二次元構造に合わせた二重ループ.
        for (int x = 0; x < xNum; ++x, ++index) {
            _cellList.at(index) = { XMFLOAT2{static_cast<float>(x) / xNum, static_cast<float>(y) / yNum}, XMFLOAT2{1.0f / xNum, 1.0f / yNum}, size };   // セル情報を作成.
        }
    }

    return true;
}

bool SpriteResource::Register(SpriteHandle& spriteHandrer)
{
    // 既に登録されている物の場合、一旦登録を解除する.
    if (spriteHandrer._hasIndex) {
        spriteHandrer.Release();
    }

    size_t tempIndex = -1;
    if (_freeSpriteIDList.empty()) {                  	// 未使用インデックスリストが空の場合.
        tempIndex = _spriteList.size();
        _spriteList.emplace_back(&_animationList);	    // リストに登録.
    }
    else {						            			// 未使用インデックスリストがある場合.
        tempIndex = _freeSpriteIDList.back();	        // 未使用のインデックスを一つpop.
        _freeSpriteIDList.pop_back();
        Sprite initSpr(&_animationList);
        _spriteList.at(tempIndex) = initSpr;            // 対象スプライトを初期化.
    }

    spriteHandrer._hasIndex = true;     // インデックス保有状態をtrueに.
    spriteHandrer._index = tempIndex;   // インデックスを保持.
    spriteHandrer._pRegisteredResource = this;       // 自身を親に登録.
    return true;
}

bool SpriteResource::Release(SpriteHandle& hSprite) {
    if (hSprite._pRegisteredResource != this || !hSprite._hasIndex) {    // 登録が自身でないオブジェクトの場合と、スプライトのインデックスを保有していない場合.
        return false;
    }

    _spriteList.at(hSprite._index).isUsed = false;      // 現在は利用していない変数だが、一応isUsedをfalseに.
    _spriteList.at(hSprite._index).onActive = false;    // 非アクティブに.
    _freeSpriteIDList.emplace_back(hSprite._index);     // 未使用スプライトとしてインデックスを登録.
    hSprite._hasIndex = false;                          // インデックスを未所有状態に.
    return true;
}


// SpriteHandle

Sprite* SpriteHandle::pGetSprite()
{
    assert(_pRegisteredResource != nullptr);
    if (_pRegisteredResource != nullptr) {           // 親が未登録状態の場合.
        if (!_hasIndex) {               // インデックスを保有していない場合.
            _pRegisteredResource->Register(*this);   // インデックスを取得しに行く.
        }
        return &_pRegisteredResource->pGetSpriteList()->at(_index);  // ポインタを返す.
    }
    return nullptr;
}

void SpriteHandle::Release()
{
    if (_pRegisteredResource == nullptr || !_hasIndex) { // nullptrチェックと、一応Releaseでチェックされるはずだがインデックスを保有していない場合もここで止める.
        return;
    }
    _pRegisteredResource->Release(*this);
}

void SpriteHandle::Unregister()
{
    Release();
    _pRegisteredResource = nullptr;
}

bool SpriteResource2::CreateCellList(size_t xNum, size_t yNum, int xSize, int ySize)
{
    size_t allNum = xNum * yNum;    // 全要素数を取得.
    _cellList.resize(allNum);       // 要素数でresize.
    assert(_cellList.size() >= allNum);
    if (_cellList.size() < allNum) { // 一応配列の要素数が不足していたらエラー.
        return false;
    }

    // セルリストを生成.
    XMFLOAT2 size = { static_cast<float>(xSize), static_cast<float>(ySize) };   // セルの構造情報.
    size_t index = 0;                                                           // 要素番号.
    for (int y = 0; y < yNum; ++y) {                                            // 二次元構造に合わせた二重ループ.
        for (int x = 0; x < xNum; ++x, ++index) {
            _cellList.at(index) = { XMFLOAT2{static_cast<float>(x) / xNum, static_cast<float>(y) / yNum}, XMFLOAT2{1.0f / xNum, 1.0f / yNum}, size };   // セル情報を作成.
        }
    }

    return true;
}