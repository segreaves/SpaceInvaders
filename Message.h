#pragma once

using MessageType = unsigned int;

struct XY
{
	XY(float x, float y) : m_x(x), m_y(y)
	{
	}
	float m_x;
	float m_y;
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
	};
};
