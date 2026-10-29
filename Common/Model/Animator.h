#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ModelResource.h"

class Animator
{
public:
    // 共有モデルを設定する
    void SetModel(std::shared_ptr<ModelResource> model);

    // 再生するアニメーションを指定する
    bool Play(unsigned int animationIndex = 0, bool isLoop = true);

    // 毎フレーム呼び出す
    void Update(float deltaTime);

    // 再生中のアニメーションモデルを取得
    ModelResource* GetModel() { return m_model.get(); }

    // ノード名から計算済みのグローバル行列を取得する
    const aiMatrix4x4* GetGlobalTransform(const std::string& nodeName) const;

    // アニメーションが終了しているかどうか
    bool IsEnd() { return m_isEnd; }

    // 再生速度変更
    void SetMultiplyer(float mult) { m_multiplier = mult; }

    bool GetModelSpaceTransform(
        const std::string& nodeName,
        aiMatrix4x4& transform) const;

    float GetTime() const { return m_time; }

    // SkinMeshの各ボーンに対応する変形行列を取得する
    void GetSkinningMatrices(const SkinMesh& mesh, std::vector<aiMatrix4x4>& matrices) const;

    // スキニング後の頂点座標を計算する
    // 元の頂点は変更せず、変形後の座標だけを返す
    aiVector3D DeformVertex(
        const SkinVertex& vertex,
        const std::vector<aiMatrix4x4>& boneMatrices) const;

    std::vector<aiVector3D> DeformVertices(
        const SkinMesh& mesh,
        const std::vector<aiMatrix4x4>& boneMatrices) const;


private:
    // ノードのアニメーションを再帰的に計算する
    void CacheNode(const aiNode* node);
    void CalculateNode(
        const aiNode* node,
        const aiMatrix4x4& parentTransform);

    // 指定時刻のアニメーション姿勢を計算する
    aiMatrix4x4 CalculateLocalTransform(
        const aiNode* node) const;

    std::shared_ptr<ModelResource> m_model;
    const aiAnimation* m_animation = nullptr;

    // このAnimator専用の再生時間
    float m_time = 0.0f;
    double m_animationTimeTicks = 0.0;

    // ノードごとのグローバル変換行列
    std::unordered_map<const aiNode*, aiMatrix4x4> m_globalTransforms;
    std::unordered_map<std::string, const aiNode*> m_nodesByName;
    std::unordered_map<const aiNode*, const aiNodeAnim*> m_animationChannelsByNode;
    std::unordered_map<const aiNode*, aiVector3D> m_animationPositionOffsetsByNode;
    std::unordered_map<const SkinMesh*, std::vector<const aiNode*>> m_skinBoneNodesByMesh;


    // ルートノードの変換を打ち消す行列
    aiMatrix4x4 m_globalInverseTransform;

    // アニメーションが終了しているか
    bool m_isEnd = false;
    // このアニメーションはループするものか
    bool m_isLoop = true;

    // アニメーション再生速度
    float m_multiplier = 1.0f;

};

