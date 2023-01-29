#pragma once
#include "SDL.h"
class Clock
{
private:
	Uint32 m_startPoint_;
	unsigned int m_clockInterval_;
	bool m_setStartPointAuto_;		//Default: true, resets the start point when check returns true
public:
	Clock(unsigned int m_clockInterval_);
	Clock(unsigned int m_clockInterval_, bool m_setStartPointAuto_);
	Clock();
	bool checkClockState();
	inline void setStartPoint(Uint32 m_startPoint_) { this->m_startPoint_ = m_startPoint_; }
	inline Uint32 getStartPoint() { return m_startPoint_; }
	inline void setInterval(unsigned int m_clockInterval_) { this->m_clockInterval_ = m_clockInterval_; }
};

