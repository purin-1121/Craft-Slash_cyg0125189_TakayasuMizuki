#pragma once
#include "BossState.h"

class Enemy;

// í«ê’èÛë‘
class BossDefeated : public BossState
{
public:

	BossDefeated(Enemy* owner);
	~BossDefeated() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

