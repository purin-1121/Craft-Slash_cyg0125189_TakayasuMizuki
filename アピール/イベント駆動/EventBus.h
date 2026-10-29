//--------------------------------------------------------------------------------------
// File: EventBus.h
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
#include "Messenge.h"

using SubsprictionID = uint16_t;

class EventBus
{
	struct Subscriber
	{
		SubsprictionID id;
		std::function<void(void*)> callback;
	};
	
public:

	SubsprictionID Subscribe(const Message::EventID& event, std::function<void(void*)> callback);	// イベント登録
	void UnSubscribe(const Message::EventID& event, SubsprictionID id);	// イベント解除

	void Publish(const Message::EventID& event, void* messenger);		// イベント発行
	
	static EventBus* GetInstance() {
		if (m_eventBus == nullptr)
		{
			m_eventBus = new EventBus();
		}

		return m_eventBus;
	}

	static void DeleteInstance()
	{
		delete m_eventBus;
	}

private:

	EventBus() = default;
	~EventBus() = default;

	// イベントのマップ
	std::unordered_map<Message::EventID, std::vector<Subscriber>> m_subscribes;
	// 購買ID
	SubsprictionID m_id = 0;
	// 自身のポインタ
	static EventBus* m_eventBus;


};

