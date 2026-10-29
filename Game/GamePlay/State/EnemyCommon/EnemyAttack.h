#pragma once
#include "EnemyState.h"

class Enemy;

// Ç–ÇÈÇ›èÛë‘
class EnemyAttack : public EnemyState
{
public:

	EnemyAttack(Enemy* owner);
	~EnemyAttack() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};

	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

