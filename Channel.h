#pragma once
#include "ISubscriber.h"
#include <vector>

class Channel
{
public:
	~Channel()
	{
		m_subs.clear();
	}

	bool addSubscriber(ISubscriber* subscriber)
	{
		if (hasSubscriber(subscriber)) return false;
		m_subs.emplace_back(subscriber);
		return true;
	}

	bool removeSubscriber(ISubscriber* subscriber)
	{
		//auto toRemove = std::find_if(m_subs.begin(), m_subs.end(), [&subscriber](ISubscriber* sub) { return sub == subscriber; });
		auto toRemove = std::find(m_subs.begin(), m_subs.end(), subscriber);
		if (toRemove == m_subs.end()) return false;
		m_subs.erase(toRemove);
		return true;
	}

	void broadcast(const Message& msg)
	{
		for (auto& sub : m_subs)
			sub->notify(msg);
	}

	bool hasSubscriber(ISubscriber* subscriber) const
	{
		//return std::find_if(m_subs.begin(), m_subs.end(), [&subscriber](ISubscriber* sub) { return sub == subscriber; }) != m_subs.end();
		return std::find(m_subs.begin(), m_subs.end(), subscriber) != m_subs.end();
	}
private:
	std::vector<ISubscriber*> m_subs;
};