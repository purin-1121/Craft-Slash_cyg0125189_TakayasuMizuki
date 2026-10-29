#include "pch.h"
#include "BossDefeated.h"
#include "Game/GamePlay/Enemy/Enemy.h"
#include "Game/GamePlay/Actor/Player.h"
#include "Message/EventBus.h"
#include "Message/AnnouncementManager.h"

BossDefeated::BossDefeated(Enemy* owner)
	: BossState(owner, "Defeated")
{
}

BossDefeated::~BossDefeated()
{
}

void BossDefeated::Enter()
{
	// ボスイベントを実行
	EventBus::GetInstance()->Publish(Message::EventID::BOSSDEFEATED, this);
	// 判定をすべて無効
	for (auto& b : m_owner->GetBounding())
	{
		b->SetEnable(false);
	}
	
	AnnouncementManager::GetInstance()->Announce(
		{ AnnouncementManager::AnnouncementType::QuestComplete,
		L"ボスを撃破!    クエスト完了!!",
		10000000.0f
		}
	);
}

void BossDefeated::Update(const GameContext&, float)
{
}

void BossDefeated::Render(const GameContext&, const SimpleMath::Matrix&)
{
}

void BossDefeated::Exit()
{
}
