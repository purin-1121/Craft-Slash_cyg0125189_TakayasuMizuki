//--------------------------------------------------------------------------------------
// File: Fall.h
//
// Ç®ÇøÇƒÇ¢ÇÈèÛë‘
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/State.h"

class Fall :public State<GameObject>
{

public:

	// óéâ∫ç≈çÇë¨ìx
	static constexpr float FALL_SPEED = 0.8f;

public:

	Fall (GameObject* owner);
	~Fall () override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;


};

