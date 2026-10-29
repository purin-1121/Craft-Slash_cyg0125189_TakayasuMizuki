#pragma once
#include "Game/GamePlay/State/State.h"

class Enemy;

// Ç–ÇÈÇ›èÛë‘
class BossState : public State<Enemy>
{
public:

	BossState(Enemy* owner, const std::string& stateName);
	~BossState() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

