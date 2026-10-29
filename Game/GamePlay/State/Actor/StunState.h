#pragma once
#include "Game/GamePlay/State/State.h"

class Actor;

// Ç–ÇÈÇ›èÛë‘
class Stun : public State<Actor>
{
public:

	Stun(Actor* owner);
	~Stun() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

private:

	static constexpr float STUN_TIME = 0.5f;
	float m_stunTime;

};

