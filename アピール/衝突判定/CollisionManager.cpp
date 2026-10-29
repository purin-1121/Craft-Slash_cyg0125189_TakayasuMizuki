#pragma once

#include "pch.h"
#include "CollisionManager.h"
#include "Game/GamePlay/GameObject/GameObject.h"
#include <tchar.h>
#include "CollisionMatrix.h"
#include "NarrowPhase.h"
#include "CollisionResolver.h"

CollisionManager* CollisionManager::m_collsionManager = nullptr;

void CollisionManager::ColliderCheck()
{
    m_checkedPairs.clear();

    // コライダーの更新と八分木の更新
    for (auto& c : m_colliders)
    {

        c->m_owner->UpdateCollider();

        if (c->IsDirty())
        {
            m_octree->Update(c);
        }
    }

    // ツリー取得
    auto& tree = m_octree->GetTree();
    // 親ノード内のオブジェクト
    std::vector<OctreeObject> list{};

    for (uint16_t index = 0; index < tree.size(); ++index)
    {
        // ルートがなかったらスキップ
        if (tree[index] == nullptr)
            continue;

        auto& objects = tree[index]->objects;

        //---------------------------------------------
        // 同じNode内のオブジェクト同士
        //---------------------------------------------
        for (int i = 0; i < objects.size(); ++i)
        {
            if (!objects[i].col->GetEnable())
                continue;

            for (int j = i + 1; j < objects.size(); ++j)
                CollisionHelper(objects[i].col,objects[j].col);
        }

        //---------------------------------------------
        // 候補リストと
        //---------------------------------------------
        // 祖先ノードのオブジェクトと
        for (int anc = index; anc != 0; )
        {
            anc = (anc - 1) / 8;
            if (!tree[anc]) continue;

            for (auto& o : objects)
            {
                if (!o.col->GetEnable()) continue;
                for (auto& p : tree[anc]->objects)
                    CollisionHelper(o.col, p.col);
            }
        }
    }

    // トリガー抜けの処理
    for (auto& pair : m_prevPairs)
    {
        if (!m_nowPairs.contains(pair))
        {
            pair.a->m_owner->OnTriggerExit(pair.a, pair.b, pair.b->m_owner);
            pair.b->m_owner->OnTriggerExit(pair.b, pair.a, pair.a->m_owner);
        }
    }

    m_prevPairs = std::move(m_nowPairs);
    m_nowPairs.clear();
}

void CollisionManager::Update()
{
    for (auto& c : m_colliders)
    {
        c->m_owner->UpdateCollider();
    }
}

void CollisionManager::TriggerMethod(Collider* a, Collider* b, GameObject* objA, GameObject* objB, CollisionInfo& info)
{
    // 接触したオブジェクトペアの登録
    CollisionPair p = { a, b };
    m_nowPairs.emplace(p);

    // 未だ接触していなかったら
    if (!m_prevPairs.contains(p))
    {
        objA->OnTriggerEnter(a, b, objB, info);
        objB->OnTriggerEnter(b, a, objA, info);
    }
    else
    {
        objA->OnTrigger(a, b, objB, info);
        objB->OnTrigger(b, a, objA, info);
    }
}

void CollisionManager::CollisionMethod(Collider* a, Collider* b, GameObject* objA, GameObject* objB, CollisionInfo& info)
{

    auto aPush = info.push * (objA->IsKinematic() ? (objB->IsKinematic() ? objB->GetMass() / (objA->GetMass() + objB->GetMass()) : 1) : 0);
    auto bPush = aPush - info.push;

    // 動的オブジェクトなら位置補正
    if (objA->IsKinematic())
    {
        objA->AddPosition(aPush);

        // オブジェクトの速度
        SimpleMath::Vector3 vA = objA->GetVelocity();
        // オブジェクトにまっすぐにぶつかる向き
        float dotA = vA.Dot(info.normal);
        // Bの方向に向かっているときだけ速度を調整
        if (dotA < 0.0f)
        {
            objA->SetVelocity(vA - info.normal * dotA);
        }
    }
    objA->OnCollision(a, b, objB, info);
    if (objB->IsKinematic())
    {
        objB->AddPosition(bPush);

        // オブジェクトの速度
        SimpleMath::Vector3 vB = objB->GetVelocity();
        // オブジェクトにまっすぐにぶつかる向き
        auto normalB = -info.normal;
        float dotB = vB.Dot(normalB);
        // Bの方向に向かっているときだけ速度を調整
        if (dotB < 0.0f)
        {
            objB->SetVelocity(vB - normalB * dotB);
        }
    }
    objB->OnCollision(b, a, objA, info);
}

void CollisionManager::CollisionHelper(Collider* a, Collider* b)
{
    // 無効状態ならスキップ
    if (!a->GetEnable() || !b->GetEnable())
        return;

    // 無視レイヤーならスキップ
    if (!CollisionMatrix::CanCollide(a->GetLayer(), b->GetLayer()))
        return;

    // コライダーペア
    CollisionPair pair{ a, b };
    // すでに接触済みだったらスキップ
    if (m_checkedPairs.contains(pair))
        return;

    // 衝突情報
    CollisionInfo info{};

    // 衝突していなかったらスキップ
    info = NarrowPhase::IsColliding(a, b);
    if (!info.is)
        return;

    // コライダーの親
    GameObject* objA = a->GetObj();
    GameObject* objB = b->GetObj();

    if (a->IsTrigger() || b->IsTrigger())
    {
        TriggerMethod(a, b, objA, objB, info);
    }
    else
    {
        CollisionMethod(a, b, objA, objB, info);
    }
}





