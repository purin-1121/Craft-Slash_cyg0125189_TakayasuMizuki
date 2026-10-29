#pragma once

struct TransformConstantsBuffer
{
	DirectX::XMFLOAT4X4 worldViewProj;  // ワールドビュー射影行列
};

struct HitEffectConstantsBuffer
{
    float whiteAmount;
    float time;
    float padding[2];
};

struct ShadowConstantsBuffer
{
	DirectX::XMFLOAT4X4 worldViewProj;     // 位置行列
	float alpha;               // 透明度
	float padding[3];              // パディング
};

class ShaderManager
{
public:

    // キャラクターの透明度設定
    void ApplySkinned(ID3D11DeviceContext* context,
        const SimpleMath::Matrix& worldViewProj,
        float whiteAmount, float time);

    // キャラクターの影設定
    void ApplyShadow(ID3D11DeviceContext* context,
        const SimpleMath::Matrix& worldViewProj,
        float heightAboveGround);

    ~ShaderManager(){};

    // リソースの作成
    bool CreateResources(ID3D11Device* device);

    static ShaderManager* GetInstance()
    {
        CreateInstance();
        return s_shaderManager.get();
    }

    static ShaderManager& GetRefarence()
    {
        CreateInstance();
        return *s_shaderManager.get();
    }

    static void Dispose()
    {
        s_shaderManager.reset();
    }

    static void CreateInstance()
    {
        if (s_shaderManager == nullptr)
        {
            s_shaderManager.reset(new ShaderManager);
        }
    }

private:

	// ---- ヒットエフェクト用シェーダー ---- //
    // ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_hitEffectPS;
    // 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_hitEffectVS;

	// ---- シャドウ用シェーダー ---- //
	// ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shadowPS;
	// 頂点シェーダー
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shadowVS; 

    // 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_shadowInputLayout;
    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_transformBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_hitEffectBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_shadowBuffer;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler;


    static std::unique_ptr<ShaderManager> s_shaderManager;
    ShaderManager() = default;

    // ---- シェーダー作成 ---- //
	bool CreateShader(ID3D11Device* device, 
        std::wstring psName,std::wstring vsName, 
        Microsoft::WRL::ComPtr<ID3D11PixelShader>& ps, 
        Microsoft::WRL::ComPtr<ID3D11VertexShader>& vs);
    // 入力レイアウト作成
	bool CreateInputLayout(ID3D11Device* device);
    // サンプラー作成
	bool CreateSamplerState(ID3D11Device* device);
    // 定数バッファ作成
	bool CreateConstantBuffers(ID3D11Device* device);

};

