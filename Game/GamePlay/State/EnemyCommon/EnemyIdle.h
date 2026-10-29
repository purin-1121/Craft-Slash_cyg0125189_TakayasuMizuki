#pragma once
#include "EnemyState.h"

class Enemy;

// Ç–ÇÈÇ›èÛë‘
class EnemyIdle : public EnemyState
{
public:

	EnemyIdle(Enemy* owner);
	~EnemyIdle() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

