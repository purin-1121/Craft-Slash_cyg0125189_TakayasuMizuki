#include "pch.h"
#include "BossIdle.h"
#include "Game/GamePlay/Enemy/Enemy.h"

BossIdle::BossIdle(Enemy* owner)
	: BossState(owner, "Idle")
{
}

BossIdle::~BossIdle()
{
}

void BossIdle::Enter()
{
	m_owner->PlayAnimation(m_owner->GetIdleAnimationName());
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
}

void BossIdle::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	elapsedTime;
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
}

void BossIdle::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	Draw::GetInstance().DrawModel(gameContext, m_owner->GetWorldMatrix(), view, ObjectLib::m_models[m_owner->GetData()->name].get());

}

void BossIdle::Exit()
{
}
