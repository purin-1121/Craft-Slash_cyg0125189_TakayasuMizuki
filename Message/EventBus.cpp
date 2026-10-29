#include "pch.h"
#include "EventBus.h"

EventBus* EventBus::m_eventBus = nullptr;

SubsprictionID EventBus::Subscribe(const Message::EventID& event, std::function<void(void*)> callback)
{
	m_subscribes[event].push_back({m_id, callback});
	return m_id++;
}

void EventBus::UnSubscribe(const Message::EventID& event, SubsprictionID id)
{
	auto& subscribers = m_subscribes[event];

	subscribers.erase(
		std::remove_if(
			subscribers.begin(),
			subscribers.end(),
			[&](const Subscriber& s)
			{
				return s.id == id;
			}),
		subscribers.end());
}

void EventBus::Publish(const Message::EventID& event, void* Messager)
{
	auto it = m_subscribes.find(event);

	if (it == m_subscribes.end())
		return;

	for (auto& cb : it->second)
	{
		cb.callback(Messager);
	}
}
