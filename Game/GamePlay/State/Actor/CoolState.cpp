#include "pch.h"
#include "CoolState.h"
#include "Game/GamePlay/Actor/Actor.h"

Cool::Cool(Actor* owner)
	: State(owner, "Cool")
	, m_CoolTime{}
{
}

Cool::~Cool()
{
}

void Cool::Enter()
{
}

void Cool::Update(const GameContext& gameContext, float elapsedTime)
{
	// Œ»Ý‚ÌƒXƒ^ƒ“ŽžŠÔ‚ªÅ‘å•b”‚ð’´‚¦‚Ä‚¢‚½‚ç
	if ((m_CoolTime += elapsedTime) > Cool_TIME)
	{
		// ‘Ò‹@ó‘Ô‚Ö
		m_owner->GetStateMachine().RequestChangeState(m_owner->GetIdleState());
	}
}

void Cool::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void Cool::Exit()
{
	m_CoolTime = 0;
}
