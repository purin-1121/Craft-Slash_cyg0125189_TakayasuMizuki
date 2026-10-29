//--------------------------------------------------------------------------------------
// File: AnnouncementManager.h
//
// ゲーム内イベントの仲介者
//
// Date: 2026.7.21
// Author: Mizuki Takayasu
//--------------------------------------------------------------------------------------


#pragma once
#include <unordered_map>
#include <functional>
#include <memory>
#include "Message.h"
#include <Common/UI/Text.h>
#include <Common/UI/Image.h>

class AnnouncementManager
{
public :
	enum class AnnouncementType
	{
		BossAttack,			// ボスの攻撃
		BossPhaseChange,	// ボスの攻撃準備
		RareItemGet,		// レアアイテムゲット
		ItemGet,			// アイテムゲット
		EnemySpawn,			// 敵出現
		PlayerDeath,		// プレイヤーの死亡
		QuestComplete,		// クエストクリア
		TimeLimit,			// 時間制限
		System				// システム関連
	};

	struct Announcement
	{
		AnnouncementType type;
		std::wstring message;

		float displayTime;
	};
	
	void Announce(Announcement message);
	void Initialize(const GameContext& gameContext);
	void Update(float elapsedTime);
	void Render() {};
	void RenderText();

	void Reset() { m_currentAnnounce.displayTime = -10.0f, m_text.SetText(L""); };

public:

	static AnnouncementManager* GetInstance() {
		if (m_announcementManager == nullptr)
		{
			m_announcementManager.reset(new AnnouncementManager);
		}

		return m_announcementManager.get();
	}

	static void DeleteInstance()
	{
		m_announcementManager.reset();
	}

	~AnnouncementManager() = default;

private:

	static std::unique_ptr<AnnouncementManager> m_announcementManager;

	Announcement m_currentAnnounce;
	Text m_text;
	Image m_image;

	AnnouncementManager() :
		m_text{ L"", {0,0} }, 
		m_image{{300, 10}, {0.3, 0.3f, 0.3f, 0.7f}, {680, 40}}, 
		m_currentAnnounce{} 
	{
	};


};

