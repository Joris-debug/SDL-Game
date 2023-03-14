#pragma once
#include "SDL.h"
class Clock
{
private:
	int64_t m_startPoint_;
	unsigned int m_clockInterval_;
	bool m_setStartPointAuto_;		//Default: true, resets the start point when check returns true
public:
	Clock(unsigned int m_clockInterval_);
	Clock(unsigned int m_clockInterval_, bool m_setStartPointAuto_);
	Clock();
	bool checkClockState();
	inline void setStartPoint(int64_t m_startPoint_) { this->m_startPoint_ = m_startPoint_; }
	inline int64_t getStartPoint() { return m_startPoint_; }
	inline void setInterval(unsigned int m_clockInterval_) { this->m_clockInterval_ = m_clockInterval_; }
	inline void updateStartPoint(unsigned int sum) { m_startPoint_ += sum; }
};

