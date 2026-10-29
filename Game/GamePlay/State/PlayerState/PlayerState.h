//--------------------------------------------------------------------------------------
// File:public PlayerState .h
//
// ÉvÉåÉCÉÑÅ[ÇÃçUåÇèÛë‘
//
// Date: 2026.4.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include "Game/GamePlay/State/State.h"
#include "InterFace/IInputObserver.h"
class Player;

class PlayerState : public State<Player>, public IInputObserver
{

public:

	PlayerState (Player* owner, const std::string& stateName);
	~PlayerState () override;

	virtual void Enter() override = 0;
	virtual void Update(const GameContext& gameContext, float elapsedTime) override = 0;
	virtual void Render(const GameContext& gameContext, const SimpleMath::Matrix& view) override = 0;
	void RenderText(const GameContext&, const SimpleMath::Matrix&) {};
	virtual void Exit() override = 0;

	bool OnInputAccept(Message::InputEvent input) override;
	virtual bool CanChanged() override { return true; };


};

