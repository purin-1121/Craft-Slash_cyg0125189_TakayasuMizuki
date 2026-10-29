#pragma once

#include "Game/GamePlay/Data/SpawnAreaData.h"
#include "EnemyManager.h"

class SpawnArea
{
public:

	SpawnArea(EnemyManager* pManager, SimpleMath::Vector3 pos, const SpawnAreaData& data);

	void Initialize();
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera);
	void Render(const GameContext& gameContext, Camera& camera);
	void RenderText();
	void Finalize();

	SimpleMath::Vector3 GetSpawnPosition();
	void CompleteSpanw() { m_spawnTime = 0; m_canSpawn = false; m_enemyCount++; };
	void EnemyDeath() { m_enemyCount--; }
	bool CanSpawn() { return m_canSpawn; }
	int GetID() { return m_id; }
	float GetSpawnTime() { return m_spawnTime; }
	const std::string& GetRandomType();
	SimpleMath::Vector3 GetPosition() { return m_position; }

private:

	static constexpr float SPAWN_INTERVAL = 5.0f;	// 通常時の敵のわく間隔
	static constexpr float INIT_INTERVAL = 1.0f;	// 最初に敵が全部わくまでの間隔
	static constexpr int MAX_ENEMIES = 4;			// 一つのエリアでわく最大数
	static constexpr SimpleMath::Vector2 SPAWN_RANGE = {3.0f, 3.0f};	// 敵がわく範囲

	EnemyManager* m_pEnemyManager;
	SimpleMath::Vector3 m_position;

	float m_spawnTime;		// わきの計測時間
	bool m_canSpawn;		// 敵をわかせてもいいか
	bool m_isMaxSpawned;	// すでに最大までわいたか
	int m_enemyCount;		// 敵の出現数
	int m_id;				// スポーンエリアの識別番号

	const SpawnAreaData* m_data;

	static int id;

};

