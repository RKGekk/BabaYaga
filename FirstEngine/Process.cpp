#include "Process.h"

Process::Process() {
	m_state = State::UNINITIALIZED;
	//m_pParent = NULL;
	//m_pChild = NULL;
}

Process::~Process() {
	if (m_pChild) {
		m_pChild->VOnAbort();
	}
}

// Removes the child from this process.  This releases ownership of the child to the caller and completely removes it
// from the process chain.
std::shared_ptr<Process> Process::RemoveChild() {
	if (m_pChild) {
		std::shared_ptr<Process> pChild = m_pChild;  // this keeps the child from getting destroyed when we clear it
		m_pChild.reset();
		//pChild->SetParent(NULL);
		return pChild;
	}

	return std::shared_ptr<Process>();
}

void Process::Succeed() {
	m_state = State::SUCCEEDED;
}

void Process::Fail() {
	m_state = State::FAILED;
}

void Process::AttachChild(std::shared_ptr<Process> pChild) {
	if (m_pChild)
		m_pChild->AttachChild(pChild);
	else
		m_pChild = pChild;
}

void Process::Pause() {
	if (m_state == State::RUNNING)
		m_state = State::PAUSED;
}

void Process::UnPause() {
	if (m_state == State::PAUSED)
		m_state = State::RUNNING;
}