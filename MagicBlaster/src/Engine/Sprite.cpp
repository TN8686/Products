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
* �X�v���C�g�`��p���_�f�[�^�^.
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
    XMScalarSinCos(&fs, &fc, r);    // �p�x����]���Ɛ������o��.
    const float rx = fc * x + fs * y;   // ���W�̉�]�̌���.
    const float ry = -fs * x + fc * y;
    const XMVECTORF32 tmp{ rx, ry, 0.0f, 0.0f };    // ���ʂ��i�[.
    XMFLOAT3 ret;
    XMStoreFloat3(&ret, XMVectorAdd(c, tmp));   // ���̃f�[�^�Ɍ��ʂ̃x�N�g�������Z���AXMFLOAT3�ɕϊ����ĕԂ�.
    return ret;
}

/**
* �ЂƂ̃X�v���C�g�f�[�^�𒸓_�o�b�t�@�ɐݒ�.
*
* @param sprite �X�v���C�g�f�[�^.
* @param v      ���_�f�[�^��`�����ރA�h���X.
* @param offset �X�N���[��������W.
*/
void AddVertex(const Sprite& sprite, const Cell* cell, const AnimationData animeData, Vertex* v, XMFLOAT2 offset, const Camera* camera = nullptr)
{
    const XMVECTORF32 center{ offset.x + sprite.pos.x + cell->offset.x * sprite.scale.x, 
                              offset.y - sprite.pos.y - cell->offset.y * sprite.scale.y, sprite.pos.z, 0.0f };

    const XMFLOAT2 halfSize{ cell->ssize.x * 0.5f * sprite.scale.x * animeData.scale.x, cell->ssize.y * 0.5f * sprite.scale.y * animeData.scale.y };

    const XMVECTOR vcolor0 = XMVectorMultiply(XMLoadFloat4(&sprite.color[0]), XMLoadFloat4(&animeData.color));
    const XMVECTOR vcolor1 = XMVectorMultiply(XMLoadFloat4(&sprite.color[1]), XMLoadFloat4(&animeData.color));

    // ���]�t���O�@�X�v���C�g�ƃA�j���[�V�����f�[�^�œ���]�t���O�����݂���̂ŁA�r���I�_���a�Ńt���O������.
    bool flipHorizontal = sprite.flipHorizontal ^ animeData.flipHorizontal;
    bool flipVertical = sprite.flipVertical ^ animeData.flipVertical;

    // ��] ���]���Ă������]���������]������.
    float rotation;
    if (flipHorizontal ^ flipVertical) {
        rotation = sprite.rotation + animeData.rotation;
    }
    else {
        rotation = -(sprite.rotation + animeData.rotation);
    }

    // �㉺���E���]�����@�O�����Z�q�ɂ��悤���Ǝv�������ǉǐ�������C�����Ă�߂�.
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

    // �e���_�������@�T���v�������ɂ��Ă����Ȃ��Ă��邯�ǁA�{���͍s��ň�����悤�ɂ��������悢�H.
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

    // �J������ł̉�].
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
* �R���X�g���N�^.
*/
Sprite::Sprite(const AnimationList* al, XMFLOAT3 p, float r, XMFLOAT2 s, XMFLOAT4 c, XMFLOAT4 ac, bool fh, bool fv, bool cr) :
    animeController(al), pos(p), rotation(r), scale(s), color{ c, c }, addColor(ac), velocity{ 0, 0 }, flipHorizontal(fh), flipVertical(fv), cameraRelative(cr)
{
}

Sprite::Sprite(XMFLOAT3 p, const AnimationList* al, float r, XMFLOAT2 s, XMFLOAT4 c, XMFLOAT4 ac, bool fh, bool fv, bool cr) :
    animeController(al), pos(p), rotation(r), scale(s), color{ c, c }, addColor(ac), velocity{ 0, 0 }, flipHorizontal(fh), flipVertical(fv), cameraRelative(cr)
{
}


// spriteObj�̕ϊ��p.
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

// spriteObj�̕ϊ��p.
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
* Renderer������������.
*
* @param device           D3D�f�o�C�X.
* @param frameBufferCount �t���[���o�b�t�@�̐�.
* @param maxSprite        �`��ł���ő�X�v���C�g��.
*
* @retval true  ����������.
* @retval false ���������s.
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


// �Q�l�ɂ��Ă鋳�ނ̃T���v���\�[�X�����ꂼ��o�[�W�������Œ��ꒃ�Ȃ̂ł��̑Ή�.
/**
* �X�v���C�g�̕`��J�n.
* @param frameIndex ���݂̃t���[���o�b�t�@�̃C���f�b�N�X.
*
* @retval true �`��\�ȏ�ԂɂȂ���.
* @retval false �`��\�ȏ�Ԃւ̑J�ڂɎ��s.
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
* �X�v���C�g��`�悷��.
*
* @param spriteList �`�悷��X�v���C�g�̃��X�g.
* @param cellList   �`��Ɏg�p����Z���̃��X�g�@nullptr��n���Ɖ摜�S�̂���̃Z���Ƃ��Ĉ���.
* @param pso        �`��Ɏg�p����PSO.
* @param texture    �`��Ɏg�p����e�N�X�`��.
* @param info       �`����.
* @param camera     �`��Ɏg�p����J����.
*
* @retval true  �R�}���h���X�g�쐬����.
* @retval false �R�}���h���X�g�쐬���s.
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
    if (list->empty()) {    // �X�v���C�g���󂾂���������I��.
        return true;
    }

    const Cell* pCellList = nullptr;
    if (!sprRes->pGetCellList()->empty()) { // �Z�����X�g����̏ꍇ��nullptr�A�����łȂ��ꍇ�͐擪�̃A�h���X��n��.
        pCellList = sprRes->pGetCellList()->data();
    }

    return Draw(&*list->begin(), (&*list->begin()) + list->size(), pCellList, pso, sprRes->GetTexture(), info, camera);
}

bool SpriteRenderer::Draw(const SpriteResource2* sprRes, const PSO& pso, RenderingInfo& info, const Camera* camera) {
    auto list = sprRes->pGetList();
    if (list->empty()) {    // �X�v���C�g���󂾂���������I��.
        return true;
    }

    const Cell* pCellList = nullptr;
    if (!sprRes->pGetCellList()->empty()) { // �Z�����X�g����̏ꍇ��nullptr�A�����łȂ��ꍇ�͐擪�̃A�h���X��n��.
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

    //���ʍ팸test.
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
    
    const XMFLOAT2 offset(-(info.viewport.Width * 0.5f), info.viewport.Height * 0.5f);  // ������W
    const int maxSprite =
        (fr.vertexBufferView.SizeInBytes / fr.vertexBufferView.StrideInBytes / 4) - spriteCount;
    int numSprite = 0;
    Vertex* v = static_cast<Vertex*>(fr.vertexBufferGPUAddress) + (spriteCount * 4);
    for (const auto* sprite = first; sprite != last; ++sprite) {
        auto tempSprite = *sprite;

        if (!tempSprite.onActive) {
            continue;
        }

        // commandList->SetGraphicsRoot32BitConstants(2, 4, &tempSprite.addColor, 0);  // ���Z�F�␳�����V�F�[�_�[�ɑ���.
        // �͂����������ǃR�����ƕ����̃X�v���C�g����x�ɕ`�悵�����ɍŌ�ɑ���ꂽ�l�ɂȂ����Ⴄ����_��������.
        // ���_���Ɏ������ăV�F�[�_�[�ɓn�������ɕύX.

        // 2D�Q�[���O��ŁA�`�悷�閈��Z�o�b�t�@���Y�����Ă���Z�o�b�t�@�ƕ`�揇�������I�Ɉ�v������.
        // �X�v���C�g�I�[�̕\�������������Ȃ�o�O��Z�o�b�t�@���֌W���Ă���͗l.
        // �Ȃ���Z�o�b�t�@�̒l�ɂ���ăX�v���C�g�̒[�Ɉُ킪�o����W���ς��A0�ɋ߂��قǍ��W�l�ɑ΂��Ă̔����p�x���Ⴂ�̂�.
        // �Ώ��Ö@�����ЂƂ܂����ɏ����Ȓl�ɂ��Ă���.
        tempSprite.pos.z = 0.001 - static_cast<float>(spriteCount + numSprite) / 10485760;

        // �ǂ����Z�o�b�t�@�Ɋւ���ݒ�������ԈႦ�Ă���͗l�@�l���傫���قǊg�傳��A500���x�̒l�ȉ�����\������Ă���.
        // TODO �v���؁A�C��.
        //static int a = 0;
        //tempSprite.pos.z = 500 - static_cast<float>(spriteCount + numSprite + a) / 65536;
        //a+= 100;

        if (tempSprite.scale.x == 0 || tempSprite.scale.y == 0) {
            continue;
        }
        const Cell* cell = cellList + tempSprite.GetCellIndex();

        // �J�������W�ɍ��킹��.
        if (camera != nullptr && tempSprite.cameraRelative) {
            tempSprite.pos.x = ((tempSprite.pos.x - camera->GetWorldPosition().x) * camera->GetZoomFactor() + clientWidth / 2);
            tempSprite.pos.y = ((tempSprite.pos.y - camera->GetWorldPosition().y) * camera->GetZoomFactor() + clientHeight / 2);
            tempSprite.scale.x *= camera->GetZoomFactor();
            tempSprite.scale.y *= camera->GetZoomFactor();
        }

        // ���_�o�b�t�@�ɐݒ�.
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

// ����������ǁASpriteObj�pDraw�֐��@SpriteObj�̃|�C���^�̃��X�g�����ɓn��.
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

    //���ʍ팸test.
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

    const XMFLOAT2 offset(-(info.viewport.Width * 0.5f), info.viewport.Height * 0.5f);  // ������W
    const int maxSprite =
        (fr.vertexBufferView.SizeInBytes / fr.vertexBufferView.StrideInBytes / 4) - spriteCount;
    int numSprite = 0;
    Vertex* v = static_cast<Vertex*>(fr.vertexBufferGPUAddress) + (spriteCount * 4);
    for (const auto* sprite = first; sprite != last; ++sprite) {
        if (!(**sprite).GetOnActive()) {
            continue;
        }

        Sprite tempSprite = **sprite;

        // commandList->SetGraphicsRoot32BitConstants(2, 4, &tempSprite.addColor, 0);  // ���Z�F�␳�����V�F�[�_�[�ɑ���.
        // �͂����������ǃR�����ƕ����̃X�v���C�g����x�ɕ`�悵�����ɍŌ�ɑ���ꂽ�l�ɂȂ����Ⴄ����_��������.
        // ���_���Ɏ������ăV�F�[�_�[�ɓn�������ɕύX.

        // 2D�Q�[���O��ŁA�`�悷�閈��Z�o�b�t�@���Y�����Ă���Z�o�b�t�@�ƕ`�揇�������I�Ɉ�v������.
        // �X�v���C�g�I�[�̕\�������������Ȃ�o�O��Z�o�b�t�@���֌W���Ă���͗l.
        // �Ȃ���Z�o�b�t�@�̒l�ɂ���ăX�v���C�g�̒[�Ɉُ킪�o����W���ς��A0�ɋ߂��قǍ��W�l�ɑ΂��Ă̔����p�x���Ⴂ�̂�.
        // �Ώ��Ö@�����ЂƂ܂����ɏ����Ȓl�ɂ��Ă���.
        tempSprite.pos.z = 0.001 - static_cast<float>(spriteCount + numSprite) / 10485760;

        // �ǂ����Z�o�b�t�@�Ɋւ���ݒ�������ԈႦ�Ă���͗l�@�l���傫���قǊg�傳��A500���x�̒l�ȉ�����\������Ă���.
        // TODO �v���؁A�C��.
        //static int a = 0;
        //tempSprite.pos.z = 500 - static_cast<float>(spriteCount + numSprite + a) / 65536;
        //a+= 100;

        if (tempSprite.scale.x == 0 || tempSprite.scale.y == 0) {
            continue;
        }
        const Cell* cell = cellList + tempSprite.GetCellIndex();

        // �J�������W�ɍ��킹��.
        if (camera != nullptr && tempSprite.cameraRelative) {
            tempSprite.pos.x = ((tempSprite.pos.x - camera->GetWorldPosition().x) * camera->GetZoomFactor() + clientWidth / 2);
            tempSprite.pos.y = ((tempSprite.pos.y - camera->GetWorldPosition().y) * camera->GetZoomFactor() + clientHeight / 2);
            tempSprite.scale.x *= camera->GetZoomFactor();
            tempSprite.scale.y *= camera->GetZoomFactor();
        }

        // ���_�o�b�t�@�ɐݒ�.
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
* �X�v���C�g�̕`��I��.
*
* @retval true  �R�}���h���X�g�쐬����.
* @retval false �R�}���h���X�g�쐬���s.
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
* �R�}���h���X�g���擾����.
*
* @return ID3D12GraphicsCommandList�C���^�[�t�F�C�X�ւ̃|�C���^.
*/
ID3D12GraphicsCommandList* SpriteRenderer::GetCommandList()
{
    return commandList.Get();
}

/**
* �t�H���g�t�@�C����ǂݍ���.
*
* @param filename �t�H���g�t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
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
        std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
        return {};
    }
    ++line;

    nextOffset = buffer.find('\n', nextOffset) + 1;
    XMFLOAT2 scale;
    ret = sscanf(&buffer[nextOffset], " common lineHeight=%*d base=%*d scaleW=%f scaleH=%f pages=%*d packed=%*d", &scale.x, &scale.y);
    if (ret < 2) {
        std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
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
        std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
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
            std::cerr << "ERROR: '" << filename << "'�̓ǂݍ��݂Ɏ��s(line=" << line << ")" << std::endl;
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
* ������̕`��͈͂̉������擾����.
*
* @param cellList �����t�H���g�����i�[���Ă���Z�����X�g.
* @param text     ������.
*
* @return ������̉���.
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

// �ȉ��́A1���玩�삵������.

// ��CreateCellList�֐��@��{�I��SpriteResource�ł��g�p���Ă�������.
int CreateCellList(int AllNum, int XNum, int YNum, int XSize, int YSize, CellList& CellListBuf) {
    assert(CellListBuf.list.size() < AllNum || CellListBuf.list.size() >= XNum * YNum);
    if (CellListBuf.list.size() < AllNum || CellListBuf.list.size() < XNum * YNum) { // �z��̗v�f�����s�����Ă�����G���[�@�����resize�͂��Ȃ�.
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
    size_t allNum = xNum * yNum;    // �S�v�f�����擾.
    _cellList.resize(allNum);       // �v�f����resize.
    assert(_cellList.size() >= allNum);
    if (_cellList.size() < allNum) { // �ꉞ�z��̗v�f�����s�����Ă�����G���[.
        return false;
    }

    // �Z�����X�g�𐶐�.
    XMFLOAT2 size = { static_cast<float>(xSize), static_cast<float>(ySize) };   // �Z���̍\�����.
    size_t index = 0;                                                           // �v�f�ԍ�.
    for (int y = 0; y < yNum; ++y) {                                            // �񎟌��\���ɍ��킹����d���[�v.
        for (int x = 0; x < xNum; ++x, ++index) {
            _cellList.at(index) = { XMFLOAT2{static_cast<float>(x) / xNum, static_cast<float>(y) / yNum}, XMFLOAT2{1.0f / xNum, 1.0f / yNum}, size };   // �Z�������쐬.
        }
    }

    return true;
}

bool SpriteResource::Register(SpriteHandle& spriteHandrer)
{
    // ���ɓo�^����Ă��镨�̏ꍇ�A��U�o�^����������.
    if (spriteHandrer._hasIndex) {
        spriteHandrer.Release();
    }

    size_t tempIndex = -1;
    if (_freeSpriteIDList.empty()) {                  	// ���g�p�C���f�b�N�X���X�g����̏ꍇ.
        tempIndex = _spriteList.size();
        _spriteList.emplace_back(&_animationList);	    // ���X�g�ɓo�^.
    }
    else {						            			// ���g�p�C���f�b�N�X���X�g������ꍇ.
        tempIndex = _freeSpriteIDList.back();	        // ���g�p�̃C���f�b�N�X�����pop.
        _freeSpriteIDList.pop_back();
        Sprite initSpr(&_animationList);
        _spriteList.at(tempIndex) = initSpr;            // �ΏۃX�v���C�g��������.
    }

    spriteHandrer._hasIndex = true;     // �C���f�b�N�X�ۗL��Ԃ�true��.
    spriteHandrer._index = tempIndex;   // �C���f�b�N�X��ێ�.
    spriteHandrer._pRegisteredResource = this;       // ���g��e�ɓo�^.
    return true;
}

bool SpriteResource::Release(SpriteHandle& hSprite) {
    if (hSprite._pRegisteredResource != this || !hSprite._hasIndex) {    // �o�^�����g�łȂ��I�u�W�F�N�g�̏ꍇ�ƁA�X�v���C�g�̃C���f�b�N�X��ۗL���Ă��Ȃ��ꍇ.
        return false;
    }

    _spriteList.at(hSprite._index).isUsed = false;      // ���݂͗��p���Ă��Ȃ��ϐ������A�ꉞisUsed��false��.
    _spriteList.at(hSprite._index).onActive = false;    // ��A�N�e�B�u��.
    _freeSpriteIDList.emplace_back(hSprite._index);     // ���g�p�X�v���C�g�Ƃ��ăC���f�b�N�X��o�^.
    hSprite._hasIndex = false;                          // �C���f�b�N�X�𖢏��L��Ԃ�.
    return true;
}


// SpriteHandle

Sprite* SpriteHandle::pGetSprite()
{
    assert(_pRegisteredResource != nullptr);
    if (_pRegisteredResource != nullptr) {           // �e�����o�^��Ԃ̏ꍇ.
        if (!_hasIndex) {               // �C���f�b�N�X��ۗL���Ă��Ȃ��ꍇ.
            _pRegisteredResource->Register(*this);   // �C���f�b�N�X���擾���ɍs��.
        }
        return &_pRegisteredResource->pGetSpriteList()->at(_index);  // �|�C���^��Ԃ�.
    }
    return nullptr;
}

void SpriteHandle::Release()
{
    if (_pRegisteredResource == nullptr || !_hasIndex) { // nullptr�`�F�b�N�ƁA�ꉞRelease�Ń`�F�b�N�����͂������C���f�b�N�X��ۗL���Ă��Ȃ��ꍇ�������Ŏ~�߂�.
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
    size_t allNum = xNum * yNum;    // �S�v�f�����擾.
    _cellList.resize(allNum);       // �v�f����resize.
    assert(_cellList.size() >= allNum);
    if (_cellList.size() < allNum) { // �ꉞ�z��̗v�f�����s�����Ă�����G���[.
        return false;
    }

    // �Z�����X�g�𐶐�.
    XMFLOAT2 size = { static_cast<float>(xSize), static_cast<float>(ySize) };   // �Z���̍\�����.
    size_t index = 0;                                                           // �v�f�ԍ�.
    for (int y = 0; y < yNum; ++y) {                                            // �񎟌��\���ɍ��킹����d���[�v.
        for (int x = 0; x < xNum; ++x, ++index) {
            _cellList.at(index) = { XMFLOAT2{static_cast<float>(x) / xNum, static_cast<float>(y) / yNum}, XMFLOAT2{1.0f / xNum, 1.0f / yNum}, size };   // �Z�������쐬.
        }
    }

    return true;
}