#pragma once

#include "event.h"

namespace sge
{
	class InputSystem
	{
	public:
		InputSystem();
		~InputSystem();

		void processMouseMovement(double x, double y);
		void processMouseScroll(float offset);
		void setPressedRightMouse(bool pressed);
		void processInput(int key, int action);

		double getX();
		double getY();
		double getScroll();
		bool isPressedMouseRight();
		bool isKeyPressed(int key_code);

	public:
		Event<float> on_mouse_scroll;

	private:
		float last_mouse_x;
		float last_mouse_y;
		float last_scroll_offset;
		bool mouse_right_button_pressed;
		bool key_state[512]{ 0 };
	};
}