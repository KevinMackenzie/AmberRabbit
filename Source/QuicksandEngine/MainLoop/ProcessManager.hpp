#ifndef QSE_PROCESSMANAGER_HPP
#define QSE_PROCESSMANAGER_HPP

#include "Process.hpp"

class ProcessManager
{
	typedef std::list<StrongProcessPtr> ProcessList;

	ProcessList m_processList;

public:
	// construction
	~ProcessManager(void);

	// interface
	unsigned int UpdateProcesses(unsigned long deltaMs);  // updates all attached processes
	WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);  // attaches a process to the process mgr
	void AbortAllProcesses(bool immediate);

	// accessors
	unsigned int GetProcessCount(void) const { return m_processList.size(); }

private:
	void ClearAllProcesses(void);  // should only be called by the destructor
};


#endif