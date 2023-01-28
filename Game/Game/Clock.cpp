#include "Clock.h"

Clock::Clock(unsigned int m_clockInterval_)
{
	this->m_clockInterval_ = m_clockInterval_;
	m_startPoint_ = SDL_GetTicks();
}

Clock::Clock()
{
	m_clockInterval_ = 0;
	m_startPoint_ = SDL_GetTicks();
}

bool Clock::checkClockState()
{
	if (m_startPoint_ + m_clockInterval_ <= SDL_GetTicks()) {
		m_startPoint_ = SDL_GetTicks();
		return true;
	}
	return false;
}
