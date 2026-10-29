#include "pch.h"
#include "EnemyDefeated.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Message/EventBus.h"
#include "Message/AnnouncementManager.h"

EnemyDefeated::EnemyDefeated(Enemy* owner)
	: EnemyState(owner, "Defeated")
{
}

EnemyDefeated::~EnemyDefeated()
{
}

void EnemyDefeated::Enter()
{
	int id = m_owner->GetID();
	EventBus::GetInstance()->Publish(Message::EventID::ENEMY_TAKE_DAMAGE, &id);

	m_owner->PlayAnimation("Defeated", false);

	// ”»’è‚ð‚·‚×‚Ä–³Œø
    for (auto& b : m_owner->GetBounding())
    {
        b->SetEnable(false);
    }
}

void EnemyDefeated::Update(const GameContext&, float)
{
	if (m_owner->GetAnimator().IsEnd())
	{    
		// Ž€–SŠÖ”‚ÌŽÀs
		m_owner->PlayDieCallback();
		m_owner->SetStop(true);
	m_owner->DisposeWeapon();

	}
}

void EnemyDefeated::Render(const GameContext&, const SimpleMath::Matrix&)
{
}

void EnemyDefeated::Exit()
{
}
