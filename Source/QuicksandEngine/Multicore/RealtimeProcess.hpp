#ifndef QSE_REALTIMEPROCESS_HPP
#define QSE_REALTIMEPROCESS_HPP

#include "..\Mainloop\Process.hpp"

class RealtimeProcess : public Process
{
protected:
	HANDLE m_hThread;
	DWORD m_ThreadID;
	int m_ThreadPriority;

public:
	// Other prioities can be:
	// THREAD_PRIORITY_ABOVE_NORMAL
	// THREAD_PRIORITY_BELOW_NORMAL
	// THREAD_PRIORITY_HIGHEST
	// THREAD_PRIORITY_LOWEST
	// THREAD_PRIORITY_IDLE
	// THREAD_PRIORITY_TIME_CRITICAL
	//
	//
	RealtimeProcess(int priority = THREAD_PRIORITY_NORMAL);
    virtual ~RealtimeProcess(void) { CloseHandle(m_hThread); }
	static DWORD WINAPI ThreadProc( LPVOID lpParam );

protected:
	virtual void VOnInit(void);
	virtual void VOnUpdate(unsigned long deltaMs) override { }   // do nothing.
	virtual void VThreadProc(void) = 0;							 // must be defined for all real time processes.
};



#endif