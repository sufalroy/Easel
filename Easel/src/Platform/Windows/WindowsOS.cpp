#include "Precompiled.h"
#include "WindowsOS.h"
#include "WindowsPower.h"
#include "Easel/Core/VFS.h"
#include "Easel/Core/CoreSystem.h"
#include "Easel/Core/OS/MemoryManager.h"
#include "Easel/Core/Application.h"

#ifdef EASEL_USE_GLFW_WINDOWS
	#include "Platform/GLFW/GLFWWindow.h"
#endif

extern Easel::Application* Easel::CreateApplication();

namespace Easel {

	void WindowsOS::Init() {

#ifdef EASEL_USE_GLFW_WINDOWS
		GLFWWindow::MakeDefault();

#endif // EASEL_USE_GLFW_WINDOWS

	}

	void WindowsOS::Run() {
		
		auto power = WindowsPower();
		auto percentage  = power.GetPowerPercentageLeft();
		auto secondsLeft = power.GetPowerSecondsLeft();
		auto state = power.GetPowerState();

		EASEL_CORE_INFO("--------------------");
		EASEL_CORE_INFO(" System Information ");
		EASEL_CORE_INFO("--------------------");

		if (state != PowerState::POWERSTATE_NO_BATTERY)
			EASEL_CORE_INFO("Battery Info - Percentage : {0} , Time Left {1}s , State : {2}", percentage, secondsLeft, PowerStateToString(state));
		else
			EASEL_CORE_INFO("Power - Outlet");

		auto systemInfo = MemoryManager::Get()->GetSystemInfo();
		systemInfo.Log();

		const std::string root = ROOT_DIR;
		VFS::Get()->Mount("Meshes", root + "/Assets/meshes");
		VFS::Get()->Mount("Textures", root + "/Assets/textures");
		VFS::Get()->Mount("Sounds", root + "/Assets/sounds");

		auto& app = Application::Get();
		app.Init();
		app.Run();
		app.Release();
	}

	SystemMemoryInfo MemoryManager::GetSystemInfo() {

		MEMORYSTATUSEX status;
		status.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&status);

		SystemMemoryInfo result = {

			(int64_t)status.ullAvailPhys,
			(int64_t)status.ullTotalPhys,

			(int64_t)status.ullAvailVirtual,
			(int64_t)status.ullTotalVirtual 
		};

		return result;
	}
}
