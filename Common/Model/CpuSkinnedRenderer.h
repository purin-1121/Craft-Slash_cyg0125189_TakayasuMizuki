#pragma once

#include <memory>
#include <vector>

#include <d3d11.h>
#include <wrl/client.h>

#include <SimpleMath.h>

#include "ModelResource.h"
#include "Animator.h"

// Skinned model renderer using GPU bone transforms.
class CpuSkinnedRenderer
{
public:
    // 頂点シェーダーなどの初期化
    bool Initialize(ID3D11Device* device);

    // ModelResourceからメッシュとインデックスを登録する
    bool CreateFromResource(
        ID3D11Device* device,
        const ModelResource& resource);

    // Update the per-character bone matrices used by the vertex shader.
    bool UpdateBoneMatrices(
        ID3D11DeviceContext* context,
        const Animator& animator,
        const ModelResource& resource);

    // 更新済み頂点を描画する
    void Draw(
        ID3D11DeviceContext* context,
        const DirectX::SimpleMath::Matrix& world,
        const DirectX::SimpleMath::Matrix& view,
        const DirectX::SimpleMath::Matrix& projection,
        float whiteAmount, float time);

private:
    struct SharedMeshGpuData
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        UINT indexCount = 0;
        UINT vertexCount = 0;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> diffuseTexture;
    };

    struct SharedModelGpuData
    {
        std::vector<SharedMeshGpuData> meshes;
    };

    struct MeshGpuData
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> boneBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> boneMatrices;
        std::vector<aiMatrix4x4> cpuBoneMatrices;
    };

    static std::shared_ptr<SharedModelGpuData> GetOrCreateSharedGpuResources(
        ID3D11Device* device,
        const ModelResource& resource);

    std::shared_ptr<SharedModelGpuData> m_sharedModelGpuData;
    std::vector<MeshGpuData> m_meshes;
};