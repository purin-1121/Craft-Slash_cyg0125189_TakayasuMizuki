#pragma once
#include "Game/GamePlay/Data/EnemyData.h"
#include "Game/GamePlay/State/EnemyCommon/EnemyState.h"
#include "Common/Manager/Resources/JsonDataManager.h"
#include <Game/GamePlay/Data/BossAttackData.h>

class EnemyStateFactory
{
public:

	std::unique_ptr<IGameObjectState> GetEnemyState(const std::string& enemyType,StateID stateID,Enemy* owner);

	static EnemyStateFactory* GetInstance()
	{
		if (s_EnemyStateFactory == nullptr)
		{
			s_EnemyStateFactory.reset(new EnemyStateFactory);
		}

		return s_EnemyStateFactory.get();
	}

	static EnemyStateFactory& GetRefarence()
	{
		if (s_EnemyStateFactory == nullptr)
		{
			s_EnemyStateFactory.reset(new EnemyStateFactory);
		}

		return *s_EnemyStateFactory.get();
	}

	static void Dispose()
	{
		s_EnemyStateFactory.reset();
	}

	~EnemyStateFactory() = default;

private:

	EnemyStateFactory()
	{};

	static std::unique_ptr<EnemyStateFactory> s_EnemyStateFactory;
};

