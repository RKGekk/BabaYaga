#pragma once

#include <memory>

class Process {
	friend class ProcessManager;

public:
	enum class State {
		// Processes that are neither dead nor alive
		UNINITIALIZED = 0,  // created but not running
		REMOVED,  // removed from the process list but not destroyed; this can happen when a process that is already running is parented to another process

		// Living processes
		RUNNING,  // initialized and running
		PAUSED,  // initialized but paused

		// Dead processes
		SUCCEEDED,  // completed successfully
		FAILED,  // failed to complete
		ABORTED,  // aborted; may not have started
	};

private:
	State m_state;  // the current state of the process
	std::shared_ptr<Process> m_pChild;  // the child process, if any

public:
	// construction
	Process();
	virtual ~Process();

protected:
	// interface; these functions should be overridden by the subclass as needed
	virtual void VOnInit() { m_state = State::RUNNING; }  // called during the first update; responsible for setting the initial state (typically RUNNING)
	virtual void VOnUpdate(float deltaMs) = 0;  // called every frame
	virtual void VOnSuccess() {}  // called if the process succeeds (see below)
	virtual void VOnFail() {}  // called if the process fails (see below)
	virtual void VOnAbort() {}  // called if the process is aborted (see below)

public:
	// Functions for ending the process.
	void Succeed();
	void Fail();

	// pause
	void Pause();
	void UnPause();

	// accessors
	State GetState() const { return m_state; }
	bool IsAlive() const { return (m_state == State::RUNNING || m_state == State::PAUSED); }
	bool IsDead() const { return (m_state == State::SUCCEEDED || m_state == State::FAILED || m_state == State::ABORTED); }
	bool IsRemoved() const { return (m_state == State::REMOVED); }
	bool IsPaused() const { return m_state == State::PAUSED; }

	// child functions
	void AttachChild(std::shared_ptr<Process> pChild);
	std::shared_ptr<Process> RemoveChild();  // releases ownership of the child
	std::shared_ptr<Process> PeekChild() { return m_pChild; }  // doesn't release ownership of the child

private:
	void SetState(State newState) { m_state = newState; }
};