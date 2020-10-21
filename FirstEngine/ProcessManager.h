#pragma once

#include <list>

#include "Process.h"

class ProcessManager {
	typedef std::list<std::shared_ptr<Process>> ProcessList;

	ProcessList m_processList;

public:
	// construction
	~ProcessManager(void);

	// interface
	unsigned int UpdateProcesses(float deltaMs);  // updates all attached processes
	std::weak_ptr<Process> AttachProcess(std::shared_ptr<Process> pProcess);  // attaches a process to the process mgr
	void AbortAllProcesses(bool immediate);

	// accessors
	unsigned int GetProcessCount(void) const { return m_processList.size(); }

private:
	void ClearAllProcesses(void);  // should only be called by the destructor
};