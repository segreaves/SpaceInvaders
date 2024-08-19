#include "Level.h"
#include "Context.h"

Level::Level(Context* context)
{
	m_context = context;
	m_context->m_level = this;
}

Level::~Level()
{
	m_context = nullptr;
}
