#pragma once

#if EASEL_PROFILE
#ifdef EASEL_PLATFORM_WINDOWS
#define TRACY_CALLSTACK 1
#endif
#include <Tracy.hpp>
#define EASEL_PROFILE_SCOPE(name) ZoneScopedN(name)
#define EASEL_PROFILE_FUNCTION() ZoneScoped
#define EASEL_PROFILE_FRAMEMARKER() FrameMark
#define EASEL_PROFILE_LOCK(type, var, name) TracyLockableN(type, var, name)
#define EASEL_PROFILE_LOCKMARKER(var) LockMark(var)
#define EASEL_PROFILE_SETTHREADNAME(name) tracy::SetThreadName(name)
#else
#define EASEL_PROFILE_SCOPE(name)
#define EASEL_PROFILE_FUNCTION()
#define EASEL_PROFILE_FRAMEMARKER()
#define EASEL_PROFILE_LOCK(type, var, name) type var
#define EASEL_PROFILE_LOCKMARKER(var)
#define EASEL_PROFILE_SETTHREADNAME(name)
#endif
