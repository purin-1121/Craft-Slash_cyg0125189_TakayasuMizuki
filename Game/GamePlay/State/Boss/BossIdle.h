#pragma once
#include "BossState.h"
#include "Message/EventBus.h"

class Boss;

// Ç–ÇÈÇ›èÛë‘
class BossIdle : public BossState
{
public:

	BossIdle(Enemy* owner);
	~BossIdle() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

