#include "Precompiled.h"
#include "Easel/Graphics/API/DescriptorSet.h"

namespace Easel {

	namespace Graphics {

		DescriptorSet* (*DescriptorSet::CreateFunc)(const DescriptorInfo&) = nullptr;

		DescriptorSet* DescriptorSet::Create(const DescriptorInfo& info) {
			EASEL_ASSERT(CreateFunc, "No DescriptorSet Create Function");

			return CreateFunc(info);
		}
	}
}

