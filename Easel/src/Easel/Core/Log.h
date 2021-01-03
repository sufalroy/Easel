#pragma once

#ifdef EASEL_PLATFORM_WINDOWS
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif // !NOMINMAX
#endif // EASEL_PLATFORM_WINDOWS


#include "Easel/Core/Base.h"
#include "Easel/Core/ReferenceCounter.h"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)


//Core Log Macros
#define EASEL_CORE_TRACE(...)    SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::trace, __VA_ARGS__)
#define EASEL_CORE_INFO(...)     SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::info, __VA_ARGS__)
#define EASEL_CORE_WARN(...)     SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::warn, __VA_ARGS__)
#define EASEL_CORE_ERROR(...)    SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::err, __VA_ARGS__)
#define EASEL_CORE_CRITICAL(...) SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetCoreLogger(), spdlog::level::level_enum::critical, __VA_ARGS__)
		
//Client Log Macros		
#define EASEL_TRACE(...)    SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetClientLogger(), spdlog::level::level_enum::trace, __VA_ARGS__)
#define EASEL_INFO(...)     SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetClientLogger(), spdlog::level::level_enum::info, __VA_ARGS__)
#define EASEL_WARN(...)     SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetClientLogger(), spdlog::level::level_enum::warn, __VA_ARGS__)
#define EASEL_ERROR(...)    SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetClientLogger(), spdlog::level::level_enum::err, __VA_ARGS__)
#define EASEL_CRITICAL(...) SPDLOG_LOGGER_CALL(::Easel::Debug::Log::GetClientLogger(), spdlog::level::level_enum::critical, __VA_ARGS__)


namespace Easel {

	namespace Debug {
		
		class EASEL_EXPORT Log {
		public:
			static void OnInit();
			static void OnRelease();

			_FORCE_INLINE_ static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
			_FORCE_INLINE_ static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
		private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
		};
	}
}
