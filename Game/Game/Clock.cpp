#include "Clock.h"

Clock::Clock(unsigned int m_clockInterval_)
{
	this->m_clockInterval_ = m_clockInterval_;
	m_setStartPointAuto_ = true;
	m_startPoint_ = SDL_GetTicks();
}

Clock::Clock(unsigned int m_clockInterval_, bool m_setStartPointAuto_)
{
	this->m_clockInterval_ = m_clockInterval_;
	this->m_setStartPointAuto_ = m_setStartPointAuto_;
	m_startPoint_ = SDL_GetTicks();
}

Clock::Clock()
{
	m_clockInterval_ = 0;
	m_setStartPointAuto_ = true;
	m_startPoint_ = SDL_GetTicks();
}

bool Clock::checkClockState()
{
	if (m_startPoint_ + m_clockInterval_ <= SDL_GetTicks()) {
		if(m_setStartPointAuto_)
			m_startPoint_ = SDL_GetTicks();
		return true;
	}
	return false;
}
