#include "pch.h"
#include "Fall.h"
#include "Game/GamePlay/Actor/Player.h"

Fall::Fall (GameObject* owner)
	: State(owner, "Fall")
{
}

Fall::~Fall ()
{
}

void Fall::Enter()
{
}

void Fall::Update(const GameContext& gameContext, float elapsedTime)
{
	/*if (m_jumpedTime > JUMP_TIME)
	{
		m_owner->GetStateMachine().RequestChangeState<PlayerIdle>()
	}*/
	gameContext;
	elapsedTime;
}

void Fall::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
}

void Fall::Exit()
{
}
