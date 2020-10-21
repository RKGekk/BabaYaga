#include "ProcessManager.h"

ProcessManager::~ProcessManager(void) {
    ClearAllProcesses();
}

// The process update tick.  Called every logic tick.  This function returns the number of process chains that 
// succeeded in the upper 32 bits and the number of process chains that failed or were aborted in the lower 32 bits.
unsigned int ProcessManager::UpdateProcesses(float deltaMs) {
    unsigned short int successCount = 0;
    unsigned short int failCount = 0;

    ProcessList::iterator it = m_processList.begin();
    while (it != m_processList.end()) {
        // grab the next process
        std::shared_ptr<Process> pCurrProcess = (*it);

        // save the iterator and increment the old one in case we need to remove this process from the list
        ProcessList::iterator thisIt = it;
        ++it;

        // process is uninitialized, so initialize it
        if (pCurrProcess->GetState() == Process::State::UNINITIALIZED)
            pCurrProcess->VOnInit();

        // give the process an update tick if it's running
        if (pCurrProcess->GetState() == Process::State::RUNNING)
            pCurrProcess->VOnUpdate(deltaMs);

        // check to see if the process is dead
        if (pCurrProcess->IsDead()) {
            // run the appropriate exit function
            switch (pCurrProcess->GetState()) {
                case Process::State::SUCCEEDED: {
                    pCurrProcess->VOnSuccess();
                    std::shared_ptr<Process> pChild = pCurrProcess->RemoveChild();
                    if (pChild) {
                        AttachProcess(pChild);
                    }
                    else {
                        ++successCount;  // only counts if the whole chain completed
                    }
                    break;
                }

                case Process::State::FAILED: {
                    pCurrProcess->VOnFail();
                    ++failCount;
                    break;
                }

                case Process::State::ABORTED: {
                    pCurrProcess->VOnAbort();
                    ++failCount;
                    break;
                }
            }

            // remove the process and destroy it
            m_processList.erase(thisIt);
        }
    }

    return ((successCount << 16) | failCount);
}

// Attaches the process to the process list so it can be run on the next update.
std::weak_ptr<Process> ProcessManager::AttachProcess(std::shared_ptr<Process> pProcess) {
    m_processList.push_front(pProcess);
    return std::weak_ptr<Process>(pProcess);
}

// Clears all processes (and DOESN'T run any exit code)
void ProcessManager::ClearAllProcesses(void) {
    m_processList.clear();
}

// Aborts all processes.  If immediate == true, it immediately calls each ones OnAbort() function and destroys all 
// the processes.
void ProcessManager::AbortAllProcesses(bool immediate) {
    ProcessList::iterator it = m_processList.begin();
    while (it != m_processList.end()) {
        ProcessList::iterator tempIt = it;
        ++it;

        std::shared_ptr<Process> pProcess = *tempIt;
        if (pProcess->IsAlive()) {
            pProcess->SetState(Process::State::ABORTED);
            if (immediate) {
                pProcess->VOnAbort();
                m_processList.erase(tempIt);
            }
        }
    }
}