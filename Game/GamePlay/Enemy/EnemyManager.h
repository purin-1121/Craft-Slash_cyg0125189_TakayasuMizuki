#pragma once
#include "Enemy.h"
#include <Game/GamePlay/Factory/ObjectFactory.h>

class EnemyManager
{

public:

	EnemyManager(Player* pPlayer);

	void Initialize(const GameContext& gameContext);
	void Update(float elapsedTime, const GameContext& gameContext, Camera& camera);
	void Render(const GameContext& gameContext, Camera& camera);
	void RenderText();
	void Finalize();

	void CreateEnemy(const std::string& name, SimpleMath::Vector3 pos, int id);

	void RegisterOnDeath(std::function<void(GameObject*, const Drop*, int, int)> f) { m_onDeath = f; };

	void SetPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }

private:

	// 敵
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	// 死んだときの呼び出し関数
	std::function<void(GameObject*, const Drop*, int, int)> m_onDeath;
	// プレイヤーのポインタ
	Player* m_pPlayer;
	// オブジェクトファクトリー
	ObjectFactory m_of;
	const GameContext* m_gameContext;

};

