#pragma once

#ifndef EASEL_PLATFORM_WINDOWS
#include <signal.h>
#endif

#include <stdint.h>

#ifdef EASEL_PLATFORM_WINDOWS

#define MEM_ALIGNMENT 16
#define MEM_ALIGN __declspec(align(MEM_ALIGNMENT))

#else

#define MEM_ALIGNMENT 16
#define MEM_ALIGN __attribute__((aligned(MEM_ALIGNMENT)))

#endif


#ifdef EASEL_PLATFORM_WINDOWS
#pragma warning (disable:4251)
#ifdef EASEL_DYNAMIC
#ifdef EASEL_ENGINE
#define EASEL_EXPORT __declspec(dllexport)
#else
#define EASEL_EXPORT __declspec(dllimport)
#endif
#else
#define EASEL_EXPORT
#endif
#define EASEL_HIDDEN
#else
#define EASEL_EXPORT __attribute__ ((visibility ("default")))
#define EASEL_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

#define BIT(x) (1 << x)

#define NUMBONES 64
#define INPUT_BUF_SIZE 128

#define SAFE_DELETE(mem) { if(mem) { delete mem; mem = NULL; } }
#define SAFE_UNLOAD(mem, ...) { if(mem) { mem->Unload(__VA_ARGS__); delete mem; mem = NULL; } }

#ifdef EASEL_DEBUG
#define EASEL_DEBUG_METHOD(x) x;
#define EASEL_DEBUG_METHOD_CALL(x) x;
#else
#define EASEL_DEBUG_METHOD(x) x {}
#define EASEL_DEBUG_METHOD_CALL(x);
#endif

#define MAX_OBJECTS 2048

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define ROOT_DIR STRINGIZE(EASEL_ROOT_DIR)

#if EASEL_PLATFORM_WINDOWS
#define EASEL_BREAK() __debugbreak();
#else
#define EASEL_BREAK() raise(SIGTRAP);
#endif

#ifdef EASEL_DEBUG
#define EASEL_ENABLE_ASSERTS
#endif

#define HEX2CHR(m_hex) \
((m_hex >= '0' && m_hex <= '9') ? (m_hex - '0') : \
((m_hex >= 'A' && m_hex <= 'F') ? (10 + m_hex - 'A') : \
((m_hex >= 'a' && m_hex <= 'f') ? (10 + m_hex - 'a') : 0)))

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
#endif

#ifdef EASEL_ENGINE
#define EASEL_ASSERT EASEL_CORE_ASSERT
#else
#define EASEL_ASSERT EASEL_CLIENT_ASSERT
#endif

#define UNIMPLEMENTED { EASEL_ERROR("Unimplemented : {0} : {1}", __FILE__, __LINE__); EASEL_BREAK(); }

#define NONCOPYABLE(type_identifier)								\
    type_identifier(const type_identifier&) = delete;				\
    type_identifier& operator=(const type_identifier&) = delete;

#if defined(_MSC_VER)
#define DISABLE_WARNING_PUSH           __pragma(warning( push ))
#define DISABLE_WARNING_POP            __pragma(warning( pop ))
#define DISABLE_WARNING(warningNumber) __pragma(warning( disable : warningNumber ))

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(4100)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(4505)
#define DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE       DISABLE_WARNING(4267)

#elif defined(__GNUC__) || defined(__clang__)
#define DO_PRAGMA(X) _Pragma(#X)
#define DISABLE_WARNING_PUSH           DO_PRAGMA(GCC diagnostic push)
#define DISABLE_WARNING_POP            DO_PRAGMA(GCC diagnostic pop)
#define DISABLE_WARNING(warningName)   DO_PRAGMA(GCC diagnostic ignored #warningName)

#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER    DISABLE_WARNING(-Wunused-parameter)
#define DISABLE_WARNING_UNREFERENCED_FUNCTION            DISABLE_WARNING(-Wunused-function)
#define DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE       DISABLE_WARNING(-Wconversion)

#else
#define DISABLE_WARNING_PUSH
#define DISABLE_WARNING_POP
#define DISABLE_WARNING_UNREFERENCED_FORMAL_PARAMETER
#define DISABLE_WARNING_UNREFERENCED_FUNCTION
#define DISABLE_WARNING_CONVERSION_TO_SMALLER_TYPE
#endif
