#include "Precompiled.h"
#include "Easel/Core/OS/OS.h"

#if defined(EASEL_PLATFORM_WINDOWS)
#include "Platform/Windows/WindowsOS.h"
#endif

namespace Easel {

	OS* OS::s_Instance = nullptr;

	void OS::Create() {

		EASEL_ASSERT(!s_Instance, "OS already exists!");

#if defined(EASEL_PLATFORM_WINDOWS)
		s_Instance = new WindowsOS();
#endif
	}

	void OS::Release() {
		delete s_Instance;
		s_Instance = nullptr;
	}

	std::string OS::PowerStateToString(PowerState state) {
		switch (state) {

		case POWERSTATE_UNKNOWN:
			return std::string("Unknown");
			break;
		case POWERSTATE_NO_BATTERY:
			return std::string("No Battery");
			break;
		case POWERSTATE_CHARGED:
			return std::string("Charged");
			break;
		case POWERSTATE_CHARGING:
			return std::string("Charging");
			break;
		case POWERSTATE_ON_BATTERY:
			return std::string("On Battery");
			break;
		default:
			return std::string("Error");
			break;
		}
	}
}
