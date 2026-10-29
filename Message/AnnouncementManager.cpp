#include "pch.h"	
#include "AnnouncementManager.h"

std::unique_ptr<AnnouncementManager> AnnouncementManager::m_announcementManager = nullptr;

void AnnouncementManager::Announce(Announcement message)
{
	m_currentAnnounce = message;
	m_text.SetText(message.message);
	m_text.SetCenter({ DataManager::GetRefInstance().CenterX , 30});
	switch (message.type)
	{
	case AnnouncementType::BossAttack:
	case AnnouncementType::BossPhaseChange:
		m_image.SetColor({1.0f, 0.0f, 0.0f, 0.4f});
		break;
	default:
		m_image.SetColor({ 0.3f, 0.3f, 0.3f, 0.7f });
		break;
	}


}

void AnnouncementManager::Update(float elapsedTime)
{
	if (m_currentAnnounce.displayTime < 0)
		return;

	m_currentAnnounce.displayTime -= elapsedTime;
	
}

void AnnouncementManager::Initialize(const GameContext& gameContext)
{
	m_image.Initialize(gameContext);
	Reset();
}

void AnnouncementManager::RenderText()
{
	if (m_currentAnnounce.displayTime > 0)
	{
		m_image.RenderText();
		m_text.RenderText();
	}
}
