#include "pch.h"
#include "ShaderManager.h"
#include "ShadowRenderer.h"
#include "Common/Model/GpuSkinnedVertex.h"
#include <cstddef>
#include <WICTextureLoader.h>
#include <Common/ReadData.h>

std::unique_ptr<ShaderManager> ShaderManager::s_shaderManager = nullptr;

void ShaderManager::ApplySkinned(
    ID3D11DeviceContext* context,
    const SimpleMath::Matrix& worldViewProj,
    float whiteAmount,float time)
{
    TransformConstantsBuffer transform{};
    XMStoreFloat4x4(&transform.worldViewProj, worldViewProj);

    HitEffectConstantsBuffer hit{};
    hit.whiteAmount = whiteAmount;
    hit.time = time;

    //  バッファ内のデータを更新
    context->UpdateSubresource(
        m_transformBuffer.Get(), 0, nullptr, &transform, 0, 0);
    context->UpdateSubresource(
        m_hitEffectBuffer.Get(), 0, nullptr, &hit, 0, 0);

    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_hitEffectVS.Get(), nullptr, 0);
    context->PSSetShader(m_hitEffectPS.Get(), nullptr, 0);

    // ---- シェーダーにどのバッファのデータを使うか設定 ---- //
    ID3D11Buffer* transformBuffer = m_transformBuffer.Get();
    context->VSSetConstantBuffers(0, 1, &transformBuffer);

    ID3D11Buffer* hitBuffer = m_hitEffectBuffer.Get();
    context->PSSetConstantBuffers(0, 1, &hitBuffer);

    ID3D11SamplerState* sampler = m_sampler.Get();
    context->PSSetSamplers(0, 1, &sampler);
}

void ShaderManager::ApplyShadow(
    ID3D11DeviceContext* context,
    const SimpleMath::Matrix& worldViewProj,
    float opacity)
{
    ShadowConstantsBuffer shadow{};
    XMStoreFloat4x4(&shadow.worldViewProj, worldViewProj);
    shadow.alpha = opacity;

    context->UpdateSubresource(
        m_shadowBuffer.Get(), 0, nullptr, &shadow, 0, 0);

    // キャラクター用ではなく、影用の入力レイアウト
    context->IASetInputLayout(m_shadowInputLayout.Get());

    context->VSSetShader(m_shadowVS.Get(), nullptr, 0);
    context->PSSetShader(m_shadowPS.Get(), nullptr, 0);

    ID3D11Buffer* buffer = m_shadowBuffer.Get();

    // Shadow.hlsliのregister(b1)に合わせる
    context->VSSetConstantBuffers(1, 1, &buffer);
    context->PSSetConstantBuffers(1, 1, &buffer);
}
bool ShaderManager::CreateResources(ID3D11Device* device)
{
    if (device == nullptr)
        return false;

    if (!CreateShader(device, L"ShadowPS.cso", L"ShadowVS.cso", m_shadowPS, m_shadowVS))
        return false;
    if (!CreateShader(device, L"HitEffectPSShader.cso", L"HitEffectVSShader.cso", m_hitEffectPS, m_hitEffectVS))
        return false;

    if (!CreateInputLayout(device))
        return false;

    if(!CreateConstantBuffers(device))
        return false;

    if (!CreateSamplerState(device))
        return false;

    return true;
}

bool ShaderManager::CreateShader(ID3D11Device* device, std::wstring psName,std::wstring vsName ,
    Microsoft::WRL::ComPtr<ID3D11PixelShader>& ps, Microsoft::WRL::ComPtr<ID3D11VertexShader>& vs)
{
        // 頂点シェーダーを読み込んで作成
    auto vsBlob = DX::ReadData(vsName.c_str());

    HRESULT hr = device->CreateVertexShader(
        vsBlob.data(),
        vsBlob.size(),
        nullptr,
        vs.ReleaseAndGetAddressOf());

    if (FAILED(hr))
        return false;

        // ピクセルシェーダーを読み込んで作成
    auto psBlob = DX::ReadData(psName.c_str());

    hr = device->CreatePixelShader(
        psBlob.data(),
        psBlob.size(),
        nullptr,
        ps.ReleaseAndGetAddressOf());

    if (FAILED(hr))
        return false;


    return true;
}

bool ShaderManager::CreateInputLayout(ID3D11Device* device)
{
	// 頂点データと頂点シェーダーを対応付ける
	// offsetof : 指定のメンバ変数までのバイト数を取得する
	const D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "SV_Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		  static_cast<UINT>(offsetof(GpuSkinnedVertex, position)),
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		  static_cast<UINT>(offsetof(GpuSkinnedVertex, textureCoordinate)),
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,
		  static_cast<UINT>(offsetof(GpuSkinnedVertex, boneIDs)),
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		  static_cast<UINT>(offsetof(GpuSkinnedVertex, weights)),
		  D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	auto vsBlob = DX::ReadData(L"HitEffectVSShader.cso");
	HRESULT hr = device->CreateInputLayout(
		inputElements,
		ARRAYSIZE(inputElements),
		vsBlob.data(),
		vsBlob.size(),
		m_inputLayout.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		return false;

    // 頂点データと頂点シェーダーを対応付ける
	// offsetof : 指定のメンバ変数までのバイト数を取得する
	const D3D11_INPUT_ELEMENT_DESC shadowInputElements[] =
    {
        {
            "SV_Position", 0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            static_cast<UINT>(offsetof(ShadowVertex, position)),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
        {
            "TEXCOORD", 0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            static_cast<UINT>(offsetof(ShadowVertex, uv)),
            D3D11_INPUT_PER_VERTEX_DATA, 0
        },
    };
	vsBlob = DX::ReadData(L"ShadowVS.cso");
	hr = device->CreateInputLayout(
		shadowInputElements,
		ARRAYSIZE(shadowInputElements),
		vsBlob.data(),
		vsBlob.size(),
		m_shadowInputLayout.ReleaseAndGetAddressOf());
	if (FAILED(hr))
		return false;

    return true;

}   

bool ShaderManager::CreateSamplerState(ID3D11Device* device)
{
    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = device->CreateSamplerState(
        &samplerDesc,
        m_sampler.ReleaseAndGetAddressOf());
    if (FAILED(hr))
        return false;

    return true;

}

bool ShaderManager::CreateConstantBuffers(ID3D11Device* device)
{
    // 定数バッファを作成
    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    // ---- ヒットエフェクト用シェーダー ---- //
    {
        // 頂点シェーダー用：ワールド・ビュー・射影行列
        bd.ByteWidth = sizeof(TransformConstantsBuffer);
        HRESULT hr = device->CreateBuffer(
            &bd,
            nullptr,
            m_transformBuffer.ReleaseAndGetAddressOf());

        if (FAILED(hr))
            return false;

        // ピクセルシェーダー用：白さ
        bd.ByteWidth = sizeof(HitEffectConstantsBuffer);
        hr = device->CreateBuffer(
            &bd,
            nullptr,
            m_hitEffectBuffer.ReleaseAndGetAddressOf());

        if (FAILED(hr))
            return false;
    }

	// ---- 影用シェーダー ---- //
	{
		// 頂点シェーダー用：ワールド・ビュー・射影行列
		bd.ByteWidth = sizeof(ShadowConstantsBuffer);
		HRESULT hr = device->CreateBuffer(
			&bd,
			nullptr,
			m_shadowBuffer.ReleaseAndGetAddressOf());
		if (FAILED(hr))
			return false;
	}

    return true;
}
