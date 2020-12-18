#include "Precompiled.h"
#include "Easel/Core/OS/MemoryManager.h"

namespace Easel {

	MemoryManager* MemoryManager::s_Instance = nullptr;

	MemoryManager::MemoryManager() {

	}

	void MemoryManager::OnInit() {

	}

	void MemoryManager::OnShutdown() {
		if (s_Instance)
			delete s_Instance;
	}

	MemoryManager* MemoryManager::Get() {
		if (s_Instance == nullptr) {
			s_Instance = new MemoryManager();
		}

		return s_Instance;
	}

	std::string MemoryManager::BytesToString(int64_t bytes) {

		static const float gb = 1024 * 1024 * 1024;
		static const float mb = 1024 * 1024;
		static const float kb = 1024;

		std::stringstream result;
		if (bytes > gb)
			result << std::fixed << std::setprecision(2) << (float)bytes / gb << " gb";
		else if (bytes > mb)
			result << std::fixed << std::setprecision(2) << (float)bytes / mb << " mb";
		else if (bytes > kb)
			result << std::fixed << std::setprecision(2) << (float)bytes / kb << " kb";
		else
			result << std::fixed << std::setprecision(2) << (float)bytes << " bytes";
		
		return result.str();
	}

	void SystemMemoryInfo::Log() {

		std::string apm, tpm, avm, tvm;

		apm = MemoryManager::BytesToString(availablePhysicalMemory);
		tpm = MemoryManager::BytesToString(totalPhysicalMemory);
		avm = MemoryManager::BytesToString(availableVirtualMemory);
		tvm = MemoryManager::BytesToString(totalVirtualMemory);

		EASEL_CORE_INFO("Memory Info:");
		EASEL_CORE_INFO("\tPhysical Memory : {0} / {1}:", apm, tpm);
		EASEL_CORE_INFO("\tVirtual Memory : {0} / {1}:", avm, tvm);
	}
}

