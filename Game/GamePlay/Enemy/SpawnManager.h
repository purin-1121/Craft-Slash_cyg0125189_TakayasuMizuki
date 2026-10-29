#pragma once

#include "SpawnArea.h"
#include "Message/EventBus.h"

class SpawnManager
{

public:

	SpawnManager(EnemyManager* pManager);
	~SpawnManager();

	void Initialize();
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera);
	void Render(const GameContext& gameContext, Camera& camera);
	void RenderText();
	void RenderEdit();
	void Finalize();

private:

	EnemyManager* m_pEnemyManager;
	ObjectFactory m_of;
	std::vector<std::unique_ptr<SpawnArea>> m_areas;

	SubsprictionID m_defeatSubscriptionID{};
	SubsprictionID m_setAreaSubscriptionID{};
	SubsprictionID m_saveAreaSubscriptionID{};
	bool m_eventsSubscribed = false;

	void LoadArea();
	void SaveArea();



};

