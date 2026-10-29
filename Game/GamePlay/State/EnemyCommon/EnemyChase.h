#pragma once
#include "EnemyState.h"

class Enemy;

// í«ê’èÛë‘
class EnemyChase : public EnemyState
{
public:

	EnemyChase(Enemy* owner);
	~EnemyChase() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

