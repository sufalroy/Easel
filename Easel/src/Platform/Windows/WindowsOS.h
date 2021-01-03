#pragma once

#include "Easel/Core/OS/OS.h"

namespace Easel {

	class EASEL_EXPORT WindowsOS : public OS {
	public:
		WindowsOS() = default;
		~WindowsOS() = default;

		void Init();
		void Run() override;
		std::string GetExecutablePath() override {
			return "";
		}
	};
}
