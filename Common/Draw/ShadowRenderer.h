#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

struct GameContext;

// 入力レイアウトからも参照する、影用の頂点
struct ShadowVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 uv;
};

class ShadowRenderer
{
public:
    static ShadowRenderer* GetInstance()
    {
        if (!s_shadowRenderer)
        {
            s_shadowRenderer.reset(new ShadowRenderer);
        }

        return s_shadowRenderer.get();
    }

    static void Dispose()
    {
        s_shadowRenderer.reset();
    }

    bool Initialize(ID3D11Device* device);

    void Draw(
        const GameContext& gameContext,
        const DirectX::SimpleMath::Matrix& worldViewProj,
        float opacity);

    ~ShadowRenderer() = default;

    ShadowRenderer(const ShadowRenderer&) = delete;
    ShadowRenderer& operator=(const ShadowRenderer&) = delete;

private:
    ShadowRenderer() = default;

    static std::unique_ptr<ShadowRenderer> s_shadowRenderer;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
};