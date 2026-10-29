#include "pch.h"
#include "BossAttackBase.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"

BossAttackBase::BossAttackBase(Enemy* owner)
	: BossState(owner, "Attack")
	, m_time{}
{
}

BossAttackBase::~BossAttackBase()
{
}

void BossAttackBase::Enter()
{
}

void BossAttackBase::Update(const GameContext&, float)
{
}

void BossAttackBase::Render(const GameContext&, const SimpleMath::Matrix& )
{
}

void BossAttackBase::RenderText(const GameContext&, const SimpleMath::Matrix&)
{
}

void BossAttackBase::Exit()
{
}
