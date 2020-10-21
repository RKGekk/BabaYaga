#include "inputclass.h"

InputClass::InputClass() : m_keys{} {}

InputClass::InputClass(const InputClass& other) : m_keys{} {}

InputClass::~InputClass() {}

void InputClass::Initialize() {
	// Initialize all the keys to being released and not pressed.
	for (int i = 0; i < 256; i++) {
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input) {
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input) {
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key) {
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}