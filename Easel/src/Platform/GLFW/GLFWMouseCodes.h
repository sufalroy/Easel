#pragma once

#include "Easel/Core/OS/MouseCodes.h"

#include <GLFW/glfw3.h>
#include <map>

namespace Easel {

	namespace GLFWMouseCodes {

		static InputCode::MouseKey GLFWToEaselMouseKey(uint32_t glfwKey) {
			
			static std::map<uint32_t, InputCode::MouseKey> keyMap = {
				
				{GLFW_MOUSE_BUTTON_LEFT,  InputCode::MouseKey::ButtonLeft},
				{GLFW_MOUSE_BUTTON_RIGHT, InputCode::MouseKey::ButtonRight},
				{GLFW_MOUSE_BUTTON_MIDDLE,InputCode::MouseKey::ButtonMiddle}
			};

			return keyMap[glfwKey];
		}
	}
}
