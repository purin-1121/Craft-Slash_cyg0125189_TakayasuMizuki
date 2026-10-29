#pragma once
#include "Game/GamePlay/State/State.h"

class Actor;

// Ç–ÇÈÇ›èÛë‘
class Cool : public State<Actor>
{
public:

	Cool(Actor* owner);
	~Cool() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

private:

	static constexpr float Cool_TIME = 0.5f;
	float m_CoolTime;

};

