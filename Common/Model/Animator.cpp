    #include "pch.h"
    #include "Animator.h"

    #include <cmath>
    #include <algorithm>

    // デバッグ用
    #include <chrono>
    #include <sstream>
    #include <iomanip>
    #include <Windows.h>

    namespace
    {
        // Mixamo exports can use different namespaces for the same skeleton.
        std::string CanonicalNodeName(const std::string& name)
        {
            if (name.compare(0, 9, "mixamorig") == 0)
            {
                const auto separator = name.find(':');
                if (separator != std::string::npos)
                    return name.substr(separator + 1);
            }
            return name;
        }
        //====================================================
        // 位置・拡縮キーの補間
        //====================================================
        // アニメーションキーとキーの間の値を計算する。
        //
        // 例：
        // 0秒  : X = 0
        // 1秒  : X = 10
        // 0.5秒: X = 5
        //
        // countが0の場合は、アニメーションキーがないため
        // ノードが元々持っている値(fallback)を使用する。
        //====================================================
        void CacheSourceNodes(const aiNode* node,
            std::unordered_map<std::string, const aiNode*>& nodes)
        {
            nodes.emplace(CanonicalNodeName(node->mName.C_Str()), node);
            for (unsigned int i = 0; i < node->mNumChildren; ++i)
                CacheSourceNodes(node->mChildren[i], nodes);
        }
        aiVector3D InterpolateVector(
            const aiVectorKey* keys, // 補間対象のキー配列
            unsigned int count,      // キーの個数
            double time,             // 現在のアニメーション時刻
            const aiVector3D& fallback)
        {
            // キーがない場合は、元の位置・拡縮を返す
            if (count == 0)
                return fallback;

            // キーが1つしかない場合や、最初のキーより前の場合
            // 最初のキーの値を使用する
            if (count == 1 || time <= keys[0].mTime)
                return keys[0].mValue;

            // 最後のキーより後の場合は、最後の値を使用する
            if (time >= keys[count - 1].mTime)
                return keys[count - 1].mValue;

            // 現在時刻を挟んでいる2つのキーを探す
            for (unsigned int i = 0; i + 1 < count; ++i)
            {
                // keys[i] <= time < keys[i + 1] となる区間を探す
                if (time < keys[i + 1].mTime)
                {
                    // 2つのキーの時間差
                    const double duration = keys[i + 1].mTime - keys[i].mTime;

                    // 2つのキーの間で現在時刻がどこにあるかを
                    // 0～1の割合として求める
                    const float factor = static_cast<float>((time - keys[i].mTime) / duration);

                    // 2つの値をfactorの割合で線形補間する
                    // factor = 0なら前のキー、1なら次のキー
                    return keys[i].mValue + (keys[i + 1].mValue - keys[i].mValue) * factor;
                }
            }

            // 通常ここには到達しないが、念のため最後の値を返す
            return keys[count - 1].mValue;
        }

        //====================================================
        // 回転キーの補間
        //====================================================
        // 回転は位置のように単純な数値の線形補間を行わず、
        // クォータニオン用の補間処理を使用する。
        //
        // aiQuaternion::Interpolateは2つの回転の間を補間する。
        // 最後にNormalizeして、回転として有効な長さに整える。
        //====================================================
        aiQuaternion InterpolateRotation(
            const aiQuatKey* keys,   // 回転キーの配列
            unsigned int count,      // キーの個数
            double time,             // 現在のアニメーション時刻
            const aiQuaternion& fallback)
        {
            // 回転キーがない場合は、元の回転を使用する
            if (count == 0)
                return fallback;

            // 最初のキーより前、またはキーが1つしかない場合
            if (count == 1 || time <= keys[0].mTime)
                return keys[0].mValue;

            // 最後のキーより後の場合
            if (time >= keys[count - 1].mTime)
                return keys[count - 1].mValue;

            // 現在時刻を挟む2つの回転キーを探す
            for (unsigned int i = 0; i + 1 < count; ++i)
            {
                if (time < keys[i + 1].mTime)
                {
                    // 2つのキーの時間差から補間率を求める
                    const double duration =
                        keys[i + 1].mTime - keys[i].mTime;

                    const ai_real factor = static_cast<ai_real>(
                        (time - keys[i].mTime) / duration);

                    aiQuaternion result;

                    // 2つの回転を補間する
                    aiQuaternion::Interpolate(
                        result,
                        keys[i].mValue,
                        keys[i + 1].mValue,
                        factor);

                    // クォータニオンを正規化する
                    result.Normalize();

                    return result;
                }
            }

            return keys[count - 1].mValue;
        }
    }

    //============================================================
    // モデルの設定
    //============================================================
    void Animator::SetModel(std::shared_ptr<ModelResource> model)
{
    m_model = std::move(model);
    m_animation = nullptr;
    m_time = 0.0f;
    m_animationTimeTicks = 0.0;
    m_globalTransforms.clear();
    m_nodesByName.clear();
    m_animationChannelsByNode.clear();
    m_animationPositionOffsetsByNode.clear();
    m_skinBoneNodesByMesh.clear();
    m_globalInverseTransform = aiMatrix4x4();

    if (!m_model)
        return;

    const aiScene* scene = m_model->GetSkeletonScene();
    if (scene == nullptr || scene->mRootNode == nullptr)
        return;

    CacheNode(scene->mRootNode);

    for (const SkinMesh& mesh : m_model->GetSkinMeshes())
    {
        std::vector<const aiNode*> nodes;
        nodes.reserve(mesh.bones.size());
        for (const SkinBone& bone : mesh.bones)
        {
            const auto nodeIt = m_nodesByName.find(CanonicalNodeName(bone.name));
            nodes.push_back(nodeIt == m_nodesByName.end() ? nullptr : nodeIt->second);
        }
        m_skinBoneNodesByMesh.emplace(&mesh, std::move(nodes));
    }

    m_globalInverseTransform = scene->mRootNode->mTransformation;
    m_globalInverseTransform.Inverse();
}
bool Animator::Play(unsigned int animationIndex, bool isLoop)
{
    if (!m_model || !m_model->GetScene())
        return false;

    const aiScene* scene = m_model->GetScene();
    if (animationIndex >= scene->mNumAnimations)
        return false;

    std::unordered_map<std::string, const aiNode*> sourceNodes;
    CacheSourceNodes(scene->mRootNode, sourceNodes);

    m_animation = scene->mAnimations[animationIndex];
    m_animationChannelsByNode.clear();
    m_animationPositionOffsetsByNode.clear();
    for (unsigned int i = 0; i < m_animation->mNumChannels; ++i)
    {
        const aiNodeAnim* channel = m_animation->mChannels[i];
        const auto nodeIt = m_nodesByName.find(CanonicalNodeName(channel->mNodeName.C_Str()));
        if (nodeIt != m_nodesByName.end())
        {
            const aiNode* targetNode = nodeIt->second;
            m_animationChannelsByNode.emplace(targetNode, channel);
            const auto sourceIt = sourceNodes.find(CanonicalNodeName(channel->mNodeName.C_Str()));
            if (sourceIt != sourceNodes.end())
            {
                const auto& targetBind = targetNode->mTransformation;
                const auto& sourceBind = sourceIt->second->mTransformation;
                // Retain the model's bind translation; transfer only clip motion.
                m_animationPositionOffsetsByNode.emplace(targetNode, aiVector3D(
                    targetBind.a4 - sourceBind.a4,
                    targetBind.b4 - sourceBind.b4,
                    targetBind.c4 - sourceBind.c4));
            }
        }
    }

    m_time = 0.0f;
    m_animationTimeTicks = 0.0;
    m_isEnd = false;
    m_isLoop = isLoop;
    Update(0.0f);
    return true;
}
void Animator::Update(float deltaTime)
    {
        using Clock = std::chrono::steady_clock;

        // Animator::Update() の処理開始時刻
        const auto updateStart = Clock::now();

        // すでにアニメーションが終了しているなら何もしない
        if (m_isEnd)
            return;

        // モデルが設定されていなければ何もしない
        if (!m_model || !m_model->GetSkeletonScene())
            return;

        const aiScene* scene = m_model->GetSkeletonScene();

        // ノード階層の起点がなければ計算できない
        if (!scene->mRootNode)
            return;

        //================================================
        // 再生時間を進める
        //================================================
        m_animationTimeTicks = 0.0;
        if (m_animation)
        {
            // アニメーションデータの時間単位を取得する。
            // キーの時刻は秒とは限らず、Ticks単位で保存されている。
            // TicksPerSecondが0の場合は25を代わりに使用する。
            const double ticksPerSecond =
                m_animation->mTicksPerSecond > 0.0
                ? m_animation->mTicksPerSecond
                : 25.0;

            // アニメーション全体の長さ（Ticks単位）
            const double duration = m_animation->mDuration;

            if (duration > 0.0)
            {
                m_time += std::max(deltaTime * m_multiplier, 0.0f);

                // ゲームの経過時間(deltaTime)は秒単位なので、
                const double elapsedTicks = m_time * ticksPerSecond;

                if (m_isLoop)
                {
                    // ループ時だけ、長さを超えた分を先頭に戻す
                    m_animationTimeTicks = std::fmod(elapsedTicks, duration);
                }
                else if (elapsedTicks >= duration)
                {
                    // 非ループは最後の時間で止める
                    m_animationTimeTicks = duration;
                    m_isEnd = true;
                }
                else
                {
                    m_animationTimeTicks = elapsedTicks;
                }
            }
        }

        //================================================
        // ノード階層の姿勢を計算する
        //================================================
        // ノード計算を開始する時刻
        const auto poseStart = Clock::now();

        // 前フレームの結果を消去する
        //m_globalTransforms.clear();   

        // ルートノードから全階層の変換行列を計算する
        CalculateNode(scene->mRootNode, aiMatrix4x4());

        // ノード階層の計算が終わった時刻
        const auto poseEnd = Clock::now();

        // 今回のノード計算時間（ミリ秒）
        const double poseMs =
            std::chrono::duration<double, std::milli>(
                poseEnd - poseStart).count();

        //================================================
        // Animator::Update() の処理時間を集計
        //================================================

        // 1秒ごとの集計に使用する変数
        static auto lastLogTime = Clock::now();
        static double totalPoseMs = 0.0;
        static double totalUpdateMs = 0.0;
        static unsigned int frameCount = 0;

        // Update開始から現在までの時間を計算
        // ※ updateStart は関数冒頭に追加する
        const double updateMs =
            std::chrono::duration<double, std::milli>(
                Clock::now() - updateStart).count();

        // 今回の計測値を累積
        totalPoseMs += poseMs;
        totalUpdateMs += updateMs;
        ++frameCount;

        // 実際に1秒経過したか確認
        const auto now = Clock::now();

        if (now - lastLogTime >= std::chrono::seconds(1))
        {
            std::ostringstream oss;

            oss << std::fixed << std::setprecision(3)
                << "[Animator Profiling]"
                << "\n  Pose  : "
                << totalPoseMs / frameCount << " ms"
                << "\n  Total : "
                << totalUpdateMs / frameCount << " ms"
                << "\n  Nodes : "
                << m_globalTransforms.size()
                << "\n  Frames: "
                << frameCount << "\n";

            const std::string message = oss.str();
            OutputDebugStringA(message.c_str());

            // 次の集計期間へ
            totalPoseMs = 0.0;
            totalUpdateMs = 0.0;
            frameCount = 0;
            lastLogTime = now;
        }
    }

    //====================================================
    // ノードのローカル変換行列を計算
    //====================================================
    aiMatrix4x4 Animator::CalculateLocalTransform(const aiNode* node) const
{
    if (!m_animation)
        return node->mTransformation;

    const auto channelIt = m_animationChannelsByNode.find(node);
    if (channelIt == m_animationChannelsByNode.end())
        return node->mTransformation;

    const aiNodeAnim* channel = channelIt->second;
    aiVector3D scaling;
    aiQuaternion rotation;
    aiVector3D position;
    node->mTransformation.Decompose(scaling, rotation, position);

    position = InterpolateVector(
        channel->mPositionKeys, channel->mNumPositionKeys,
        m_animationTimeTicks, position);
    // Constant bind offsets must not be added to a different character's hips.
    if (channel->mNumPositionKeys > 0)
    {
        const auto offset = m_animationPositionOffsetsByNode.find(node);
        if (offset != m_animationPositionOffsetsByNode.end())
            position += offset->second;
    }
    scaling = InterpolateVector(
        channel->mScalingKeys, channel->mNumScalingKeys,
        m_animationTimeTicks, scaling);
    rotation = InterpolateRotation(
        channel->mRotationKeys, channel->mNumRotationKeys,
        m_animationTimeTicks, rotation);

    aiMatrix4x4 scaleMatrix;
    aiMatrix4x4 rotationMatrix;
    aiMatrix4x4 translationMatrix;
    aiMatrix4x4::Scaling(scaling, scaleMatrix);
    rotationMatrix = aiMatrix4x4(rotation.GetMatrix());
    aiMatrix4x4::Translation(position, translationMatrix);
    return translationMatrix * rotationMatrix * scaleMatrix;
}
void Animator::CacheNode(const aiNode* node)
    {
        m_nodesByName[CanonicalNodeName(node->mName.C_Str())] = node;
        m_globalTransforms.emplace(node, aiMatrix4x4());

        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            CacheNode(node->mChildren[i]);
        }
    }
    void Animator::CalculateNode(
        const aiNode* node,
        const aiMatrix4x4& parentTransform)
    {
        // ① このノードのローカル変換行列を計算
        aiMatrix4x4 localTransform =
            CalculateLocalTransform(node);

        // ② 親の変換行列と合成して、ワールド空間での変換行列を求める
        aiMatrix4x4 global =
            parentTransform * localTransform;

        // ③ 計算結果をノード名で保存する
        m_globalTransforms[node] = global;

        // ④ 子ノードも再帰的に計算する
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            CalculateNode(node->mChildren[i], global);
        }
    }

    //====================================================
    // ノード名からグローバル変換行列を取得
    //====================================================
    const aiMatrix4x4* Animator::GetGlobalTransform(
        const std::string& nodeName) const
    {
        // 指定された名前のノードが計算済みか検索する
        auto nodeIt = m_nodesByName.find(CanonicalNodeName(nodeName));
        if (nodeIt == m_nodesByName.end())
            return nullptr;

        auto transformIt = m_globalTransforms.find(nodeIt->second);
        if (transformIt == m_globalTransforms.end())
            return nullptr;

        return &transformIt->second;
    }

    bool Animator::GetModelSpaceTransform(
        const std::string& nodeName,
        aiMatrix4x4& transform) const
    {
        // 位置行列を計算
        const aiMatrix4x4* globalTransform = GetGlobalTransform(nodeName);
        if (globalTransform == nullptr)
            return false;

        transform = m_globalInverseTransform * (*globalTransform);
        return true;
    }
    //============================================================
    // ボーンごとの変形行列を計算する
    //============================================================
void Animator::GetSkinningMatrices(
    const SkinMesh& mesh,
    std::vector<aiMatrix4x4>& matrices) const
{
    matrices.resize(mesh.bones.size());

    const auto cachedNodesIt = m_skinBoneNodesByMesh.find(&mesh);
    for (size_t i = 0; i < mesh.bones.size(); ++i)
    {
        const SkinBone& bone = mesh.bones[i];
        const aiNode* boneNode = nullptr;

        if (cachedNodesIt != m_skinBoneNodesByMesh.end() &&
            i < cachedNodesIt->second.size())
        {
            boneNode = cachedNodesIt->second[i];
        }
        else
        {
            const auto nodeIt = m_nodesByName.find(CanonicalNodeName(bone.name));
            if (nodeIt != m_nodesByName.end())
                boneNode = nodeIt->second;
        }

        if (boneNode == nullptr)
        {
            matrices[i] = aiMatrix4x4();
            continue;
        }

        const auto transformIt = m_globalTransforms.find(boneNode);
        if (transformIt == m_globalTransforms.end())
        {
            matrices[i] = aiMatrix4x4();
            continue;
        }

        matrices[i] = m_globalInverseTransform
            * transformIt->second
            * bone.offsetMatrix;
    }
}
    aiVector3D Animator::DeformVertex(
        const SkinVertex& vertex,
        const std::vector<aiMatrix4x4>& boneMatrices) const
    {
        aiVector3D resultPosition(0.0f, 0.0f, 0.0f);
        float totalWeight = 0.0f;

        for (unsigned int j = 0; j < MAX_BONE_INFLUENCE; ++j)
        {
            const float weight = vertex.weights[j];
            if (weight <= 0.0f)
                continue;

            const unsigned int boneID = vertex.boneIDs[j];
            if (boneID >= boneMatrices.size())
                continue;

            resultPosition += (boneMatrices[boneID] * vertex.position) * weight;
            totalWeight += weight;
        }

        if (totalWeight > 0.0f)
            return resultPosition * (1.0f / totalWeight);

        return vertex.position;
    }

    std::vector<aiVector3D> Animator::DeformVertices(
        const SkinMesh& mesh,
        const std::vector<aiMatrix4x4>& boneMatrices) const
    {
        std::vector<aiVector3D> deformedVertices(mesh.vertices.size());
        for (size_t i = 0; i < mesh.vertices.size(); ++i)
            deformedVertices[i] = DeformVertex(mesh.vertices[i], boneMatrices);
        return deformedVertices;
    }
