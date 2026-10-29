#include "pch.h"
#include "PlayerIdle.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Game/GamePlay/State/Actor/StunState.h"

PlayerIdle::PlayerIdle(Player* owner)
	: PlayerState(owner, "Idle")
	, m_debugTime{}
{
}

PlayerIdle::~PlayerIdle()
{
}

void PlayerIdle::Enter()
{
	if (m_owner->GetBoneName() != "mixamorig6:Hips")
	{
		m_owner->SetBoneName("mixamorig6:RightHand");
		m_owner->GetWeapon().SetOffsetOfState("Idle");
	}

	// 地面にいたら歩行アニメーション
	if (m_owner->GetGround() == GroundState::Ground)
		m_owner->PlayAnimation("PlayerIdle");
}

void PlayerIdle::Update(const GameContext& gameContext, float elapsedTime)
{
	// 地面にいたら待機アニメーション
	if (m_owner->GetGround() == GroundState::Ground &&
		m_owner->GetAnimator().GetModel() != ObjectLib::m_animatedModels["PlayerIdle"].get())
		m_owner->PlayAnimation("PlayerIdle");

	auto& machine = m_owner->GetStateMachine();
	machine.AddTimer(elapsedTime);
	if (machine.GetTimer() > 5.0f && m_owner->Holding())
	{
		m_owner->GetWeapon().SetOffsetOfState("Run");
		m_owner->SetBoneName("mixamorig6:Hips");
	}
}

void PlayerIdle::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, m_owner->GetModel<PlayerAttack>());
}

void PlayerIdle::Exit()
{
	m_owner->GetStateMachine().ResetTimer();
}

bool PlayerIdle::OnInputAccept(Message::InputEvent input)
{
	switch (input)
	{
	case Message::InputEvent::MOVEUP:
	case Message::InputEvent::MOVEDOWN:
	case Message::InputEvent::MOVELEFT:
	case Message::InputEvent::MOVERIGHT:
		m_owner->GetStateMachine().RequestChangeState<PlayerWolk>();
		break;
	case Message::InputEvent::JUMP:
		m_owner->GetStateMachine().RequestChangeState<PlayerJump>();
		break;
	case Message::InputEvent::ATTACK:
		m_owner->GetStateMachine().RequestChangeState<PlayerAttack>();
		break;
	default:
		break;
	}
	return false;
}
