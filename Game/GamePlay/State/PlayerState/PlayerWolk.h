//--------------------------------------------------------------------------------------
// File: PlayerWolk .h
//
// プレイヤーの攻撃状態
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/PlayerState/PlayerState.h"
class Player;

class PlayerWolk : public PlayerState
{

public:

	PlayerWolk (Player* owner);
	~PlayerWolk () override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	void Exit() override;

	bool OnInputAccept(Message::InputEvent input) override;

private:

	// 移動中と判断する最低速度
	static constexpr float MIN_MOVEMENT_SPEED = 3.00f;

};

