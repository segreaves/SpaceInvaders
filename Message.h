#pragma once

using MessageType = unsigned int;

struct XY
{
	XY(float x, float y) :
		x(x), y(y)
	{
	}

	float x;
	float y;
};

struct Message
{
	Message(MessageType type) :
		m_type(type),
		m_xy(0, 0),
		m_sender(-1),
		m_receiver(-1)
	{
	}

	MessageType m_type;
	unsigned int m_sender;
	unsigned int m_receiver;

	union
	{
		XY m_xy;
		bool m_bool;
		int m_int;
		float m_float;
	};
};
