#pragma once
#include <queue>

using EventId = unsigned int;

class EventQueue
{
public:
	void addEvent(const EventId& event)
	{
		m_eventQueue.push(event);
	}

	bool processEvent(EventId& event)
	{
		if (m_eventQueue.empty())
			return false;
		event = m_eventQueue.front();
		m_eventQueue.pop();
		return true;
	}
private:
	std::queue<EventId> m_eventQueue;
};
