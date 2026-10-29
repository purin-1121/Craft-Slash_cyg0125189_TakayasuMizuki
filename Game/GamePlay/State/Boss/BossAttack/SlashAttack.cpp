#include "pch.h"
#include "SlashAttack.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"

SlashAttack::SlashAttack(Enemy* owner)
	: BossAttackBase(owner)
{
}

SlashAttack::~SlashAttack()
{
}

void SlashAttack::Enter()
{
	m_owner->SetVelocity(DirectX::SimpleMath::Vector3::Zero);
	ResetFinished();
}

void SlashAttack::Update(const GameContext& gameContext, float elapsedTime)
{
	AddTime(elapsedTime);

	if (GetTime() >= 1.5f)
	{
		Finish();
	}
	else if(GetTime() >= 0.5f)
	{
		m_owner->AttackOn();
	}
}

void SlashAttack::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void SlashAttack::Exit()
{
	m_owner->AttackOff();
	ResetTime();
}
