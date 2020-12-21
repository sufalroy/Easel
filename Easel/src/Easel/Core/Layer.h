#pragma once

#include "Easel/Core/Base.h"
#include "Easel/Events/Event.h"
#include "Easel/Utils/Timestep.h"

namespace Easel {

	class EASEL_EXPORT Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		_FORCE_INLINE_ const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
