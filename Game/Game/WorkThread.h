#pragma once
#include <thread>

class WorkThread
{
protected:
	std::thread* m_p_thread;
	bool m_threadIsRunning;
public:
	WorkThread();
	~WorkThread();
	virtual void run() = 0;		//This function stores the work which has to be done
	void startThread();
};

