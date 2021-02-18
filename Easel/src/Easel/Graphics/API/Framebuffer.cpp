#include "Precompiled.h"
#include "Easel/Graphics/API/Framebuffer.h"

#include "Easel/Graphics/API/Texture.h"
#include "Easel/Graphics/API/GraphicsContext.h"

#include "Easel/Utils/CombineHash.h"

namespace Easel {

	namespace Graphics {

		Framebuffer* (*Framebuffer::CreateFunc)(const FramebufferInfo&) = nullptr;

		Framebuffer* Framebuffer::Create(const FramebufferInfo& framebufferInfo) {
			EASEL_ASSERT(CreateFunc, "No Framebuffer Create Function");
			return CreateFunc(framebufferInfo);
		}

		static std::unordered_map<std::size_t, Ref<Framebuffer>> m_FramebufferCache;

		Ref<Framebuffer> Framebuffer::Get(const FramebufferInfo& framebufferInfo) {

			size_t hash = 0;
			HashCombine(hash, framebufferInfo.attachmentCount, framebufferInfo.width, framebufferInfo.height, framebufferInfo.layer, framebufferInfo.renderPass, framebufferInfo.screenFBO);

			for(uint32_t i = 0; i < framebufferInfo.attachmentCount; i++)
				HashCombine(hash, framebufferInfo.attachmentTypes[i], framebufferInfo.attachments[i]);

			auto found = m_FramebufferCache.find(hash);
			if (found != m_FramebufferCache.end() && found->second)
				return found->second; //Disable until fix resize issue.

			auto framebuffer = Ref<Framebuffer>(Create(framebufferInfo));
			m_FramebufferCache[hash] = framebuffer;
			return framebuffer;
		}

		void Framebuffer::ClearCache() {
			m_FramebufferCache.clear();
		}

		void Framebuffer::DeleteUnusedCache() {
			for (const auto& [key, value] : m_FramebufferCache) {
				if (value && value.GetCounter()->GetReferenceCount() == 1)
					m_FramebufferCache[key] = nullptr;
			}
		}

		Framebuffer::~Framebuffer() {

		}
	}
}

