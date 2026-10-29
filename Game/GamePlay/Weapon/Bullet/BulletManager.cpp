#include "pch.h"
#include "BulletManager.h"
#include "Lib/Camera.h"

BulletManager::BulletManager()
	: m_bullets{}
{
	m_bullets.reserve(BULLET_NUM);
	for (int i = 0; i < BULLET_NUM; i++)
	{
		m_bullets.push_back(std::make_unique<Bullet>());	
	}
}



BulletManager::~BulletManager()
{
	
	m_bullets.clear();
}

void BulletManager::Initialize(const GameContext& gameContext)
{
	for (auto& b : m_bullets)
	{
		b->Initialize(gameContext);
	}
}

void BulletManager::Update(float elapsedTime, const GameContext& gameContext, Camera& camera)
{
	for (auto& b : m_bullets)
	{
		if(b->IsActive())
			b->Update(elapsedTime, gameContext, camera);
	}
}

void BulletManager::Render(const GameContext& gameContext, SimpleMath::Matrix view)
{
	for (auto& b : m_bullets)
	{
		if (b->IsActive())
			b->Render(gameContext, view);
	}
}

void BulletManager::Finalize()
{
	for (auto& b : m_bullets)
	{
		b->Finalize();
	}
}


void BulletManager::Shoot(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, std::string name, ColliderTag tag)
{
	for (auto& b : m_bullets)
	{
		if (b->IsActive())
			continue;

		auto& bulletData = JsonDataManager<BulletData>::GetInstance().GetDatas();
		b->AttackOn(pos, vec, JsonDataManager<BulletData>::GetInstance().GetData(name), tag);
		break;
	}
}
