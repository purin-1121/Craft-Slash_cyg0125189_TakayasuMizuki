#include "pch.h"
#include "PlayerAttack.h"
#include "Game/GamePlay/Actor/Player.h"

PlayerAttack::PlayerAttack (Player* owner)
	: PlayerState(owner, "Attack")
{
}

PlayerAttack::~PlayerAttack ()
{
}

void PlayerAttack::Enter()
{
	auto& weapon = m_owner->GetWeapon();
	weapon.AttackOn(ColliderTag::PlayerAttack);
	weapon.SetOffsetOfState("Attack");
	m_owner->SetBoneName("mixamorig6:RightHand");
	std::string attackName = weapon.GetName().data();
	m_owner->PlayAnimation(attackName + "Attack", false);
	m_owner->SetHold(true);
	m_owner->GetAnimator().SetMultiplyer(2.0f);
}

void PlayerAttack::Update(const GameContext&, float)
{
	if (m_owner->GetAnimator().IsEnd())
	{
		m_owner->GetStateMachine().RequestChangeState<PlayerIdle>();
	}
}

void PlayerAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	//ƒ‚ƒfƒ‹‚Ì•`‰æ
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, m_owner->GetModel<PlayerAttack>());

}

void PlayerAttack::Exit()
{
	m_owner->GetWeapon().AttackOff();
	m_owner->GetStateMachine().ResetTimer();
	m_owner->GetAnimator().SetMultiplyer(1.0f);
}

bool PlayerAttack::CanChanged()
{
	return m_owner->GetAnimator().IsEnd();
}
