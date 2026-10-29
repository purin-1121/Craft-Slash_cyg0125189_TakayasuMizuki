#pragma once
#include "Tools/GameContext.h"
#include "Common/Collider/CollisionManager.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Game/GamePlay/Blocks/Goal.h"
#include "Game/GamePlay/Factory/ObjectFactory.h"
#include "Game/GamePlay/Item/ItemManager.h"
#include "Game/GamePlay/Maps/HeightMap.h"
#include "Game/GamePlay/Enemy/EnemyManager.h"
#include "Game/GamePlay/Enemy/SpawnManager.h"

class Canvas;

class GameObjectManager : public InputGroup
{

public:

	// ボックスの大きさ
	static constexpr float BOX_SIZE = 2.0f;
	// 地面の大きさ
	static constexpr SimpleMath::Vector2 GROUND_SIZE{10, 10};
	// グリッド数(辺)
	static constexpr int GRID = 4;

	GameObjectManager(InputGroup* group);
	~GameObjectManager();

	void Initialize(Canvas&, const GameContext& gameContext);
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera)  ;
	void Render(const GameContext& gameContext, Camera& camera)  ;
	void RenderText()  ;
	void RenderBounding(const GameContext& gameContext, SimpleMath::Matrix view)  ;
	void Finalize()  ;

	// ------ Get ------ //
	Player& GetPlayer() { return *m_player; }
	Goal& GetGoal() { return *m_goal; }
	HeightMap& GetHeightMap() { return m_worldHeightMap; }
	SpawnManager& GetSpawnManager() { return m_spawnAreaManager; }

	// ------ Set ------ //
	
	// ------ Add ------ //

	// ------ Other ------ //
	
	
private:

	// ---- 基本のパラメータ ---- //
	// ゲームオブジェクト
	std::vector<std::unique_ptr<GameObject>> m_Objects;
	// プレイヤー
	Player* m_player;
	// ゴール
	Goal* m_goal;
	// オブジェクトファクトリー
	ObjectFactory m_of;
	// アイテム
	std::unique_ptr<ItemManager> m_itemManager;
	// ワールドハイトマップ
	HeightMap m_worldHeightMap;
	// 敵マネージャ
	EnemyManager m_enemyManager;
	// スポーンエリアマネージャ
	SpawnManager m_spawnAreaManager;


	// ---- デバッグ用 ---- //

};

