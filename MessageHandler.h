#pragma once
#include "EntityMessage.h"
#include "Channel.h"
#include <unordered_map>

class MessageHandler
{
public:
	bool subscribe(EntityMessage messageType, ISubscriber* sub)
	{
		// check what happens when the system doesn't exist
		return m_channels[messageType]->addSubscriber(sub);
	}

	bool unsubscribe(EntityMessage messageType, ISubscriber* sub)
	{
		return m_channels[messageType]->removeSubscriber(sub);
	}

	void dispatch(EntityMessage messageType, const Message& msg)
	{
		m_channels[messageType]->broadcast(msg);
	}
private:
	std::unordered_map<EntityMessage, Channel*> m_channels;
};
