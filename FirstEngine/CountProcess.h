#pragma once

#include <iostream>

#include "Process.h"

class CountProcess : public Process {

	int m_count = 0;

	// Inherited via Process
	virtual void VOnUpdate(float deltaMs) override;
};