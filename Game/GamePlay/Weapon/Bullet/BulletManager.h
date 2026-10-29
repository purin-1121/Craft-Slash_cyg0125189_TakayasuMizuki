#pragma once

#include "Bullet.h"
#include "Game/GamePlay/Data/BulletData.h"
#include "Common/Manager/Resources/JsonDataManager.h"

class BulletManager
{
public:

	void Shoot(SimpleMath::Vector3 pos, SimpleMath::Vector3 vec, std::string name, ColliderTag tag);

	void Initialize(const GameContext& gameContext);
	void Update(float elapsedTime, const GameContext&, Camera&);
	void Render(const GameContext& gameContext, SimpleMath::Matrix view);
	void Finalize();

	BulletManager();
	~BulletManager();

	static constexpr int BULLET_NUM = 100;

private:

	std::vector<std::unique_ptr<Bullet>> m_bullets;

};

