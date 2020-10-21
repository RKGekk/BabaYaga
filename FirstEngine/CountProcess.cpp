#include "CountProcess.h"

void CountProcess::VOnUpdate(float deltaMs) {

	if (m_count == 3) {
		Succeed();
	}
	std::cout << m_count << std::endl;
	++m_count;
}