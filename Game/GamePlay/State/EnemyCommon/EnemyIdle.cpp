#include "pch.h"
#include "EnemyIdle.h"
#include "Game/GamePlay/Enemy/Enemy.h"

EnemyIdle::EnemyIdle(Enemy* owner)
	: EnemyState(owner, "Idle")
{
}

EnemyIdle::~EnemyIdle()
{
}

void EnemyIdle::Enter()
{
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	m_owner->PlayAnimation(m_owner->GetIdleAnimationName());
	m_owner->SetAfterID(StateID::Chase);
}

void EnemyIdle::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	elapsedTime;
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
}

void EnemyIdle::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());
}

void EnemyIdle::Exit()
{
}
