//--------------------------------------------------------------------------------------
// File: PlayerIdle.h
//
// プレイヤーの待機状態
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/PlayerState/PlayerState.h"

class Actor;

class PlayerIdle : public PlayerState
{

public:

	PlayerIdle(Player* owner);
	~PlayerIdle() override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	bool OnInputAccept(Message::InputEvent input) override;

private:

	// ---- デバッグ用 ---- //
	float m_debugTime;

};

