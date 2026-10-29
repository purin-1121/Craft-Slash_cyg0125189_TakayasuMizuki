#pragma once
#include "BossAttackBase.h"

class Enemy;

// Ç–ÇÈÇ›èÛë‘
class SlashAttack : public BossAttackBase
{
public:

	SlashAttack(Enemy* owner);
	~SlashAttack() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

