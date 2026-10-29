#include "pch.h"
#include "ModelResource.h"

bool ModelResource::Load(const std::string& filePath)
{
    m_scene = nullptr;
    m_baseModel.reset();
    m_skinMeshes = std::make_shared<std::vector<SkinMesh>>();
    m_importer = std::make_unique<Assimp::Importer>();

    m_scene = m_importer->ReadFile(
        filePath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights
    );

    // シーンの読み込みが成功した後に実行
    if (!LoadSkinningData(m_scene))
    {
        std::cerr << "スキニング情報の読み込みに失敗しました\n";
        return false;
    }

    // 読み込みに失敗したらエラーを出す
    if (m_scene == nullptr ||
        (m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) ||
        m_scene->mRootNode == nullptr)
    {
        OutputDebugStringA(
            m_importer->GetErrorString()
        );
        OutputDebugStringA("\n");

        m_scene = nullptr;
        return false;
    }

    return true;
}

//============================================================
// Assimpで読み込んだシーンからスキニング情報を構築する
//============================================================
bool ModelResource::LoadAnimation(
    const std::string& filePath,
    const std::shared_ptr<ModelResource>& modelResource)
{
    if (!modelResource || modelResource.get() == this ||
        modelResource->GetSkinMeshes().empty())
    {
        return false;
    }

    auto importer = std::make_unique<Assimp::Importer>();
    const aiScene* scene = importer->ReadFile(
        filePath,
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights);

    if (scene == nullptr ||
        scene->mRootNode == nullptr ||
        scene->mNumAnimations == 0)
    {
        OutputDebugStringA(importer->GetErrorString());
        OutputDebugStringA("\nAnimation-only FBX load failed.\n");
        std::ostringstream oss;
        oss << "[LoadAnimation]"
            << " flags=" << scene->mFlags
            << " incomplete="
            << ((scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0)
            << " rootNull=" << (scene->mRootNode == nullptr)
            << " animations=" << scene->mNumAnimations
            << '\n';
        const std::string messgae = oss.str();

        OutputDebugStringA(messgae.c_str());
        m_scene = nullptr;
        return false;
    }

    m_importer = std::move(importer);
    m_scene = scene;
    m_baseModel = modelResource;
    m_skinMeshes = modelResource->m_skinMeshes;
    m_skinBones.clear();
    m_boneNameToID.clear();
    return true;
}
bool ModelResource::LoadSkinningData(const aiScene* scene)
{
    // シーンやメッシュが存在しない場合は失敗
    if (scene == nullptr || scene->mNumMeshes == 0)
    {
        return false;
    }

    // 再読み込みに備えて、以前の情報を消去する
    m_skinMeshes = std::make_shared<std::vector<SkinMesh>>();
    m_skinBones.clear();
    m_boneNameToID.clear();

    // メッシュごとに処理する
    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        const aiMesh* mesh = scene->mMeshes[meshIndex];

        if (mesh == nullptr)
        {
            continue;
        }

        // このメッシュ用のデータを作る
        SkinMesh skinMesh;
        skinMesh.materialIndex = mesh->mMaterialIndex;

        // 頂点数に合わせて配列を確保
        skinMesh.vertices.resize(mesh->mNumVertices);

        // ---------------------------------------------
        // ① 頂点座標と法線をコピーする
        // ---------------------------------------------
        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            SkinVertex& vertex = skinMesh.vertices[i];

            // FBXの元の頂点座標
            vertex.position = mesh->mVertices[i];

            // 法線が存在する場合だけコピー
            if (mesh->HasNormals())
            {
                vertex.normal = mesh->mNormals[i];
            }

            if (mesh->HasTextureCoords(0))
            {
                vertex.textureCoordinate = mesh->mTextureCoords[0][i];
                vertex.textureCoordinate.y = 1.0f - vertex.textureCoordinate.y;
            }
        }

        // ---------------------------------------------
        // ② ボーン情報とウェイトを読み込む
        // ---------------------------------------------
        for (unsigned int i = 0; i < mesh->mNumBones; ++i)
        {
            const aiBone* bone = mesh->mBones[i];

            if (bone == nullptr)
                continue;

            // このメッシュ内でのボーン番号
            // 頂点が持つ boneIDs と同じ番号体系にする
            const unsigned int boneID =
                static_cast<unsigned int>(skinMesh.bones.size());

            // ボーン名とオフセット行列を保存する
            SkinBone skinBone;
            skinBone.name = bone->mName.C_Str();
            skinBone.offsetMatrix = bone->mOffsetMatrix;

            std::ostringstream oss;

            oss << skinBone.name.c_str() << '\n';

            const std::string message = oss.str();

            OutputDebugStringA(message.c_str());

            skinMesh.bones.push_back(skinBone);

            // そのボーンが影響する頂点とウェイトを登録
            for (unsigned int j = 0; j < bone->mNumWeights; ++j)
            {
                const aiVertexWeight& weight = bone->mWeights[j];

                if (weight.mVertexId >= skinMesh.vertices.size())
                    continue;

                SkinVertex& vertex =
                    skinMesh.vertices[weight.mVertexId];

                // 空いているウェイト枠に登録する
                for (unsigned int k = 0; k < MAX_BONE_INFLUENCE; ++k)
                {
                    if (vertex.weights[k] == 0.0f)
                    {
                        vertex.boneIDs[k] = boneID;
                        vertex.weights[k] = weight.mWeight;
                        break;
                    }
                }
            }
        }
        // ---------------------------------------------
        // ③ ウェイトの合計を1に正規化する
        // ---------------------------------------------
        for (SkinVertex& vertex : skinMesh.vertices)
        {
            float totalWeight = 0.0f;

            for (float weight : vertex.weights)
            {
                totalWeight += weight;
            }

            // 合計が0より大きければ、合計が1になるよう調整
            if (totalWeight > 0.0f)
            {
                for (float& weight : vertex.weights)
                {
                    weight /= totalWeight;
                }
            }
        }

        // ---------------------------------------------
        // ④ 三角形のインデックスをコピーする
        // ---------------------------------------------
        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                skinMesh.indices.push_back(face.mIndices[j]);
            }
        }

        // 完成したメッシュを登録
        m_skinMeshes->push_back(std::move(skinMesh));
    }

    // 頂点とボーンの情報が取得できた
    // メッシュが存在しなければ失敗
    if (m_skinMeshes->empty())
    {
        return false;
    }

    // 少なくとも1つのメッシュにボーンがあれば成功
    for (const SkinMesh& mesh : *m_skinMeshes)
    {
        if (!mesh.bones.empty())
        {
            return true;
        }
    }

    // メッシュは読み込めたが、ボーン情報が見つからなかった
    return false;
}