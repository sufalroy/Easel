#pragma once

#include "Easel/Core/Base.h"

namespace Easel {

	namespace Internal {
		// Low-level System operations
		class EASEL_EXPORT CoreSystem {
		public:
			static void Init(bool enableProfiler = false);
			static void Shutdown();
		};
	}
}
