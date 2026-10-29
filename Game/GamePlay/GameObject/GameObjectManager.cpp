#include "pch.h"
#include "GameObjectManager.h"
#include "Lib/Camera.h"
#include "Common/UI/Canvas.h"
#include "Common/UI/Bar/Bar.h"
#include "Common/Scene/SceneManager.h"
#include "Common/Model/ObjectManager.h"
#include "Common/Manager/StorageManager.h"
#include "Common/Manager/InputManager.h"
#include "Game/GamePlay/Blocks/Earth.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Factory/DropFactory.h"

using SimpleMath::Vector3;

GameObjectManager::GameObjectManager(InputGroup* group)
    : InputGroup(group)
    , m_Objects{}
    , m_player{}
    , m_goal{}
    , m_of{}
    , m_itemManager{std::make_unique<ItemManager>(2000) }
    , m_worldHeightMap{10}
    , m_enemyManager{m_player}
    , m_spawnAreaManager{&m_enemyManager}
{
}

GameObjectManager::~GameObjectManager()
{
}

void GameObjectManager::Initialize(Canvas& canvas, const GameContext& gameContext)
{
    m_itemManager->Initialize(canvas, gameContext);

    // ---- プレイヤーの作成・設定 ---- //
    auto player = std::make_unique<Player>(this);
    m_player = player.get();

    m_Objects.push_back(std::move(player));

    // ---- スポーンエリアの作成・設定 ---- //
    m_spawnAreaManager.Initialize();
    
    m_enemyManager.SetPlayer(m_player);
    m_enemyManager.Initialize(gameContext);
    

    // 死亡時呼び出し関数
    DropFactory::GetInstance().CreateDropMethod(m_itemManager.get());

	m_enemyManager.RegisterOnDeath(DropFactory::GetInstance().GetDropMethod());


    // ---- 宝箱の作成 ---- //
    std::vector<std::unique_ptr<GameObject>> list;
    for (int i = 0; i < 5; i++)
    {
        float x = Random::Range(-10.0f, 10.0f);
        float z = Random::Range(-10.0f, 10.0f);
        auto chest = m_of.Create<Chest>(SimpleMath::Vector3{x, 0, z}, DropFactory::GetInstance().GetInfo("Chest"), this);
        chest.get()->SetCallBack(DropFactory::GetInstance().GetDropMethod());
        chest.get()->SetInputParent(this);
        list.push_back(std::move(chest));
    }
    for (auto& object : list)
    {
        m_Objects.push_back(std::move(object));
    }

    for (const auto& obj : m_Objects)
    {
        obj->Initialize(gameContext);
    }

    // 地形マップの初期化
    m_worldHeightMap.Initialize(gameContext);

}

void GameObjectManager::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
    for (const auto& o : m_Objects)
    {
        o->Update(elapsedTime, gameContext, camera);
    }
    m_enemyManager.Update(elapsedTime, gameContext, camera);
    m_spawnAreaManager.Update(elapsedTime, gameContext, camera);
    m_itemManager->Update(elapsedTime, m_player->GetPosition(), gameContext, camera);


    CollisionManager::GetInstance()->ColliderCheck();

    // 動的オブジェクトを取得
    for (auto& object : PhysicsUtility::GetInstance()->GetObjects())
    {
        // プレイヤーの位置
        SimpleMath::Vector3 p = object->GetPosition();
        // ハイトマップから高さを取得
        float height = m_worldHeightMap.GetWorldHeight(p.x, p.z);
        // 滑るかどうか
        bool slide = m_worldHeightMap.IsSlide(p.x, p.z);
        // 頂点の下にいたら
        if (p.y < height)
        {
            // 滑るなら
            if (slide)
            {
                // 交点のある面の法線を取得
                SimpleMath::Vector3 normal = m_worldHeightMap.GetNormal(p.x, p.z);
                // 押し戻し量を計算
                float penetrationY = height - p.y;
                // 面に垂直な最短距離だけ押し戻す
                float pushLength = penetrationY * normal.y;
                p += normal * pushLength;
            }
            else
            {
                // 滑らない床なら、強制的にY座標を合わせる
                p.y = std::max(p.y, height);
            }
            // 滑らない状態の時設置判定
            object->Flying(GroundState::Ground);
        }

        // 位置と頂点の高さを設定
        object->SetPosition(p);
		object->SetHeight(height);
    }

    // マップにプレイヤー位置保存
    m_worldHeightMap.SetPlayerPos(m_player->GetPosition());

    // カメラのアップデート
    camera.SetChasePos(m_player->GetCenter(), Camera::ChasePatern::Object);
}

void GameObjectManager::Render(const GameContext& gameContext, Camera& camera)
{
}

void GameObjectManager::RenderText()
{
    for (const auto& obj : m_Objects)
    {
        obj->RenderText();
    }
    m_enemyManager.RenderText();
    m_itemManager->RenderText();
}

void GameObjectManager::RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)
{
    gameContext;
    view;
}

void GameObjectManager::Finalize()
{
    for (auto& obj : m_Objects)
    {
        obj->Finalize();
    }
    m_enemyManager.Finalize();
    m_spawnAreaManager.Finalize();
    m_itemManager->Finalize();
    DeleteObservers();

    m_Objects.clear();
    m_goal = nullptr;
    m_player = nullptr;
}


