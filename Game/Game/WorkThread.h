#pragma once
#include <thread>

enum class MultiplayerStatus { isRunning, gameOver, gameEnded };

class WorkThread
{
protected:
	std::thread* m_p_thread;
	MultiplayerStatus m_threadStatus;
public:
	WorkThread();
	virtual ~WorkThread();
	virtual void run() = 0;		//This function stores the work which has to be done
	void startThread();
	inline void setThreadStatus(MultiplayerStatus m_threadStatus) { this->m_threadStatus = m_threadStatus; }
	inline MultiplayerStatus getThreadStatus() { return m_threadStatus; }
};

