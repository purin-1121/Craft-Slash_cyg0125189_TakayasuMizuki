#include "pch.h"
#include "EnemyAttack.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "EnemyChase.h"

EnemyAttack::EnemyAttack(Enemy* owner)
	: EnemyState(owner, "Attack")
{
}

EnemyAttack::~EnemyAttack()
{
}

void EnemyAttack::Enter()
{
	m_owner->AttackOn();
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	auto& weapon = m_owner->GetWeapon();
	weapon.AttackOn(ColliderTag::EnemyAttack);
	weapon.SetOffsetOfState("Attack");
	m_owner->SetBoneName("mixamorig6:RightHand");
	std::string attackName = weapon.GetName().data();
	m_owner->PlayAnimation(attackName + "Attack", false);
	m_owner->SetAfterID(StateID::Chase);
}

void EnemyAttack::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	// 攻撃持続時間以上だったら
	if (m_owner->GetAnimator().IsEnd())
	{
		// プレイヤーが範囲内だったら追跡状態
		if(m_owner->IsPlayerInSearchArea())
			m_owner->GetStateMachine().RequestChangeState<EnemyChase>();
		// 違ったら待機状態
		else
			m_owner->GetStateMachine().RequestChangeState<EnemyIdle>();
	}
}

void EnemyAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());
}

void EnemyAttack::Exit()
{
	m_owner->AttackOff();
}
