#pragma once
#include "SDL.h"
class Clock
{
private:
	Uint32 m_startPoint_;
	unsigned int m_clockInterval_;
public:
	Clock(unsigned int m_clockInterval_);
	Clock();
	bool checkClockState();
	inline void setStartPoint(Uint32 m_startPoint_) { this->m_startPoint_ = m_startPoint_; }
	inline void setInterval(unsigned int m_clockInterval_) { this->m_clockInterval_ = m_clockInterval_; }
};

