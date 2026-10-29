#include "pch.h"
#include "ShadowRenderer.h"
#include "ShaderManager.h"
#include "Tools/GameContext.h"

#include <algorithm>
#include <cstdint>

std::unique_ptr<ShadowRenderer>
    ShadowRenderer::s_shadowRenderer = nullptr;

bool ShadowRenderer::Initialize(ID3D11Device* device)
{
    if (!device)
        return false;

    // 既に作成済みなら再作成しない
    if (m_vertexBuffer && m_indexBuffer)
        return true;

    // 原点を中心としたXZ平面の板
    const ShadowVertex vertices[] =
    {
        { {-1.0f, 0.0f, -1.0f}, {0.0f, 0.0f} },
        { { 1.0f, 0.0f, -1.0f}, {1.0f, 0.0f} },
        { { 1.0f, 0.0f,  1.0f}, {1.0f, 1.0f} },
        { {-1.0f, 0.0f,  1.0f}, {0.0f, 1.0f} },
    };

    // 四角形を三角形2枚で構成
    const uint16_t indices[] =
    {
        0, 1, 2,
        0, 2, 3,
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

    // 頂点バッファ
    D3D11_BUFFER_DESC vertexDesc{};
    vertexDesc.ByteWidth = static_cast<UINT>(sizeof(vertices));
    vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;   // 読み取り専用
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // 頂点バッファとして使用

    D3D11_SUBRESOURCE_DATA vertexData{};
    vertexData.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(
        &vertexDesc,
        &vertexData,
        vertexBuffer.GetAddressOf());

    if (FAILED(hr))
        return false;

    // インデックスバッファ
    D3D11_BUFFER_DESC indexDesc{};
    indexDesc.ByteWidth = static_cast<UINT>(sizeof(indices));
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;    // 読み取り専用
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;  // インデックスバッファとして使用

    D3D11_SUBRESOURCE_DATA indexData{};
    indexData.pSysMem = indices;

    hr = device->CreateBuffer(
        &indexDesc,
        &indexData,
        indexBuffer.GetAddressOf());

    if (FAILED(hr))
        return false;

    m_vertexBuffer = vertexBuffer;
    m_indexBuffer = indexBuffer;

    return true;
}

void ShadowRenderer::Draw(
    const GameContext& gameContext,
    const DirectX::SimpleMath::Matrix& worldViewProj,
    float opacity)
{
    if (!m_vertexBuffer || !m_indexBuffer)
        return;

    auto* context = gameContext.deviceResources.GetD3DDeviceContext();

    auto& states = gameContext.commonStates;

    // 影を描く間だけ変更する描画設定を保存
    Microsoft::WRL::ComPtr<ID3D11BlendState> oldBlend;      // 合成方法 
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> oldDepth;   // 奥行の判定
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> oldRasterizer;//裏面の描画設定

    FLOAT oldBlendFactor[4]{};
    UINT oldSampleMask = 0;
    UINT oldStencilRef = 0;

    context->OMGetBlendState(
        oldBlend.GetAddressOf(),
        oldBlendFactor,
        &oldSampleMask);

    context->OMGetDepthStencilState(
        oldDepth.GetAddressOf(),
        &oldStencilRef);

    context->RSGetState(oldRasterizer.GetAddressOf());

    // 半透明合成・深度テストあり・深度書き込みなし
    context->OMSetBlendState(
        states.NonPremultiplied(), nullptr, 0xFFFFFFFF);

    context->OMSetDepthStencilState(states.DepthRead(), 0);

    // 板の表裏で消えないようにする
    context->RSSetState(states.CullNone());

    // 影シェーダーを適用
    ShaderManager::GetInstance()->ApplyShadow(
        context,
        worldViewProj,
        std::clamp(opacity, 0.0f, 1.0f));


    ID3D11Buffer* vertexBuffer = m_vertexBuffer.Get();
    const UINT stride = sizeof(ShadowVertex);
    const UINT offset = 0;

    context->IASetVertexBuffers(
        0, 1, &vertexBuffer, &stride, &offset);

    context->IASetIndexBuffer(
        m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

    context->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    context->DrawIndexed(6, 0, 0);

    // 変更した描画設定を元に戻す
    context->OMSetBlendState(
        oldBlend.Get(), oldBlendFactor, oldSampleMask);

    context->OMSetDepthStencilState(
        oldDepth.Get(), oldStencilRef);

    context->RSSetState(oldRasterizer.Get());
}
