#include "WorkThread.h"
#include <iostream>
WorkThread::WorkThread()
{
	m_threadStatus = MultiplayerStatus::isRunning;
	m_p_thread = nullptr;
}

WorkThread::~WorkThread()
{
	std::cout << "Delete workthread\n";
	delete m_p_thread;
}

void WorkThread::startThread()
{
	m_p_thread = new std::thread(&WorkThread::run, this);	//Opens overriten method
}
