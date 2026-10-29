//--------------------------------------------------------------------------------------
// File: PlayerJump .h
//
// プレイヤーの攻撃状態
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/PlayerState/PlayerState.h"
class Player;

class PlayerJump :public PlayerState
{
public:

	// ジャンプボタンが押されてるかどうか
	bool IsPressedJump() { return m_justPressed; }

public:

	// ジャンプできる最大秒数
	static constexpr float JUMP_TIME = 0.5f;
	// ジャンプ力(初速)
	static constexpr float JUMP_POWER = 1.5f;

public:

	PlayerJump (Player* owner);
	~PlayerJump () override;

	void Enter() override;
	void Update(const GameContext& gameContext, float elapsedTime) override;
	void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override;
	void RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view) {};
	void Exit() override;

	bool OnInputAccept(Message::InputEvent input) override;

private:

	// ---- ジャンプ用 ---- //
	// ジャンプした後か
	bool m_afterjump;
	// このフレームでジャンプしたか
	bool m_justPressed;
	// ジャンプし続けている秒数
	float m_jumpedTime;

};

