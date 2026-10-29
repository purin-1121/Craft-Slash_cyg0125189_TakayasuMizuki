#pragma once

#include <memory>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <array>
#include <vector>

// 1頂点に影響するボーン数の上限
constexpr unsigned int MAX_BONE_INFLUENCE = 4;

// 1頂点が持つスキニング情報
struct SkinVertex
{
    // FBXから読み込んだ変形前の頂点座標
    aiVector3D position{ 0.0f, 0.0f, 0.0f };
    aiVector3D textureCoordinate{ 0.0f, 0.0f, 0.0f };

    // 法線。ライティング計算で使用する
    aiVector3D normal{ 0.0f, 1.0f, 0.0f };

    // この頂点に影響するボーンの番号
    std::array<unsigned int, MAX_BONE_INFLUENCE> boneIDs{};

    // 各ボーンが頂点に与える影響の割合
    std::array<float, MAX_BONE_INFLUENCE> weights{};
};

// ボーン1本分の情報
struct SkinBone
{
    // Animatorのノード名と対応付けるための名前
    std::string name;

    // メッシュの頂点をボーンのローカル空間へ戻す変換
    aiMatrix4x4 offsetMatrix;
};

struct SkinMesh
{
    // 変形前の頂点
    std::vector<SkinVertex> vertices;

    // 三角形を構成する頂点番号
    std::vector<unsigned int> indices;

    // このメッシュ専用のボーン情報
    // 各頂点の boneIDs は、この配列のインデックスを参照する
    std::vector<SkinBone> bones;
    unsigned int materialIndex = 0;
};

class ModelResource
{
public:

    // FBXなどのモデルデータを読み込む
    bool Load(const std::string& filePath);
    bool LoadAnimation(
        const std::string& filePath,
        const std::shared_ptr<ModelResource>& modelResource);

    // 読み込んだシーンを取得する
    // Keep model-specific bones and bind poses when applying shared clips.
    const aiScene* GetSkeletonScene() const
    {
        return m_baseModel ? m_baseModel->GetSkeletonScene() : m_scene;
    }
    const aiScene* GetScene() const
    {
        return m_scene;
    }

    // 読み込んだメッシュのスキニング情報を取得
    const std::vector<SkinMesh>& GetSkinMeshes() const
    {
        return *m_skinMeshes;
    }

    // モデル全体のボーン情報を取得
    const std::vector<SkinBone>& GetSkinBones() const
    {
        return m_skinBones;
    }

private:

    // メッシュ単位の頂点・インデックス情報
    std::shared_ptr<std::vector<SkinMesh>> m_skinMeshes = std::make_shared<std::vector<SkinMesh>>();

    // モデル全体で共有するボーン情報
    std::vector<SkinBone> m_skinBones;

    // ボーン名からボーン番号を検索する
    std::unordered_map<std::string, unsigned int> m_boneNameToID;

    // シーンのメッシュ情報からスキニングデータを作る
    bool LoadSkinningData(const aiScene* scene);

    // aiSceneの所有権はImporter側にあるので、
    // シーンを使用している間はImporterを保持する
    std::unique_ptr<Assimp::Importer> m_importer;

    std::shared_ptr<ModelResource> m_baseModel;
    const aiScene* m_scene = nullptr;
};