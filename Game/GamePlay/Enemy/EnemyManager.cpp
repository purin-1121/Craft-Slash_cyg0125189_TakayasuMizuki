#include "pch.h"
#include "EnemyManager.h"
#include "Lib/Camera.h"

EnemyManager::EnemyManager(Player* pPlayer)
	: m_pPlayer{pPlayer}
	, m_gameContext{}
{
}

void EnemyManager::Initialize(const GameContext& gameContext)
{
	m_gameContext = &gameContext;
	CreateEnemy("BossA", SimpleMath::Vector3{ 10.0f, 0.0f, 10.0f }, 100);

	for (auto& e : m_enemies)
	{
		e->Initialize(gameContext);
	}
}

void EnemyManager::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	for (auto& e : m_enemies)
	{
		e->Update(elapsedTime, gameContext, camera);
	}
}

void EnemyManager::Render(const GameContext& gameContext, Camera& camera)
{
	for (auto& e : m_enemies)
	{
		e->Render(gameContext, camera.GetCameraView());
	}
}

void EnemyManager::RenderText()
{
	for (auto& e : m_enemies)
	{
		e->RenderText();
	}
}

void EnemyManager::Finalize()
{
	for (auto& e : m_enemies)
	{
		e->Finalize();
	}
}


void EnemyManager::CreateEnemy(const std::string& name, SimpleMath::Vector3 pos, int id)
{
	auto e = m_of.CreateForJson<Enemy>(name, m_pPlayer, pos, id);
	//e->SetDieCallback(m_onDeath);
	e->Initialize(*m_gameContext);
	m_enemies.push_back(std::move(e));
}
