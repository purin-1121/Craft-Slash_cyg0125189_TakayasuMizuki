#include "pch.h"
#include "EnemyState.h"
#include "Game/GamePlay/Enemy/Enemy.h"

EnemyState::EnemyState(Enemy* owner, const std::string& stateName)
	: State(owner, stateName)
{
}

EnemyState::~EnemyState()
{
}

void EnemyState::Enter()
{

}

void EnemyState::Update(const GameContext& gameContext, float elapsedTime)
{
	gameContext;
	elapsedTime;
}

void EnemyState::Render(const GameContext& gameContext, const SimpleMath::Matrix& view)
{
	gameContext;
	view;
}

void EnemyState::Exit()
{
}
