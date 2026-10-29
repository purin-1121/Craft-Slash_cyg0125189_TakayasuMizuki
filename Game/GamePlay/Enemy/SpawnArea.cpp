#include "pch.h"
#include "SpawnArea.h"

int SpawnArea::id = 0;

SpawnArea::SpawnArea(EnemyManager* pManager, SimpleMath::Vector3 pos, const SpawnAreaData& data)
	: m_pEnemyManager{pManager}
	, m_position{pos}
	, m_spawnTime{}
	, m_enemyCount{}
	, m_isMaxSpawned{}
	, m_canSpawn{}
	, m_data{&data}
{
	m_id = id++;
}

void SpawnArea::Initialize()
{
	m_spawnTime = 0.0f;
	m_enemyCount = 0;
	m_isMaxSpawned = false;
	m_canSpawn = false;

	m_data;
}

void SpawnArea::Update(float elapsedTime, const GameContext& gameContext, Camera&)
{
	if (!m_canSpawn && m_enemyCount < MAX_ENEMIES)
	{
		m_spawnTime += elapsedTime;
		if (m_spawnTime > (m_isMaxSpawned ? SPAWN_INTERVAL : INIT_INTERVAL))
			m_canSpawn = true;

		if (m_enemyCount >= MAX_ENEMIES)
			m_isMaxSpawned = true;
	}
}

void SpawnArea::Finalize()
{

}

SimpleMath::Vector3 SpawnArea::GetSpawnPosition()
{
	float x = Random::Range(0.0f, SPAWN_RANGE.x);
	float z = Random::Range(0.0f, SPAWN_RANGE.y);

	return SimpleMath::Vector3(m_position.x + x, m_position.y, m_position.z + z);
}

const std::string& SpawnArea::GetRandomType()
{
	int r = 0;
	
	if (m_data->enemyTypes.size() > 1)
		r = Random::Range(0, m_data->enemyTypes.size() - 1);

	return m_data->enemyTypes[r];
}
