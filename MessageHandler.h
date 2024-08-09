#pragma once
#include "EntityMessage.h"
#include "Channel.h"
#include "Message.h"
#include <unordered_map>

class MessageHandler
{
public:
	bool subscribe(EntityMessage messageType, ISubscriber* sub)
	{
		// check what happens when the message type doesn't exist
		return m_channels[messageType]->addSubscriber(sub);
	}

	bool unsubscribe(EntityMessage messageType, ISubscriber* sub)
	{
		return m_channels[messageType]->removeSubscriber(sub);
	}

	void dispatch(const Message& msg)
	{
		auto it = m_channels.find((EntityMessage)msg.m_type);
		if (it == m_channels.end()) return;
		it->second->broadcast(msg);
	}
private:
	std::unordered_map<EntityMessage, Channel*> m_channels;
};
