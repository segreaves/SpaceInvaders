#pragma once
#include "ActorMessage.h"
#include "Channel.h"
#include "Message.h"
#include <unordered_map>

class MessageHandler
{
public:
	bool subscribe(ActorMessage messageType, ISubscriber* sub)
	{
		return m_channels[messageType].addSubscriber(sub);
	}

	bool unsubscribe(ActorMessage messageType, ISubscriber* sub)
	{
		return m_channels[messageType].removeSubscriber(sub);
	}

	void dispatch(const Message& msg)
	{
		auto it = m_channels.find((ActorMessage)msg.m_type);
		if (it == m_channels.end()) return;
		it->second.broadcast(msg);
	}
private:
	std::unordered_map<ActorMessage, Channel> m_channels;
};
