#include "Precompiled.h"
#include "Easel/Core/Engine.h"

namespace Easel {

	Engine::Engine()
		: m_MaxFramesPerSecond(1000.0f / 60.0f), m_Timestep(Timestep(0.0f)) {

	}

	Engine::~Engine() {

	}
}

