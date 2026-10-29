#pragma once
#include "BossState.h"

class Boss;

// í«ê’èÛë‘
class BossChase : public BossState
{
public:

	BossChase(Enemy* owner);
	~BossChase() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

