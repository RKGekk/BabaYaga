#pragma once

#include <queue>
#include <algorithm>

class Mouse {
	
public:
	struct v2i {
		int x;
		int y;
	};

	class Event {
	public:
		enum class Type {
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;
		bool rightIsPressed;
		int x;
		int y;
	public:
		Event() : type(Type::Invalid), leftIsPressed(false), rightIsPressed(false), x(0), y(0) {}
		Event(Type type, const Mouse& parent) : type(type), leftIsPressed(parent.leftIsPressed), rightIsPressed(parent.rightIsPressed), x(parent.x), y(parent.y) {}

		bool IsValid() const {
			return type != Type::Invalid;
		}

		Type GetType() const {
			return type;
		}

		int GetPosX() const {
			return x;
		}

		int GetPosY() const {
			return y;
		}

		bool LeftIsPressed() const {
			return leftIsPressed;
		}

		bool RightIsPressed() const {
			return rightIsPressed;
		}
	};

	class MouseTracker {
	public:
		void Engage(int x, int y) {
			m_x = x;
			m_y = y;
			engaged = true;
		}
		void Release() {
			engaged = false;
		}
		v2i Move(int x, int y) {
			int delta_x = x - m_x;
			int delta_y = y - m_y;
			//if(delta_x != 0) delta_x = delta_x > 0 ? 1 : -1;
			//if(delta_y != 0) delta_y = delta_y > 0 ? 1 : -1;
			m_x = x;
			m_y = y;
			return {delta_x, delta_y};
		}
		bool Engaged() const {
			return engaged;
		}
	private:
		bool engaged = false;
		int m_x;
		int m_y;
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	int GetPosX() const;
	int GetPosY() const;
	bool LeftIsPressed() const;
	bool RightIsPressed() const;
	bool IsInWindow() const;
	Mouse::Event Read();
	bool IsEmpty() const;
	void Flush();

	void OnMouseMove(int x, int y);
	void OnMouseLeave();
	void OnMouseEnter();
	void OnLeftPressed(int x, int y);
	void OnLeftReleased(int x, int y);
	void OnRightPressed(int x, int y);
	void OnRightReleased(int x, int y);
	void OnWheelUp(int x, int y);
	void OnWheelDown(int x, int y);
	void TrimBuffer();

private:
	static constexpr unsigned int bufferSize = 1u;
	int x = 0;
	int y = 0;
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	std::queue<Event> buffer;
};