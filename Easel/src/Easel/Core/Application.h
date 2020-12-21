#pragma once

#include "Easel/Core/Base.h"
#include "Easel/Events/Event.h"

#include <string>

int main(int argc, char** argv);

namespace Easel {

	class EASEL_EXPORT Application {
	public:
		Application(const std::string& projectRoot, const std::string& projectName);
		virtual ~Application();

		void OnEvent(Event& e);

		void Close();

		_FORCE_INLINE_ static Application& Get() { return *s_Instance; }

	private:
		void Run();
		
	private:
		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}
