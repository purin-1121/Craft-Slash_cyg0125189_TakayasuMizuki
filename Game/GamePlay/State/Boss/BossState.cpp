#include "pch.h"
#include "BossState.h"
#include "Game/GamePlay/Enemy/Enemy.h"

BossState::BossState(Enemy* owner, const std::string& stateName)
	: State(owner, stateName)
{
}

BossState::~BossState()
{
}

void BossState::Enter()
{
}

void BossState::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	elapsedTime;
}

void BossState::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void BossState::RenderText(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
}

void BossState::Exit()
{
}
