#pragma once

#include "Easel/Core/OS/KeyCodes.h"

#include <GLFW/glfw3.h>
#include <map>

namespace Easel {

	namespace GLFWKeyCodes {
		
		static InputCode::Key GLFWToEaselKeyboardKey(uint32_t glfwKey)
		{
			static std::map<uint32_t, InputCode::Key> keyMap = {

				{GLFW_KEY_A, InputCode::Key::A},
				{GLFW_KEY_B, InputCode::Key::B},
				{GLFW_KEY_C, InputCode::Key::C},
				{GLFW_KEY_D, InputCode::Key::D},
				{GLFW_KEY_E, InputCode::Key::E},
				{GLFW_KEY_F, InputCode::Key::F},
				{GLFW_KEY_G, InputCode::Key::G},
				{GLFW_KEY_H, InputCode::Key::H},
				{GLFW_KEY_I, InputCode::Key::I},
				{GLFW_KEY_J, InputCode::Key::J},
				{GLFW_KEY_K, InputCode::Key::K},
				{GLFW_KEY_L, InputCode::Key::L},
				{GLFW_KEY_M, InputCode::Key::M},
				{GLFW_KEY_N, InputCode::Key::N},
				{GLFW_KEY_O, InputCode::Key::O},
				{GLFW_KEY_P, InputCode::Key::P},
				{GLFW_KEY_Q, InputCode::Key::Q},
				{GLFW_KEY_R, InputCode::Key::R},
				{GLFW_KEY_S, InputCode::Key::S},
				{GLFW_KEY_T, InputCode::Key::T},
				{GLFW_KEY_U, InputCode::Key::U},
				{GLFW_KEY_V, InputCode::Key::V},
				{GLFW_KEY_W, InputCode::Key::W},
				{GLFW_KEY_X, InputCode::Key::X},
				{GLFW_KEY_Y, InputCode::Key::Y},
				{GLFW_KEY_Z, InputCode::Key::Z},
							 
				{GLFW_KEY_0, InputCode::Key::D0},
				{GLFW_KEY_1, InputCode::Key::D1},
				{GLFW_KEY_2, InputCode::Key::D2},
				{GLFW_KEY_3, InputCode::Key::D3},
				{GLFW_KEY_4, InputCode::Key::D4},
				{GLFW_KEY_5, InputCode::Key::D5},
				{GLFW_KEY_6, InputCode::Key::D6},
				{GLFW_KEY_7, InputCode::Key::D7},
				{GLFW_KEY_8, InputCode::Key::D8},
				{GLFW_KEY_9, InputCode::Key::D9},

				{GLFW_KEY_F1, InputCode::Key::F1},
				{GLFW_KEY_F2, InputCode::Key::F2},
				{GLFW_KEY_F3, InputCode::Key::F3},
				{GLFW_KEY_F4, InputCode::Key::F4},
				{GLFW_KEY_F5, InputCode::Key::F5},
				{GLFW_KEY_F6, InputCode::Key::F6},
				{GLFW_KEY_F7, InputCode::Key::F7},
				{GLFW_KEY_F8, InputCode::Key::F8},
				{GLFW_KEY_F9, InputCode::Key::F9},
				{GLFW_KEY_F10,InputCode::Key::F10},
				{GLFW_KEY_F11,InputCode::Key::F11},
				{GLFW_KEY_F12,InputCode::Key::F12},

				{GLFW_KEY_MINUS, InputCode::Key::Minus},
				{GLFW_KEY_DELETE, InputCode::Key::Delete},
				{GLFW_KEY_SPACE, InputCode::Key::Space},
				{GLFW_KEY_LEFT, InputCode::Key::Left},
				{GLFW_KEY_RIGHT, InputCode::Key::Right},
				{GLFW_KEY_UP, InputCode::Key::Up},
				{GLFW_KEY_DOWN, InputCode::Key::Down},
				{GLFW_KEY_LEFT_SHIFT, InputCode::Key::LeftShift},
				{GLFW_KEY_RIGHT_SHIFT, InputCode::Key::RightShift},
				{GLFW_KEY_ESCAPE, InputCode::Key::Escape},
				{GLFW_KEY_KP_ADD, InputCode::Key::A},
				{GLFW_KEY_COMMA, InputCode::Key::Comma},
				{GLFW_KEY_BACKSPACE, InputCode::Key::Backspace},
				{GLFW_KEY_ENTER, InputCode::Key::Enter},
				{GLFW_KEY_LEFT_SUPER, InputCode::Key::LeftSuper},
				{GLFW_KEY_RIGHT_SUPER, InputCode::Key::RightSuper},
				{GLFW_KEY_LEFT_ALT, InputCode::Key::LeftAlt},
				{GLFW_KEY_RIGHT_ALT, InputCode::Key::RightAlt},
				{GLFW_KEY_LEFT_CONTROL, InputCode::Key::LeftControl},
				{GLFW_KEY_RIGHT_CONTROL, InputCode::Key::RightControl}
			};

			return keyMap[glfwKey];
		}
	}
}
