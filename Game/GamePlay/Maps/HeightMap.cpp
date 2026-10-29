#include "pch.h"
#include "HeightMap.h"
#include <iomanip>
#include <limits>
#include "Common/Collider/RayCast.h"
#include "Common/Model/ObjectManager.h"
#include "Game/GamePlay/Factory/DropFactory.h"

namespace
{
    // 頂点の色を計算する関数
    XMFLOAT4 CalcColor(float t, float normalY)
    {
        using SimpleMath::Color;
        // 使用する色
        const Color Dirt(0.45f, 0.30f, 0.15f, 1.0f);
        const Color Grass(0.25f, 0.55f, 0.20f, 1.0f);
        const Color Rock(0.50f, 0.48f, 0.45f, 1.0f);
        const Color Snow(0.95f, 0.95f, 0.98f, 1.0f);

        // 色の最高値と最低値
        const float DirtHeight = -5.0f;
        const float SnowHeight = 5.0f;

        // 色を変える割合
        Color c;
        const float DirtLine = 0.0f;
        const float GrassLine = 0.3f;
        const float RockLine = 0.7f;
        const float SnowLine = 1.0f;

        // 頂点の高さが最低値から最高値のどこにあるか計算し、０～１にクランプ
        t = (t + std::abs(DirtHeight)) / (SnowHeight + std::abs(DirtHeight));
        t = std::clamp(t, 0.0f, 1.0f);  

        if (t < GrassLine)      c = Color::Lerp(Dirt, Grass, t / GrassLine);
        else if (t < RockLine)  c = Color::Lerp(Grass, Rock, (t - GrassLine) / RockLine - GrassLine);
        else                    c = Color::Lerp(Rock, Snow, (t - SnowLine) / SnowLine - RockLine);

        // 頂点間の傾斜によって岩の色素を混ぜる
        // 混ぜ始める角度
        const float RockSlope = 0.25f;
        float slope = 1.0f - normalY;
        float rockMix = std::clamp((slope - RockSlope) / RockSlope, 0.0f, 1.0f);
        c = Color::Lerp(c, Rock, rockMix);

        return XMFLOAT4(c.x, c.y, c.z, c.w);
    }
}

HeightMap::HeightMap(float size)
    : m_width{ 0 }
    , m_depth{ 0 }
    , m_gridSize{ size }
    , m_origin{}
    , m_chunkCountX{}
    , m_chunkCountZ{}
    , m_chunkOffsetX{}
    , m_chunkOffsetZ{}
{

}

HeightMap::~HeightMap()
{
}

void HeightMap::Initialize(const GameContext& gameContext)
{
    LoadMap("Resources/CSV/Map.csv");
    LoadModel("Resources/Jsons/ModelObjectsData.json");
    auto device = gameContext.deviceResources.GetD3DDevice();

    // ---- エフェクトの作成・設定 ---- //

    m_effect = std::make_unique<DirectX::BasicEffect>(device);
    m_effect->SetVertexColorEnabled(true);  // 頂点カラーを有効化
    m_effect->EnableDefaultLighting();      // デフォルトライトを適応
    
    // 入力レイアウトの作成
    DirectX::CreateInputLayoutFromEffect<DirectX::VertexPositionNormalColor>(
        device, m_effect.get(), m_layout.ReleaseAndGetAddressOf());

    // 頂点メッシュの作成
    BuildMesh(device);
    // モデルの初期化
    for (auto& model : m_models)
    {
        model->Initialize(gameContext);
    }
    // チャンクの初期化
    InitializeChunks();
    // チャンクキャッシュの更新
    UpdateChunkCaches();
}

void HeightMap::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
    auto device = gameContext.deviceResources.GetD3DDevice();
    auto context = gameContext.deviceResources.GetD3DDeviceContext();

    // 変更済み＆頂点バッファがあれば
    if (m_dirty && m_vb)
    {
        // 
        UpdateRegion(context, m_dirtyXMin, m_dirtyXMax, m_dirtyZMin, m_dirtyZMax);
        m_dirty = false;
    }

    // 
    if (m_meshDirty)
    {
        m_ib.Reset();
        BuildMesh(device);
        m_meshDirty = false;
        m_dirty = false;
    }

    if (m_indexCount == 0) return;

    // ワールド、ビュー、射影行列を作成・適応
    m_effect->SetWorld(SimpleMath::Matrix::Identity);
    m_effect->SetView(view);
    m_effect->SetProjection(Draw::GetInstance().GetRenderingContext().proj);
    m_effect->Apply(context);

    // 入力レイアウトを作成
    context->IASetInputLayout(m_layout.Get());
    UINT stride = sizeof(DirectX::VertexPositionNormalColor);
    // ---- インデックス情報を設定 ---- //
    // 開始インデックスを設定
    //auto rendStart = m_playerPos - SimpleMath::Vector2{m_chunkOffsetX* CHUNK_SIZE, m_chunkOffsetZ* CHUNK_SIZE};
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, m_vb.GetAddressOf(), &stride, &offset);
    context->IASetIndexBuffer(m_ib.Get(), DXGI_FORMAT_R32_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 半透明を無効化
    context->OMSetBlendState(gameContext.commonStates.Opaque(), nullptr, 0xFFFFFFFF);
    // 通常の深度バッファを適応
    context->OMSetDepthStencilState(gameContext.commonStates.DepthDefault(), 0);
    // カリングを設定（反時計）
    context->RSSetState(gameContext.commonStates.CullCounterClockwise());

    // 頂点描画
    context->DrawIndexed(m_indexCount, 0, 0);

    // モデル描画
    for (auto& model : m_models)
    {
        model->Render(gameContext, view);
    }

}

void HeightMap::EditRender(
    const GameContext& gameContext,
    const SimpleMath::Vector3& playerPos,
    int radius,
    SimpleMath::Matrix view)
{
    // チャンクキャッシュ作成
    UpdateChunkCaches();

    const auto offset = GetCenterOffset();

    // グリッド座標に変換
    const int px = static_cast<int>(
        (playerPos.x + offset.x) / m_gridSize);
    const int pz = static_cast<int>(
        (playerPos.z + offset.y) / m_gridSize);

    // グリッドの四隅を計算
    const int xMin = std::max(px - radius, 0);
    const int xMax = std::min(px + radius, m_width - 1);
    const int zMin = std::max(pz - radius, 0);
    const int zMax = std::min(pz + radius, m_depth - 1);

    // チャンクの四隅を計算
    const int chunkMinX = xMin / CHUNK_SIZE;
    const int chunkMaxX = std::min(xMax / CHUNK_SIZE, m_chunkCountX - 1);

    const int chunkMinZ = zMin / CHUNK_SIZE;
    const int chunkMaxZ = std::min(zMax / CHUNK_SIZE, m_chunkCountZ - 1);

    // チャンクの描画
    for (int z = chunkMinZ; z <= chunkMaxZ; ++z)
    {
        for (int x = chunkMinX; x <= chunkMaxX; ++x)
        {
            // チャンクを取得
            const int index = z * m_chunkCountX + x;
            const auto& chunk = m_chunks[index];

            if (chunk.vertexCount == 0)
                continue;

            // チャンクの頂点を線分描画
            Draw::GetInstance().DrawLineList(chunk.vertices.data(), chunk.vertexCount);
        }
    }

    // モデル描画
    for (auto& model : m_models)
    {
        model->Render(gameContext, view);
    }
}
float HeightMap::GetHeight(int x, int z) const
{
    if (x >= 0 && x < m_width && z >= 0 && z < m_depth)
    {
        return m_heights[(z + m_origin.y) * m_width + x + m_origin.x];
    }

    return 0.0f;
}

float HeightMap::GetWorldHeight(float worldX, float worldZ) const
{
    // ワールド座標をマス座標に変換し、範囲外なら処理をしない
    auto [x, z] = GetGrid(worldX, worldZ);
    if (!IsInvid(static_cast<int>(x), static_cast<int>(z))) 
        return 0.0f;


    // マスの端からどれだけ進んだか（0 <= x < 1）
    auto offset = GetCenterOffset();
    float localX = ((worldX + offset.x) / m_gridSize) - x;
    float localZ = ((worldZ + offset.y) / m_gridSize) - z;

    // 一マス範囲を広げる
    DirectX::SimpleMath::Vector3 p0(0.0f, GetHeight(x, z), 0.0f);
    DirectX::SimpleMath::Vector3 p1(1.0f, GetHeight(x + 1, z), 0.0f);
    DirectX::SimpleMath::Vector3 p2(0.0f, GetHeight(x, z + 1), 1.0f);
    DirectX::SimpleMath::Vector3 p3(1.0f, GetHeight(x + 1, z + 1), 1.0f);

    // ===== 上半分の三角形 =====
    if (localX + localZ <= 1.0f)
        return GetTriangleHeight(p0,p1,p2,localX,localZ);

    // ===== 下半分の三角形 =====
    return GetTriangleHeight(p1,p3,p2,localX,localZ);
}

bool HeightMap::IsSlide(float worldX, float worldZ) const
{
    // ワールド座標をマス座標に変換し、範囲外なら処理をしない
    auto [x, z] = GetGrid(worldX, worldZ);
    if (!IsInvid(static_cast<int>(x), static_cast<int>(z))) 
        return false;

    auto offset = GetCenterOffset();

    // 指定位置の頂点の法線を求める
    DirectX::SimpleMath::Vector3 normal = GetNormal(worldX, worldZ);

    // 頂点の法線の角度を求める
    float slopeAngleDeg = XMConvertToDegrees(std::acos(
        MathLib::Clamp(normal.y, -1.0f, 1.0f)
    ));

    return slopeAngleDeg >= SLIDE_ANGLE;
}

// グリッド内にいるか
bool HeightMap::IsInvid(int x, int z) const
{
    if (x < 0 || x >= m_width - 1 || z < 0 || z >= m_depth - 1)
    {
        return false;
    }

    return true;
}

SimpleMath::Vector2 HeightMap::GetGrid(float worldX, float worldZ) const
{
    auto offset = GetCenterOffset();

    // 指定のワールド座標
    int x = static_cast<int>(std::floor((worldX + offset.x) / m_gridSize));
    int z = static_cast<int>(std::floor((worldZ + offset.y) / m_gridSize));

    return { static_cast<float>(x), static_cast<float>(z) };
}

std::vector<std::pair<int, int>> HeightMap::GetVerticesInQuad(SimpleMath::Vector3 p0, SimpleMath::Vector3 p1, SimpleMath::Vector3 p2, SimpleMath::Vector3 p3) const
{
    std::vector<std::pair<int, int>> result;
    auto offset = GetCenterOffset();

    // 送られた頂点の中から最大と最小の値を出す
    float minX = std::min({ p0.x, p1.x, p2.x, p3.x });
    float maxX = std::max({ p0.x, p1.x, p2.x, p3.x });
    float minZ = std::min({ p0.z, p1.z, p2.z, p3.z });
    float maxZ = std::max({ p0.z, p1.z, p2.z, p3.z });

    // 左上座標と右下座標を計算
    int startX = static_cast<int>(std::floor((minX + offset.x) / m_gridSize));
    int startZ = static_cast<int>(std::floor((minZ + offset.y) / m_gridSize));
    int endX = static_cast<int>(std::floor((maxX + offset.x) / m_gridSize));
    int endZ = static_cast<int>(std::floor((maxZ + offset.y) / m_gridSize));

    // 範囲外対策でクランプ
    startX = std::max(startX, 0);
    startZ = std::max(startZ, 0);
    endX = std::min(endX, m_width - 1);
    endZ = std::min(endZ, m_depth - 1);

    SimpleMath::Vector2 pts[4] = { {p0.x, p0.z}, {p1.x, p1.z}, {p2.x, p2.z}, {p3.x, p3.z} };


    for (int z = startZ; z <= endZ; ++z)
    {
        for (int x = startX; x <= endX; ++x)
        {
            // 対象の頂点を計算
            SimpleMath::Vector2 target(x * m_gridSize - offset.x, z * m_gridSize - offset.y);
            bool isInside = true;       // 中にあるか
            bool hasPositive = false;   // 
            bool hasNegative = false;   // 

            // 送られた４つの頂点との
            for (int i = 0; i < 4; ++i)
            {
                // 一つ目の頂点と次の頂点
                SimpleMath::Vector2 a = pts[i];
                SimpleMath::Vector2 b = pts[(i + 1) % 4];

                // 
                float cross = (b.x - a.x) * (target.y - a.y) - (b.y - a.y) * (target.x - a.x);

                if (cross > 0) hasPositive = true;
                if (cross < 0) hasNegative = true;

                // 
                if (hasPositive && hasNegative)
                {
                    isInside = false;
                    break;
                }
            }

            // 中にあるなら結果に追加
            if (isInside)
            {
                result.push_back({ x, z });
            }
        }
    }

    return result;
}

bool HeightMap::RayCast(
    LineCollider& line,
    DirectX::SimpleMath::Vector3& outHitPoint) const
{
    if (m_chunkCountX <= 0 || m_chunkCountZ <= 0)
        return false;

    const auto offset = GetCenterOffset();

    //=========================================================
    // 始点終点から線分の長さを計算
    //=========================================================
    const SimpleMath::Vector3 start = line.GetStart();
    const SimpleMath::Vector3 end = line.GetEnd();

    const SimpleMath::Vector3 dir = end - start;

    // 長さがなかったら処理をしない
    if (dir.LengthSquared() <= 0.000001f)
        return false;


    //=========================================================
    // 始点のグリッド番号と線分上にある各方向のグリッド数を計算
    //=========================================================
    // グリッド番号
    const float startGX = (start.x + offset.x) / m_gridSize;
    const float startGZ = (start.z + offset.y) / m_gridSize;
    // 線分上のグリッド数
    const float dirGX = dir.x / m_gridSize;
    const float dirGZ = dir.z / m_gridSize;


    //=========================================================
    // セルの数を取得
    //=========================================================
    const int cellCountX = m_width - 1;
    const int cellCountZ = m_depth - 1;

    // 片軸でもセルがなかったら処理をしない
    if (cellCountX <= 0 || cellCountZ <= 0)
        return false;

    //=========================================================
    // 線分が各軸に入った位置を計算
    //=========================================================

    float tMin = 0.0f;
    float tMax = 1.0f;

    //----------- X軸 -----------
    if (!MathLib::UpdateRayRange(startGX, dirGX, 0.0f, static_cast<float>(cellCountX), tMin, tMax))
        return false;

    //----------- Z軸 -----------
    if (!MathLib::UpdateRayRange(startGZ, dirGZ, 0.0f, static_cast<float>(cellCountX), tMin, tMax))
        return false;

    //=========================================================
    // 線分が衝突し始めたグリッド位置を計算（開始グリッド+線分*衝突位置の線分の割合）
    //=========================================================

    const float EPS = 0.00001f;                         // 誤差対策
    const float startT = std::min(tMin + EPS, tMax);    // 衝突位置が小さいほうを選ぶ
    const float currentGX = startGX + dirGX * startT;   // X方向の衝突位置
    const float currentGZ = startGZ + dirGZ * startT;   // Z方向の衝突位置


    //=========================================================
    // 今いるグリッドを範囲内にクランプ
    //=========================================================
    // 小数の丸めこみ
    int cellX = static_cast<int>(std::floor(currentGX));
    int cellZ = static_cast<int>(std::floor(currentGZ));

    // 範囲内にクランプ
    cellX = std::clamp(cellX, 0, cellCountX - 1);
    cellZ = std::clamp(cellZ, 0, cellCountZ - 1);

    //=========================================================
    // グリッドを進む向きを整理
    //=========================================================

    const int stepX = (dirGX > 0.0f) ? 1 :
        (dirGX < 0.0f) ? -1 : 0;

    const int stepZ = (dirGZ > 0.0f) ? 1 :
        (dirGZ < 0.0f) ? -1 : 0;

    //=========================================================
    // 
    //=========================================================

    float tMaxX = FLT_MAX;
    float tMaxZ = FLT_MAX;
    float tDeltaX = FLT_MAX;
    float tDeltaZ = FLT_MAX;

    //----------- X -----------

    if (stepX != 0)
    {
        // つぎに見るグリッド位置を計算
        const float nextBoundary = 
            (stepX > 0) ? static_cast<float>(cellX + 1) : static_cast<float>(cellX);

        // 線分がここを出た位置の割合
        tMaxX = startT + (nextBoundary - currentGX) / dirGX;
        // X方向に１グリッド進むときの線分の割合
        tDeltaX = 1.0f / std::abs(dirGX);
    }


    //----------- Z -----------

    if (stepZ != 0)
    {
        const float nextBoundary =
            (stepZ > 0) ? static_cast<float>(cellZ + 1) : static_cast<float>(cellZ);

        tMaxZ = startT + (nextBoundary - currentGZ) / dirGZ;
        tDeltaZ = 1.0f / std::abs(dirGZ);
    }


    //=========================================================
    // セルが範囲内の間
    //=========================================================

    while (cellX >= 0 &&
        cellX < cellCountX &&
        cellZ >= 0 &&
        cellZ < cellCountZ)
    {
        //=====================================================
        // 頂点から三角形を作成
        //=====================================================
        SimpleMath::Vector3 p0(
            cellX * m_gridSize - offset.x,
            GetHeight(cellX, cellZ),
            cellZ * m_gridSize - offset.y);

        SimpleMath::Vector3 p1(
            (cellX + 1) * m_gridSize - offset.x,
            GetHeight(cellX + 1, cellZ),
            cellZ * m_gridSize - offset.y);

        SimpleMath::Vector3 p2(
            cellX * m_gridSize - offset.x,
            GetHeight(cellX, cellZ + 1),
            (cellZ + 1) * m_gridSize - offset.y);

        SimpleMath::Vector3 p3(
            (cellX + 1) * m_gridSize - offset.x,
            GetHeight(cellX + 1, cellZ + 1),
            (cellZ + 1) * m_gridSize - offset.y);


        TriangleCollider t1(p0, p1, p2);
        TriangleCollider t2(p1, p3, p2);

        // どちらかに衝突したら衝突位置を返す
        if (RayCaster::RayCast(line, t1) ||
            RayCaster::RayCast(line, t2))
        {
            outHitPoint = line.GetCrossPoint();
            return true;
        }


        //=====================================================
        // 次に通過するグリッド境界のtが小さい軸へ進む
        //=====================================================

        if (tMaxX < tMaxZ)
        {
            // セルを進んで線分の割合に加算
            cellX += stepX;
            tMaxX += tDeltaX;
        }
        else
        {
            // セルを進んで線分の割合に加算
            cellZ += stepZ;
            tMaxZ += tDeltaZ;
        }


        // 次の線分が、有効な範囲を超えたら終了
        if (std::min(tMaxX, tMaxZ) > tMax)
            break;
    }

    return false;
}

// モデル読み込み
void HeightMap::LoadModel(const std::string& parth)
{
    auto ifs = StorageManager::GetInstance()->LoadTextFile(parth);

    try
    {
        json j = json::parse(ifs);
        if (j.is_array())
        {
            for (const auto& i : j)
            {
                if (i.is_object() && i.contains("name") && i.contains("pos"))
                {
                    SimpleMath::Vector3 pos = SimpleMath::Vector3::Zero;
                    SimpleMath::Vector3 size = SimpleMath::Vector3::Zero;
                    std::string name = i["name"].get<std::string>();
                    if (i["pos"].is_array())
                    {
                        pos.x = i["pos"][0].get<float>();
                        pos.y = i["pos"][1].get<float>();
                        pos.z = i["pos"][2].get<float>();
                    }
                    m_models.push_back(
                        std::move(m_of.CreateForJson<BreakableObject>(
                            name,
                            pos,
                            SimpleMath::Vector3::Zero,
                            DropFactory::GetInstance().GetInfo(name))
                        )
                    );
                }
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "モデルオブジェクトの読み込みに失敗しました\n" << e.what();
    }
}

void HeightMap::AddModel(std::unique_ptr<BreakableObject> model)
{

    // モデルの位置から所属するチャンクを計算
    auto pos = model->GetPosition();
    int x = std::floor(pos.x / m_chunkCountX) + m_chunkOffsetX;
    int z = std::floor(pos.z / m_chunkCountZ) + m_chunkOffsetX;
    m_chunks[z * m_chunkCountX + x].m_chunkObjects.push_back(model.get());

    // リストに追加
    m_models.push_back(std::move(model));
}

// 高さを更新
void HeightMap::SetHeight(int x, int z, float height)
{
    // 範囲内
    if (x >= 0 && x < m_width && z >= 0 && z < m_depth)
    {
        m_heights[z * m_width + x] = height;
        m_editDirty = true;

        // 未編集だったら
        if (!m_dirty)
        {
            // 最大値と最小値に送られた値を設定
            m_dirty = true;
            m_dirtyXMin = m_dirtyXMax = x;
            m_dirtyZMin = m_dirtyZMax = z;
        }
        // 編集済みなら
        else
        {
            // 現状と送られた値を比べて設定
            m_dirtyXMin = std::min(m_dirtyXMin, x);
            m_dirtyXMax = std::max(m_dirtyXMax, x);
            m_dirtyZMin = std::min(m_dirtyZMin, z);
            m_dirtyZMax = std::max(m_dirtyZMax, z);
        }

        // =========================================
        // 送られた頂点位置が属するチャンクと隣接するチャンクを編集済みに設定
        // =========================================

        // 現在のチャンクと隣接するチャンクのループ
        for (int cz = z - 1; cz <= z; ++cz)
        {
            for (int cx = x - 1; cx <= x; ++cx)
            {
                // チャンクが範囲外だったら次へ
                if (cx < 0 || cx >= m_width - 1 ||
                    cz < 0 || cz >= m_depth - 1)
                    continue;

                // 位置とチャンクの大きさから計算
                int chunkX = cx / CHUNK_SIZE;
                int chunkZ = cz / CHUNK_SIZE;

                int index = chunkZ * m_chunkCountX + chunkX;

                m_chunks[index].isDirty = true;
            }
        }
    }
}

DirectX::SimpleMath::Vector2 HeightMap::GetCenterOffset() const
{
    // 各軸の大きさにグリッドのサイズを掛けたものの半分
    return {
        (m_width - 1) * m_gridSize * 0.5f,
        (m_depth - 1) * m_gridSize * 0.5f
    };
}

// 頂点作成
void HeightMap::MakeVertex(int x, int z)
{
    auto offset = GetCenterOffset();

    // 送られた頂点の高さを取得
    auto h = [&](int px, int pz)
        {
            px = std::clamp(px, 0, m_width - 1);
            pz = std::clamp(pz, 0, m_depth - 1);
            return GetHeight(px, pz);
        };

    float y = h(x, z);
    // 送られた座標をグリッド座標に変換
    SimpleMath::Vector3 pos = { x * m_gridSize - offset.x, y, z * m_gridSize - offset.y };

    // 頂点の法線ベクトルを計算
    SimpleMath::Vector3 n{
        h(x - 1, z) - h(x + 1, z),  // X方向の傾き
        2.0f * m_gridSize,          // 隣接するマストのサンプル間隔
        h(x, z - 1) - h(x, z + 1)   // Z方向の傾き
    };
    n.Normalize();

    // 頂点情報を追加
    m_verts[z * m_width + x] = VertexPositionNormalColor{
        XMFLOAT3(pos.x, pos.y, pos.z),
        XMFLOAT3(n.x, n.y, n.z),
        CalcColor(y, n.y)
    };
}

// メッシュ情報の作成
void HeightMap::BuildMesh(ID3D11Device* device)
{
    // 最小値と最大値を設定
    m_minH = FLT_MAX;
    m_maxH = -FLT_MAX;
    for (int z = 0; z < m_depth; ++z)
        for (int x = 0; x < m_width; ++x)
        {
            float y = GetHeight(x, z);
            m_minH = std::min(m_minH, y);
            m_maxH = std::max(m_maxH, y);
        }

    // 頂点の二次元サイズ分メモリを確保
    m_verts.resize(static_cast<size_t>(m_width) * m_depth);
    for (int z = 0; z < m_depth; ++z)
        for (int x = 0; x < m_width; ++x)
            MakeVertex(x, z);

    // 頂点情報のバッファを作成
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = UINT(m_verts.size() * sizeof(VertexPositionNormalColor));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA sd = { m_verts.data() };
    device->CreateBuffer(&bd, &sd, m_vb.ReleaseAndGetAddressOf());

    // 各頂点から隣接する頂点を用いて三角形を作成
    // 頂点バッファがなかったら
    if (!m_ib)
    {
        std::vector<uint32_t> idx;
        // 三角形を描画するための頂点情報分のメモリを確保
        idx.reserve(static_cast<size_t>(m_width - 1) * (m_depth - 1) * 6);
        for (int z = 0; z < m_depth - 1; ++z)
            for (int x = 0; x < m_width - 1; ++x)
            {
                uint32_t a = z * m_width + x;   // (x, z)
                uint32_t b = a + 1;             // (x + 1, z)
                uint32_t c = a + m_width;       // (x, z + 1)
                uint32_t d = c + 1;             // (x + 1, z + 1)
                idx.insert(idx.end(), { a, b, c,  b, d, c });
            }
        m_indexCount = static_cast<UINT>(idx.size());

        // 三角形描画用の頂点バッファを作成
        D3D11_BUFFER_DESC ibd = {};
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        ibd.ByteWidth = UINT(idx.size() * sizeof(uint32_t));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        D3D11_SUBRESOURCE_DATA isd = { idx.data() };
        device->CreateBuffer(&ibd, &isd, m_ib.ReleaseAndGetAddressOf());
    }
}

void HeightMap::InitializeChunks()
{
    // 各軸のセルを整理
    const int cellCountX = m_width - 1;
    const int cellCountZ = m_depth - 1;

    // ０にならないようチャンクの数を計算
    m_chunkCountX = (cellCountX + CHUNK_SIZE - 1) / CHUNK_SIZE;
    m_chunkCountZ = (cellCountZ + CHUNK_SIZE - 1) / CHUNK_SIZE;

    // チャンクの数からチャンクのメモリを確保
    m_chunks.resize(m_chunkCountX * m_chunkCountZ);

    // オフセット計算
    m_chunkOffsetX = m_chunkCountX / 2;
    m_chunkOffsetZ = m_chunkCountZ / 2;
}

void HeightMap::UpdateChunkCache(TerrainChunk& chunk, int chunkX, int chunkZ)
{
    // チャンクの頂点数を初期化
    chunk.vertexCount = 0;

    // 縺薙・繝√Ε繝ｳ繧ｯ縺梧球蠖薙☆繧九そ繝ｫ縺ｮ遽・峇
    // チャンクの開始位置
    const int xStart = chunkX * CHUNK_SIZE;
    const int zStart = chunkZ * CHUNK_SIZE;
    // チャンクの終了地点
    const int xEnd = std::min(xStart + CHUNK_SIZE, m_width - 1);
    const int zEnd = std::min(zStart + CHUNK_SIZE, m_depth - 1);
    // オフセットと頂点色を設定
    const auto offset = GetCenterOffset();
    const auto color = Colors::Brown;

    // 線分を追加するラムダ
    auto addLine = [&](const SimpleMath::Vector3& start, const SimpleMath::Vector3& end)
        {
            chunk.vertices[chunk.vertexCount++] = VertexPositionColor(start, color);

            chunk.vertices[chunk.vertexCount++] = VertexPositionColor(end, color);
        };

    for (int z = zStart; z < zEnd; ++z)
    {
        for (int x = xStart; x < xEnd; ++x)
        {
            // 現在の位置を整理
            const float currentY = GetHeight(x, z);

            const SimpleMath::Vector3 currentPos(
                x * m_gridSize - offset.x,
                currentY,
                z * m_gridSize - offset.y);

            // 右隣接の高さを整理
            const float rightY = GetHeight(x + 1, z);

            const SimpleMath::Vector3 rightPos(
                (x + 1) * m_gridSize - offset.x,
                rightY,
                z * m_gridSize - offset.y);

            // 下隣接の整理
            const float bottomY = GetHeight(x, z + 1);

            const SimpleMath::Vector3 bottomPos(
                x * m_gridSize - offset.x,
                bottomY,
                (z + 1) * m_gridSize - offset.y);

            // 上辺を追加
            addLine(currentPos, rightPos);

            // 左辺を追加
            addLine(currentPos, bottomPos);

            // 斜辺を追加
            addLine(bottomPos, rightPos);
        }
    }

    // 変更済みを解除
    chunk.isDirty = false;
}

void HeightMap::UpdateChunkCaches()
{
    for (int z = 0; z < m_chunkCountZ; ++z)
    {
        for (int x = 0; x < m_chunkCountX; ++x)
        {
            const int index = z * m_chunkCountX + x;

            auto& chunk = m_chunks[index];

            // 更新があったチャンクだけ更新
            if (chunk.isDirty)
            {
                UpdateChunkCache(chunk, x, z);
            }
        }
    }
}

// 更新された頂点をバッファに再登録
void HeightMap::UpdateRegion(ID3D11DeviceContext* context,
    int xMin, int xMax, int zMin, int zMax)
{
    // 各座標を範囲内にクランプ
    xMin = std::max(xMin - 1, 0);
    zMin = std::max(zMin - 1, 0);
    xMax = std::min(xMax + 1, m_width - 1);
    zMax = std::min(zMax + 1, m_depth - 1);
    // 送られた座標に矛盾があったら処理をしない
    if (xMin > xMax || zMin > zMax) return;

    const UINT stride = sizeof(VertexPositionNormalColor);

    for (int z = zMin; z <= zMax; ++z)
    {
        for (int x = xMin; x <= xMax; ++x)
            MakeVertex(x, z);

        // 送られた頂点から矩形バッファを再登録
        D3D11_BOX box = {};
        box.left = UINT(z * m_width + xMin) * stride;
        box.right = UINT(z * m_width + xMax + 1) * stride;
        box.top = 0;
        box.bottom = 1;
        box.front = 0;
        box.back = 1;
        context->UpdateSubresource(m_vb.Get(), 0, &box,
            &m_verts[z * m_width + xMin], 0, 0);
    }
}

// マップ読み込み
void HeightMap::LoadMap(const std::string& path)
{
    std::string str = StorageManager::LoadTextFile(path);

    // UTF-8 BOM付きのCSVでも先頭の数値を読み取れるようにする
    if (str.compare(0, 3, "\xEF\xBB\xBF") == 0)
    {
        str.erase(0, 3);
    }

    std::istringstream fileStream(str);
    std::string line;

    // 行区切りで読み込み
    if (std::getline(fileStream, line))
    {
        std::stringstream ss(line);
        std::string token;

        // 最初にこのマップの幅と深さとグリッドの大きさを取得
        std::getline(ss, token, ',');
        m_width = std::stoi(token);
        std::getline(ss, token, ',');
        m_depth = std::stoi(token);
        std::getline(ss, token, ',');
        m_gridSize = std::stof(token);

        // マップの範囲内に０を上書き
        m_heights.assign(m_width * m_depth, 0.0f);
    }

    // 二次元情報を一次元情報に変換
    int z = 0;
    while (std::getline(fileStream, line) && z < m_depth)
    {
        std::stringstream ss(line);
        std::string token;
        int x = 0;

        // コンマ区切りで読み込み
        while (std::getline(ss, token, ',') && x < m_width)
        {
            {
                m_heights[z * m_width + x] = std::stof(token);
            }
            x++;
        }
        z++;
    }
    m_editDirty = true;
}

bool HeightMap::SaveMap()
{
    // 一時ファイルに書き込み、成功したらマップを置き換える
    std::ofstream ofs("Resources/CSV/Map.csv.tmp", std::ios::trunc);
    // ファイルを開けなかったら処理を中断
    if (!ofs)
    {
        MessageBoxW(nullptr, L"Map save failed: cannot open Resources/CSV/Map.csv.tmp", L"Save error", MB_OK | MB_ICONERROR);
        return false;
    }
    ofs << std::setprecision(std::numeric_limits<float>::max_digits10);

    // 幅、深さ、グリッドサイズの書き込み
    ofs << m_width << ',' << m_depth << ',' << m_gridSize << '\n';

    for (int i = 0; i < m_depth; i++)
    {
        for (int j = 0; j < m_width; j++)
        {
            ofs << m_heights[i * m_width + j];
            // 改行の位置じゃなかったらコンマも追加
            if (j < m_width - 1)
            {
                ofs << ',';
            }
        }
        ofs << '\n';
    }

    ofs.close();
    // ファイル出力が失敗 or ファイル移動が失敗したら中断
    if (!ofs || !MoveFileExW(L"Resources/CSV/Map.csv.tmp", L"Resources/CSV/Map.csv",
        MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        MessageBoxW(nullptr, L"Map save failed: cannot write Resources/CSV/Map.csv", L"Save error", MB_OK | MB_ICONERROR);
        return false;
    }

    return true;
}

// モデルオブジェクトの保存
void HeightMap::SaveModel()
{
    auto ofs = StorageManager::GetInstance()->LoadInputTextFile("Resources/Jsons/ModelObjectsData.json");
    try
    {
        json j = json::array();
        for (auto& o : m_models)
        {
            json i;
            i["name"] = o->GetData().name;
            auto pos = o->GetPosition();
            i["pos"] = { MathLib::RoundPosition(pos.x),
                        MathLib::RoundPosition(pos.y),
                        MathLib::RoundPosition(pos.z) };
            j.push_back(i);
        }
        ofs << j.dump(4) << std::endl;
    }
    catch (const std::exception&)
    {
        ofs.close();
    }
    ofs.close();
}

float HeightMap::GetTriangleHeight(
    const DirectX::SimpleMath::Vector3& p0,
    const DirectX::SimpleMath::Vector3& p1,
    const DirectX::SimpleMath::Vector3& p2,
    float x,
    float z) const
{
    // 三角形の調べたい座標の高さを計算

    float det =
        (p1.z - p2.z) * (p0.x - p2.x) +
        (p2.x - p1.x) * (p0.z - p2.z);

    float l1 =
        ((p1.z - p2.z) * (x - p2.x) +
            (p2.x - p1.x) * (z - p2.z)) / det;

    float l2 =
        ((p2.z - p0.z) * (x - p2.x) +
            (p0.x - p2.x) * (z - p2.z)) / det;

    float l3 = 1.0f - l1 - l2;

    return
        l1 * p0.y +
        l2 * p1.y +
        l3 * p2.y;
}

DirectX::SimpleMath::Vector3 HeightMap::GetNormal(float worldX, float worldZ) const
{
    // 座標が属するマスを求める
    auto [x, z] = GetGrid(worldX, worldZ);
    if (!IsInvid(static_cast<int>(x), static_cast<int>(z))) 
        return DirectX::SimpleMath::Vector3::Up;

    // マスのどの位置にいるか（0~1）
    auto offset = GetCenterOffset();
    float localX = ((worldX + offset.x) / m_gridSize) - x;
    float localZ = ((worldZ + offset.y) / m_gridSize) - z;

    // 送られた座標がいる矩形を計算
    DirectX::SimpleMath::Vector3 p0(0.0f, GetHeight(x, z), 0.0f);
    DirectX::SimpleMath::Vector3 p1(m_gridSize, GetHeight(x + 1, z), 0.0f);
    DirectX::SimpleMath::Vector3 p2(0.0f, GetHeight(x, z + 1), m_gridSize);
    DirectX::SimpleMath::Vector3 p3(m_gridSize, GetHeight(x + 1, z + 1), m_gridSize);

    DirectX::SimpleMath::Vector3 normal;

    // 上側の三角形か、下側の三角形化を計算
    if (localX + localZ <= 1.0f)
    {
        // ベクトルを計算
        DirectX::SimpleMath::Vector3 v1 = p2 - p0;
        DirectX::SimpleMath::Vector3 v2 = p1 - p0;
        // 外積から法線を取得
        normal = v1.Cross(v2);
    }
    else
    {
        DirectX::SimpleMath::Vector3 v1 = p3 - p2;
        DirectX::SimpleMath::Vector3 v2 = p1 - p2;
        normal = v1.Cross(v2);
    }

    // 法線の正規化
    normal.Normalize();

    // 法線が地面の下向きにならないようにする
    if (normal.y < 0.0f) normal = -normal;

    return normal;
}
