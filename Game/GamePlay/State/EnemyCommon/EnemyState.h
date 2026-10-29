#pragma once
#include "Game/GamePlay/State/State.h"

class Enemy;

// Ç–ÇÈÇ›èÛë‘
class EnemyState : public State<Enemy>
{
public:

	EnemyState(Enemy* owner, const std::string& stateName);
	~EnemyState() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	virtual bool CanChanged() override { return true; };

};

