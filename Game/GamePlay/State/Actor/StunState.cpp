#include "pch.h"
#include "StunState.h"
#include "Game/GamePlay/Actor/Actor.h"
#include "Game/GamePlay/Actor/Player.h"

Stun::Stun(Actor* owner)
	: State(owner, "Stun")
	, m_stunTime{}
{
}

Stun::~Stun()
{
}

void Stun::Enter()
{
	m_owner->PlayAnimation("Stun", false);

	// –³“G•t—^
	m_owner->InvisibleOn(1.5f);
}

void Stun::Update(const GameContext& gameContext, float elapsedTime)
{
	auto& stateMachine = m_owner->GetStateMachine();
	stateMachine.AddTimer(elapsedTime);
	// Œ»Ý‚ÌƒXƒ^ƒ“ŽžŠÔ‚ªÅ‘å•b”‚ð’´‚¦‚Ä‚¢‚½‚ç
	if (stateMachine.GetTimer() > STUN_TIME)
	{
		// ‘Ò‹@ó‘Ô‚Ö
		m_owner->GetStateMachine().RequestChangeState(m_owner->GetAfterID());
	}
	gameContext;
	elapsedTime;
}

void Stun::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void Stun::Exit()
{
	m_owner->GetStateMachine().ResetTimer();
}
