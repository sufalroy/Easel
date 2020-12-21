#include "Precompiled.h"
#include "Easel/Core/CoreSystem.h"
#include "Easel/Core/VFS.h"
#include "Easel/Core/JobSystem.h"
#include "Easel/Core/OS/MemoryManager.h"

namespace Easel {

	namespace Internal {

		void CoreSystem::Init(bool enableProfiler) {
			Debug::Log::OnInit();

			EASEL_CORE_INFO("Easel Engine");

			System::JobSystem::OnInit();
			EASEL_CORE_INFO("Initializing System");
			VFS::OnInit();
		}

		void CoreSystem::Shutdown() {
			EASEL_CORE_INFO("Shutting down System");
			VFS::OnShutdown();
			Easel::Memory::LogMemoryInformation();

			Debug::Log::OnRelease();

			MemoryManager::OnShutdown();
		}
	}
}

