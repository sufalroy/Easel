#pragma once

#include "GLRenderer.h"
#include "GL.h"

namespace Easel {

#ifdef EASEL_DEBUG
	#ifdef glDebugMessageCallback
		#define GL_DEBUG_CALLBACK 1
	#else
		#define GL_DEBUG 1
	#endif
#endif // EASEL_DEBUG

#define OPENGLLOG "[OPENGL] - "

#ifdef GL_DEBUG
	bool GLLogCall(const char* function, const char* file, const int32_t line);
	void GLClearError();
#endif // GL_DEBUG

}

#ifdef GL_DEBUG
#define GLCall(x) GLClearError();\
		x;\
		if(!GLLogCall(#x, __FILE__, __LINE__)) EASEL_BREAK();
#else
#define GLCall(x) x
#endif // GL_DEBUG
