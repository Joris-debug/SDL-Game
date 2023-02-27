#include "WorkThread.h"

WorkThread::WorkThread()
{
	m_threadIsRunning = true;
}

WorkThread::~WorkThread()
{
	m_threadIsRunning = false;	//To ensure the app doesnt wait forever
	m_p_thread->join();
	delete m_p_thread;
}

void WorkThread::startThread()
{
	m_p_thread = new std::thread(&WorkThread::run, this);	//Opens overriten method
}
