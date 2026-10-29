//====================================================//
// ファイル名   : HeightMap.h
// 作成者       : 高安瑞貴
// 作成日時     : 2026/06/17 14:35:20
//
// 概要 : ワールドの地面を管理するクラス
//
// 更新履歴 :
// 2026/06/17 14:35:20 新規作成
//====================================================//

#pragma once
#include "Tools/GameContext.h"
#include "SimpleMath.h"
#include <vector>
#include "Common/UI/Image.h"
#include <Game/GamePlay/GameObject/BreakableObject.h>
#include "Game/GamePlay/Factory/ObjectFactory.h"

class LineCollider;

//====================================================//
// インクルードファイル
//====================================================//


//====================================================//
// クラス宣言
//====================================================//
class HeightMap
{

public:

    //-----------------------------------------------------
    // Getter
    //-----------------------------------------------------
    std::pair<int, int> GetSize() { return {m_width, m_depth}; } // マップの大きさ取得
    float GetGridSize() { return m_gridSize; }
    float GetHeight(int x, int z) const;    // 高さ
    float GetWorldHeight(float worldX, float worldZ) const; // ワールド座標の高さ
    bool IsSlide(float worldX, float WorldZ) const;    // プレイヤーのいる範囲が滑るかどうか
    bool IsInvid(int x, int z)const;
    SimpleMath::Vector2 GetGrid(float worldX, float worldZ)const;
    DirectX::SimpleMath::Vector3 GetNormal(float worldX, float worldZ) const; // 法線を求める関数
    const std::vector<std::unique_ptr<BreakableObject>>& GetModels() { return m_models; }

    std::vector<std::pair<int, int>> GetVerticesInQuad(SimpleMath::Vector3 p0, SimpleMath::Vector3 p1, SimpleMath::Vector3 p2, SimpleMath::Vector3 p3) const;
    //-----------------------------------------------------
    // Setter
    //-----------------------------------------------------
    void SetHeight(int x, int z, float height);
    void SetGrid(int x, int z)
    {
        m_width = x;
        m_depth = z;
        m_heights.resize(static_cast<size_t>(x) * z, 0);
        m_meshDirty = true;
        m_editDirty = true;
    }
    void SetGridSize(float size)
    {
        m_gridSize = size;
        m_meshDirty = true;
        m_editDirty = true;
    }
    // プレイヤー位置保存（水平方向のみ）
    void SetPlayerPos(SimpleMath::Vector3 pos) { m_playerPos = { pos.x, pos.z }; }


    // マップ読みこみ
    void LoadMap(const std::string& path);
    // マップの保存
    bool SaveMap();

    bool RayCast(LineCollider& line, DirectX::SimpleMath::Vector3& outHitPoint) const; 

    // 地形モデル読み込み
    void LoadModel(const std::string& parth);
    // 地形モデルの追加
    void AddModel(std::unique_ptr<BreakableObject> model);
    // 地形モデルの保存
    void SaveModel();

public:

    //-----------------------------------------------------
    // コンストラクタ / デストラクタ
    //-----------------------------------------------------
    HeightMap(float size = 0.5f);
    ~HeightMap();

    // 初期化
    void Initialize(const GameContext& gameContext);
    // 描画
    void Render(const GameContext& gameContext, SimpleMath::Matrix view);
    // プレイヤー周辺のチャンクを描画
    void DrawChunks(const SimpleMath::Vector3& playerPos,int radius);
    // エディット用の描画
    void EditRender(const GameContext& gameContext, const SimpleMath::Vector3& playerPos, int radius, SimpleMath::Matrix view);
    void UpdateRegion(ID3D11DeviceContext* context, int xMin, int xMax, int zMin, int zMax);
    void MakeVertex(int x, int z);

private:

    // グリッドの情報
    int m_width;
    int m_depth;
    float m_gridSize;

    // 変更があった範囲(格子番号)。m_dirty が true のときだけ有効
    bool m_dirty = false;
    int  m_dirtyXMin = 0, m_dirtyXMax = 0;
    int  m_dirtyZMin = 0, m_dirtyZMax = 0;
    
    // グリッドの原点座標
    SimpleMath::Vector2 m_origin;

    // 各頂点における高さ(メモリ削減のため一次元配列)
    std::vector<float> m_heights;

    // 各チャンクの辺の頂点数
    static constexpr int CHUNK_SIZE = 16;

    // 1セルあたり最大6頂点
    static constexpr int VERTICES_PER_CELL = 6;

    // 1チャンクの最大頂点数
    static constexpr int MAX_CHUNK_VERTICES = CHUNK_SIZE * CHUNK_SIZE * VERTICES_PER_CELL;

    // プレイヤーの周り何マスを描画するか
    static constexpr int REND_MASS = 100;

    // チャンク
    struct TerrainChunk
    {
        // グリッド線の頂点データ
        std::array<VertexPositionColor, MAX_CHUNK_VERTICES> vertices{};

        // このチャンク内にいるモデルオブジェクト
        std::vector<BreakableObject*> m_chunkObjects;

        // 実際に使用している頂点数
        size_t vertexCount = 0;

        // キャッシュの更新が必要か
        bool isDirty = true;
    };

    // チャンクの横方向・縦方向の個数
    uint8_t m_chunkCountX = 0;
    uint8_t m_chunkCountZ = 0;
    // チャンクのオフセット
    uint8_t m_chunkOffsetX;
    uint8_t m_chunkOffsetZ;
    // チャンクのデータ
    std::vector<TerrainChunk> m_chunks;

    // 滑る角度
    const float SLIDE_ANGLE = 30.0f;

    // 地形モデルオブジェクト
    std::vector<std::unique_ptr<BreakableObject>> m_models;
    // モデルファクトリー
    ObjectFactory m_of;

    // 頂点とかのバッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vb, m_ib;
    // 頂点の個数
    UINT m_indexCount = 0;

    // CPU側の頂点キャッシュ(部分更新に使う)
    std::vector<VertexPositionNormalColor> m_verts;
    // 色分け用の高さ範囲(BuildMesh時に決定)
    float m_minH = 0.0f;
    float m_maxH = 1.0f;

    // プレイヤーの座標(チャンク描画用)
    SimpleMath::Vector2 m_playerPos;

    std::vector<VertexPositionColor> m_editVertices;
    int  m_editPx = -1, m_editPz = -1, m_editRadius = -1;
    bool m_editDirty = true;   // SetHeight / SetGrid / SetGridSize / 読み込み時に true にする

    std::unique_ptr<DirectX::BasicEffect> m_effect;          // 地形専用エフェクト
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;      // 地形用の入力レイアウト
    bool m_meshDirty = false;    // 高さが変わったらtrue

private:

    // 三角形にしたマスの高さを取得
    float GetTriangleHeight(
        const DirectX::SimpleMath::Vector3& p0,
        const DirectX::SimpleMath::Vector3& p1,
        const DirectX::SimpleMath::Vector3& p2,
        float x,
        float z
    ) const;

    // マップの中心を原点にするためのオフセット計算
    DirectX::SimpleMath::Vector2 GetCenterOffset() const;

    // 描画用メッシュを事前に作成
    void BuildMesh(ID3D11Device* device);
    // チャンクの初期化
    void InitializeChunks();
    // 各チャンクの更新
    void UpdateChunkCache(TerrainChunk& chunk, int chunkX, int chunkZ);
    // チャンク全体の更新
    void UpdateChunkCaches();
};