#include "pch.h"
#include "PlayerJump.h"
#include "Game/GamePlay/Actor/Player.h"

PlayerJump::PlayerJump (Player* owner)
	: PlayerState(owner, "Jump")
	, m_afterjump{}
	, m_justPressed{}
	, m_jumpedTime{}
{
}

PlayerJump::~PlayerJump ()
{
}

void PlayerJump::Enter()
{
	m_jumpedTime = 0.0f;
	m_owner->PlayAnimation("Jump");
	m_owner->SetVelocity_Y(JUMP_POWER);
	m_owner->SetJumped(true);
	m_owner->Flying(GroundState::Fly);
}

void PlayerJump::Update(const GameContext& gameContext, float elapsedTime)
{
	if (m_owner->GetGround() == GroundState::Ground && m_owner->GetVelocity().y <= 0.0f)
	{
		m_owner->GetStateMachine().RequestChangeState<PlayerIdle>();
	}
	m_jumpedTime += elapsedTime;
}

void PlayerJump::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, m_owner->GetModel<PlayerAttack>());
}

void PlayerJump::Exit()
{
	
	m_afterjump = 0;
	m_justPressed = false;
	m_jumpedTime = 0;
	m_owner->SetJumped(true);
}

bool PlayerJump::OnInputAccept(Message::InputEvent input)
{
	switch (input)
	{
	case Message::InputEvent::MOVEUP:
	case Message::InputEvent::MOVEDOWN:
	case Message::InputEvent::MOVELEFT:
	case Message::InputEvent::MOVERIGHT:
		// Apply air control without leaving the jump state.
		PlayerWolk(m_owner).OnInputAccept(input);
		break;
	case Message::InputEvent::ATTACK:
		m_owner->GetStateMachine().RequestChangeState<PlayerAttack>();
		break;
	default:
		break;
	}

	return false;
}
