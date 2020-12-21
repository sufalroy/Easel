#pragma once

#ifndef EASEL_PLATFORM_WINDOWS
	#include <signal.h>
#endif // !EASEL_PLATFORM_WINDOWS

#include <stdint.h>

#ifdef EASEL_PLATFORM_WINDOWS
	#define MEM_ALIGNMENT 16
	#define MEM_ALIGN __declspec(align(MEM_ALIGNMENT))
#else
	#define MEM_ALIGNMENT 16
	#define MEM_ALIGN __attribute__((aligned(MEM_ALIGNMENT)))
#endif // EASEL_PLATFORM_WINDOWS


#ifdef EASEL_PLATFORM_WINDOWS
	#pragma warning (disable:4251)
	#ifdef EASEL_DYNAMIC
		#ifdef EASEL_ENGINE
			#define EASEL_EXPORT __declspec(dllexport)
		#else
			#define EASEL_EXPORT __declspec(dllimport)
		#endif // EASEL_ENGINE
	#else
		#define EASEL_EXPORT
	#endif // EASEL_DYNAMIC
	#define EASEL_HIDDEN
	#else
		#define EASEL_EXPORT __attribute__ ((visibility ("default")))
		#define EASEL_HIDDEN __attribute__ ((visibility ("hidden")))
#endif // EASEL_PLATFORM_WINDOWS

#define BIT(x) (1 << x)

#define SAFE_DELETE(mem) { if(mem) { delete mem; mem = NULL; } }
#define SAFE_UNLOAD(mem, ...) { if(mem) { mem->Unload(__VA_ARGS__); delete mem; mem = NULL; } }


#ifdef EASEL_DEBUG
	#define EASEL_DEBUG_METHOD(x) x;
	#define EASEL_DEBUG_METHOD_CALL(x) x;
#else
	#define EASEL_DEBUG_METHOD(x) x {}
	#define EASEL_DEBUG_METHOD_CALL(x);
#endif // EASEL_DEBUG

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define ROOT_DIR STRINGIZE(EASEL_ROOT_DIR)

#if EASEL_PLATFORM_WINDOWS
	#define EASEL_BREAK() __debugbreak();
#else
	#define EASEL_BREAK() raise(SIGTRAP);
#endif // EASEL_PLATFORM_WINDOWS

#ifdef EASEL_DEBUG
	#define EASEL_ENABLE_ASSERTS
#endif // EASEL_DEBUG

#ifndef _ALWAYS_INLINE_
	#if defined(__GNUC__) && (__GNUC__ >= 4)
		#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
	#elif defined(__llvm__)
		#define _ALWAYS_INLINE_ __attribute__((always_inline)) inline
	#elif defined(_MSC_VER)
		#define _ALWAYS_INLINE_ __forceinline
	#else
		#define _ALWAYS_INLINE_ inline
	#endif
#endif

#ifndef _FORCE_INLINE_
	#ifdef DISABLE_FORCED_INLINE
		#define _FORCE_INLINE_ inline
	#else
		#define _FORCE_INLINE_ _ALWAYS_INLINE_
	#endif // DISABLE_FORCED_INLINE
#endif // !_FORCE_INLINE_

//TODO: Segregate error messages for Core and Client 
#ifdef EASEL_ENABLE_ASSERTS
	#define EASEL_ASSERT_NO_MESSAGE(condition) { if(!(condition)) { EASEL_ERROR("Assertion Failed!"); EASEL_BREAK(); }}
	#define EASEL_ASSERT_MESSAGE(condition, ...) { if(!(condition)) { EASEL_ERROR("Assertion Failed : {0}", __VA_ARGS__); EASEL_BREAK(); }}
	#define EASEL_CLIENT_ASSERT EASEL_ASSERT_MESSAGE
	#define EASEL_CORE_ASSERT EASEL_ASSERT_MESSAGE
#else
	#define EASEL_CLIENT_ASSERT(...)
	#define EASEL_CORE_ASSERT(...)
	#define EASEL_ASSERT_NO_MESSAGE(...)
	#define EASEL_ASSERT_MESSAGE(condition)
#endif // EASEL_ENABLE_ASSERTS

#ifdef EASEL_ENGINE
	#define EASEL_ASSERT EASEL_CORE_ASSERT
#else
	#define EASEL_ASSERT EASEL_CLIENT_ASSERT
#endif // EASEL_ENGINE

#define UNIMPLEMENTED { EASEL_ERROR("Unimplemented : {0} : {1}", __FILE__, __LINE__); EASEL_BREAK(); }

#define NONCOPYABLE(type_identifier)								\
    type_identifier(const type_identifier&) = delete;				\
    type_identifier& operator=(const type_identifier&) = delete;
