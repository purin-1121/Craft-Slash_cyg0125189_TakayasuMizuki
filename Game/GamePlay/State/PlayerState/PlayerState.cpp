#include "pch.h"
#include "PlayerState.h"
#include "Game/GamePlay/Actor/Player.h"

PlayerState::PlayerState (Player* owner, const std::string& stateName)
	: State<Player>(owner, stateName)
	, IInputObserver(owner)
{
	owner->GetStateMachine().AddObserver(this);
}

PlayerState::~PlayerState ()
{
}

void PlayerState::Enter()
{
}

void PlayerState::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	elapsedTime;
}

void PlayerState::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void PlayerState::Exit()
{
}

bool PlayerState::OnInputAccept(Message::InputEvent input)
{
	return false;
}
