//====================================================//
// ファイル名   : CollisionManager.h
// 作成者       : 高安瑞貴
//
// 概要 : 衝突判定を制御するクラスを管理するヘッダ
//====================================================//

#pragma once

//====================================================//
// インクルードファイル
//====================================================//
#include "Collider.h"
#include <unordered_set>
#include "Octree.h"

//====================================================//
// 宣言
//====================================================//




// このゲームの物理挙動を管理するマネージャ
class CollisionManager
{
public:

	void AddCollider(Collider* c)
	{
		m_colliders.push_back(c);
	}

    void RemoveCollider(Collider* c)
    {
        if (!c) return;
        // Octree traversal may still hold this collider until cleanup.
        c->SetEnable(false);
        std::erase(m_colliders, c);
    }

    // Called when deleting a collider, outside collision traversal.
    void RemoveDestroyedCollider(Collider* c)
    {
        RemoveCollider(c);
        for (auto& node : m_octree->GetTree())
        {
            if (node)
                std::erase_if(node->objects, [c](const OctreeObject& object)
                    { return object.col == c; });
        }
        const auto referencesCollider = [c](const CollisionPair& pair)
            { return pair.a == c || pair.b == c; };
        std::erase_if(m_prevPairs, referencesCollider);
        std::erase_if(m_nowPairs, referencesCollider);
        std::erase_if(m_checkedPairs, referencesCollider);
    }
	void ColliderCheck();

	void RenderCollider()
	{
		std::vector<Collider*> debug;
		for (const auto& c : m_colliders)
		{
			if (!c->GetEnable())
				continue;
			c->Draw();
			debug.push_back(c);
		}

		debug;
	}

	static CollisionManager* GetInstance()
	{
		if (m_collsionManager == nullptr)
		{
			m_collsionManager = new CollisionManager();
		}
		return m_collsionManager;
	}

	void Reset()
	{
		m_octree->Finalize();
		m_colliders.clear();
		m_prevPairs.clear();
		m_nowPairs.clear();
	}

	std::vector<Collider*> GetColliders() { return m_colliders; }

private:

	// ゲームのコライダーたち
	std::vector<Collider*> m_colliders;

	// 前フレーム衝突していたか
	std::unordered_set<CollisionPair, CollisionPairHash> m_prevPairs;
	// このフレーム衝突しているか
	std::unordered_set<CollisionPair, CollisionPairHash> m_nowPairs;

	// 自身のポインタ
	static CollisionManager* m_collsionManager;

	// 判定用Octree
	std::unique_ptr<Octree> m_octree;

	// このフレームの衝突管理用セットマップ（空間分割用）
	std::unordered_set<CollisionPair, CollisionPairHash> m_checkedPairs;

	CollisionManager()
		: m_colliders{}
		, m_octree{std::make_unique<Octree>()}
	{}
	
	void Update();

	// トリガー衝突
	void TriggerMethod(Collider* a, Collider* b, GameObject* objA, GameObject* objB, CollisionInfo& info);
	// コリジョン衝突
	void CollisionMethod(Collider* a, Collider* b, GameObject* objA, GameObject* objB, CollisionInfo& info);
	// 衝突検査ヘルパー
	void CollisionHelper(Collider* a, Collider* b);

	// ------ デバッグ用 ------ //
	
	// ---------------------- //

};