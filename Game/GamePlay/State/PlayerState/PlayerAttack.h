//--------------------------------------------------------------------------------------
// File: PlayerAttack .h
//
// ÉvÉåÉCÉÑÅ[ÇÃçUåÇèÛë‘
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/PlayerState/PlayerState.h"

class Player;

class PlayerAttack :public PlayerState
{

public:

	PlayerAttack (Player* owner);
	~PlayerAttack () override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	bool CanChanged() override;

};

