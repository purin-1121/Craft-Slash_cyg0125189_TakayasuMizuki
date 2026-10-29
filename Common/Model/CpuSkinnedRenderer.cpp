#include "pch.h"
#include "CpuSkinnedRenderer.h"
#include "GpuSkinnedVertex.h"
#include "Common/Draw/ShaderManager.h"
#include <WICTextureLoader.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <unordered_map>
#include <Common/ReadData.h>

namespace
{
    DirectX::XMFLOAT4X4 ToRowVectorMatrix(const aiMatrix4x4& matrix)
    {
        // Assimp uses column-vector transforms. Transpose for HLSL row-vector mul.
        return DirectX::XMFLOAT4X4(
            matrix.a1, matrix.b1, matrix.c1, matrix.d1,
            matrix.a2, matrix.b2, matrix.c2, matrix.d2,
            matrix.a3, matrix.b3, matrix.c3, matrix.d3,
            matrix.a4, matrix.b4, matrix.c4, matrix.d4);
    }
}

bool CpuSkinnedRenderer::Initialize(ID3D11Device* device)
{
    return device != nullptr;
}

std::shared_ptr<CpuSkinnedRenderer::SharedModelGpuData>
CpuSkinnedRenderer::GetOrCreateSharedGpuResources(
    ID3D11Device* device,
    const ModelResource& resource)
{
    struct CacheEntry
    {
        ID3D11Device* device = nullptr;
        std::weak_ptr<SharedModelGpuData> resources;
    };

    static std::unordered_map<const ModelResource*, CacheEntry> cache;
    static std::mutex cacheMutex;
    std::lock_guard<std::mutex> lock(cacheMutex);

    const auto cached = cache.find(&resource);
    if (cached != cache.end() && cached->second.device == device)
    {
        if (auto resources = cached->second.resources.lock())
            return resources;
    }

    auto resources = std::make_shared<SharedModelGpuData>();
    const auto& skinMeshes = resource.GetSkinMeshes();
    const aiScene* scene = resource.GetScene();
    if (scene == nullptr || skinMeshes.empty())
        return nullptr;

    for (const SkinMesh& skinMesh : skinMeshes)
    {
        if (skinMesh.vertices.empty() || skinMesh.indices.empty())
            continue;

        if (skinMesh.vertices.size() > UINT_MAX / sizeof(GpuSkinnedVertex) ||
            skinMesh.indices.size() > UINT_MAX / sizeof(unsigned int))
        {
            return nullptr;
        }

        std::vector<GpuSkinnedVertex> vertices;
        vertices.reserve(skinMesh.vertices.size());
        for (const SkinVertex& source : skinMesh.vertices)
        {
            GpuSkinnedVertex vertex{};
            vertex.position = DirectX::XMFLOAT3(
                source.position.x, source.position.y, source.position.z);
            vertex.textureCoordinate = DirectX::XMFLOAT2(
                source.textureCoordinate.x, source.textureCoordinate.y);
            for (unsigned int i = 0; i < MAX_BONE_INFLUENCE; ++i)
                vertex.boneIDs[i] = source.boneIDs[i];
            vertex.weights = DirectX::XMFLOAT4(
                source.weights[0], source.weights[1],
                source.weights[2], source.weights[3]);
            vertices.push_back(vertex);
        }

        SharedMeshGpuData sharedMesh;
        sharedMesh.vertexCount = static_cast<UINT>(vertices.size());

        D3D11_BUFFER_DESC vertexDesc{};
        vertexDesc.ByteWidth = static_cast<UINT>(
            vertices.size() * sizeof(GpuSkinnedVertex));
        vertexDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexData{};
        vertexData.pSysMem = vertices.data();
        HRESULT hr = device->CreateBuffer(
            &vertexDesc,
            &vertexData,
            sharedMesh.vertexBuffer.ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            OutputDebugStringA("CpuSkinnedRenderer: CreateBuffer(vertex) failed.\n");
            return nullptr;
        }

        if (skinMesh.materialIndex >= scene->mNumMaterials)
            return nullptr;

        aiString texturePath;
        const aiMaterial* material = scene->mMaterials[skinMesh.materialIndex];
        if (material == nullptr ||
            material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) != AI_SUCCESS)
        {
            OutputDebugStringA("CpuSkinnedRenderer: diffuse texture not found.\n");
            return nullptr;
        }

        const aiTexture* embeddedTexture =
            scene->GetEmbeddedTexture(texturePath.C_Str());
        if (embeddedTexture == nullptr || embeddedTexture->mHeight != 0)
        {
            OutputDebugStringA("CpuSkinnedRenderer: unsupported embedded diffuse texture.\n");
            return nullptr;
        }

        const HRESULT textureHr = DirectX::CreateWICTextureFromMemory(
            device,
            reinterpret_cast<const uint8_t*>(embeddedTexture->pcData),
            embeddedTexture->mWidth,
            nullptr,
            sharedMesh.diffuseTexture.GetAddressOf());
        if (FAILED(textureHr))
        {
            OutputDebugStringA("CpuSkinnedRenderer: diffuse texture load failed.\n");
            return nullptr;
        }

        D3D11_BUFFER_DESC indexDesc{};
        indexDesc.ByteWidth = static_cast<UINT>(
            skinMesh.indices.size() * sizeof(unsigned int));
        indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexData{};
        indexData.pSysMem = skinMesh.indices.data();

        const HRESULT indexHr = device->CreateBuffer(
            &indexDesc,
            &indexData,
            sharedMesh.indexBuffer.ReleaseAndGetAddressOf());
        if (FAILED(indexHr))
        {
            OutputDebugStringA("CpuSkinnedRenderer: CreateBuffer(index) failed.\n");
            return nullptr;
        }

        sharedMesh.indexCount = static_cast<UINT>(skinMesh.indices.size());
        resources->meshes.push_back(std::move(sharedMesh));
    }

    if (resources->meshes.empty())
        return nullptr;

    CacheEntry entry;
    entry.device = device;
    entry.resources = resources;
    cache[&resource] = entry;
    return resources;
}

bool CpuSkinnedRenderer::CreateFromResource(
    ID3D11Device* device,
    const ModelResource& resource)
{
    if (device == nullptr)
        return false;

    m_meshes.clear();
    m_sharedModelGpuData.reset();

    auto sharedResources = GetOrCreateSharedGpuResources(device, resource);
    if (sharedResources == nullptr)
        return false;

    const auto& skinMeshes = resource.GetSkinMeshes();
    for (const SkinMesh& skinMesh : skinMeshes)
    {
        if (skinMesh.vertices.empty() || skinMesh.indices.empty())
            continue;

        const size_t elementCount = (std::max)(size_t(1), skinMesh.bones.size());
        if (elementCount > UINT_MAX / sizeof(DirectX::XMFLOAT4X4))
            return false;

        MeshGpuData gpuMesh;
        D3D11_BUFFER_DESC boneDesc{};
        boneDesc.ByteWidth = static_cast<UINT>(
            elementCount * sizeof(DirectX::XMFLOAT4X4));
        boneDesc.Usage = D3D11_USAGE_DYNAMIC;
        boneDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        boneDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        boneDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        boneDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);

        HRESULT hr = device->CreateBuffer(
            &boneDesc,
            nullptr,
            gpuMesh.boneBuffer.ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            OutputDebugStringA("CpuSkinnedRenderer: CreateBuffer(bones) failed.\n");
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = DXGI_FORMAT_UNKNOWN;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        srvDesc.Buffer.FirstElement = 0;
        srvDesc.Buffer.NumElements = static_cast<UINT>(elementCount);

        hr = device->CreateShaderResourceView(
            gpuMesh.boneBuffer.Get(),
            &srvDesc,
            gpuMesh.boneMatrices.ReleaseAndGetAddressOf());
        if (FAILED(hr))
        {
            OutputDebugStringA("CpuSkinnedRenderer: CreateShaderResourceView(bones) failed.\n");
            return false;
        }

        m_meshes.push_back(std::move(gpuMesh));
    }

    if (m_meshes.empty() || m_meshes.size() != sharedResources->meshes.size())
        return false;

    m_sharedModelGpuData = std::move(sharedResources);
    return true;
}

bool CpuSkinnedRenderer::UpdateBoneMatrices(
    ID3D11DeviceContext* context,
    const Animator& animator,
    const ModelResource& resource)
{
    if (context == nullptr || m_sharedModelGpuData == nullptr ||
        m_meshes.size() != m_sharedModelGpuData->meshes.size())
    {
        return false;
    }

    using Clock = std::chrono::steady_clock;
    const auto updateStart = Clock::now();
    double matrixMs = 0.0;
    double uploadMs = 0.0;
    size_t gpuIndex = 0;

    for (const SkinMesh& skinMesh : resource.GetSkinMeshes())
    {
        if (skinMesh.vertices.empty() || skinMesh.indices.empty())
            continue;
        if (gpuIndex >= m_meshes.size())
            return false;

        MeshGpuData& gpuMesh = m_meshes[gpuIndex++];
        const auto matrixStart = Clock::now();
        animator.GetSkinningMatrices(skinMesh, gpuMesh.cpuBoneMatrices);
        matrixMs += std::chrono::duration<double, std::milli>(
            Clock::now() - matrixStart).count();

        const auto uploadStart = Clock::now();
        D3D11_MAPPED_SUBRESOURCE mapped{};
        const HRESULT hr = context->Map(
            gpuMesh.boneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD,
            0, &mapped);
        if (FAILED(hr))
            return false;

        auto* destination = static_cast<DirectX::XMFLOAT4X4*>(mapped.pData);
        if (gpuMesh.cpuBoneMatrices.empty())
        {
            destination[0] = DirectX::XMFLOAT4X4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f);
        }
        else
        {
            for (size_t i = 0; i < gpuMesh.cpuBoneMatrices.size(); ++i)
                destination[i] = ToRowVectorMatrix(gpuMesh.cpuBoneMatrices[i]);
        }
        context->Unmap(gpuMesh.boneBuffer.Get(), 0);
        uploadMs += std::chrono::duration<double, std::milli>(
            Clock::now() - uploadStart).count();
    }

    static auto lastLogTime = Clock::now();
    static double totalMatrixMs = 0.0;
    static double totalUploadMs = 0.0;
    static double totalUpdateMs = 0.0;
    static int callCount = 0;

    const double updateMs = std::chrono::duration<double, std::milli>(
        Clock::now() - updateStart).count();
    totalMatrixMs += matrixMs;
    totalUploadMs += uploadMs;
    totalUpdateMs += updateMs;
    ++callCount;

    const auto now = Clock::now();
    if (now - lastLogTime >= std::chrono::seconds(1))
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3)
            << "[Skinning Profiling]"
            << "\n  Bone matrices: " << totalMatrixMs / callCount << " ms/call"
            << "\n  Upload       : " << totalUploadMs / callCount << " ms/call"
            << "\n  Total        : " << totalUpdateMs / callCount << " ms/call"
            << "\n  Calls/sec    : " << callCount << "\n";
        OutputDebugStringA(oss.str().c_str());

        totalMatrixMs = 0.0;
        totalUploadMs = 0.0;
        totalUpdateMs = 0.0;
        callCount = 0;
        lastLogTime = now;
    }

    return gpuIndex == m_meshes.size();
}

void CpuSkinnedRenderer::Draw(
    ID3D11DeviceContext* context,
    const DirectX::SimpleMath::Matrix& world,
    const DirectX::SimpleMath::Matrix& view,
    const DirectX::SimpleMath::Matrix& projection,
    float whiteAmount, float time)
{
    if (context == nullptr || m_sharedModelGpuData == nullptr ||
        m_sharedModelGpuData->meshes.size() != m_meshes.size())
    {
        return;
    }

    // スケールを小さく
    const SimpleMath::Matrix scaleOffset =
        SimpleMath::Matrix::CreateScale(0.0075f);
    // 描画用ワールド行列を計算
    const SimpleMath::Matrix drawWorld = scaleOffset * world;
    // ワールドビュー射影行列を計算
    const SimpleMath::Matrix worldViewProj =
        drawWorld * view * projection;

	// シェーダーを適用
    ShaderManager::GetInstance()->ApplySkinned(
        context, worldViewProj, whiteAmount, time);

    for (size_t i = 0; i < m_meshes.size(); ++i)
    {
        const MeshGpuData& mesh = m_meshes[i];
        const SharedMeshGpuData& sharedMesh = m_sharedModelGpuData->meshes[i];

        ID3D11ShaderResourceView* textureView =
            sharedMesh.diffuseTexture.Get();
        context->PSSetShaderResources(0, 1, &textureView);

        ID3D11ShaderResourceView* boneView = mesh.boneMatrices.Get();
        context->VSSetShaderResources(1, 1, &boneView);

        ID3D11Buffer* vertexBuffer = sharedMesh.vertexBuffer.Get();
        const UINT stride = sizeof(GpuSkinnedVertex);
        const UINT offset = 0;
        context->IASetVertexBuffers(
            0, 1, &vertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(
            sharedMesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(
            D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(sharedMesh.indexCount, 0, 0);
    }

    ID3D11ShaderResourceView* nullView = nullptr;
    context->VSSetShaderResources(1, 1, &nullView);
}