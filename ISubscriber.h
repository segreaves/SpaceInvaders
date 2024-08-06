#pragma once

class Message;

class ISubscriber
{
public:
	virtual ~ISubscriber() {}
	virtual void notify(const Message& msg) = 0;
};
