#pragma once

#include <DirectXMath.h>
#include <cstdint>

struct GpuSkinnedVertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 textureCoordinate;
    std::uint32_t boneIDs[4];
    DirectX::XMFLOAT4 weights;
};

static_assert(sizeof(GpuSkinnedVertex) == 52, "Unexpected skinned vertex layout");