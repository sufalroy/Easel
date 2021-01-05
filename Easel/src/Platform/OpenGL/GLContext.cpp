#include "Precompiled.h"
#include "GLContext.h"

#include "GL.h"
#include "GLDebug.h"

#include <GLFW/glfw3.h>

#ifdef EASEL_PLATFORM_WINDOWS
#undef NOGDI
#include <glad/glad_wgl.h>
#include <Windows.h>
#define NOGDI
#endif // EASEL_PLATFORM_WINDOWS

#include <imgui.h>

#ifdef GL_DEBUG_CALLBACK

static std::string GetStringForType(GLenum type) {

	switch (type) {

	case GL_DEBUG_TYPE_ERROR:
		return"Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return "Deprecated behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return "Undefined behavior";
	case GL_DEBUG_TYPE_PORTABILITY:
		return "Portability issue";
	case GL_DEBUG_TYPE_PERFORMANCE:
		return "Performance issue";
	case GL_DEBUG_TYPE_MARKER:
		return "Stream annotation";
	case GL_DEBUG_TYPE_OTHER_ARB:
		return "Other";
	default:
		return "";
	}
}

static bool PrintMessage(GLenum type)
{
	switch (type) {

	case GL_DEBUG_TYPE_ERROR:
		return true;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		return true;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		return true;
	case GL_DEBUG_TYPE_PORTABILITY:
		return true;
	case GL_DEBUG_TYPE_PERFORMANCE:
		return false;
	case GL_DEBUG_TYPE_MARKER:
		return false;
	case GL_DEBUG_TYPE_OTHER_ARB:
		return false;
	default:
		return false;
	}
}

static std::string GetStringForSource(GLenum source) {

	switch (source) {

	case GL_DEBUG_SOURCE_API:
		return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		return "Shader compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		return "Third party";
	case GL_DEBUG_SOURCE_APPLICATION:
		return "Application";
	case GL_DEBUG_SOURCE_OTHER:
		return "Other";
	default:
		return "";
	}
}

namespace Easel {

	static std::string GetStringForSeverity(GLenum severity) {
		
		switch (severity) {

		case GL_DEBUG_SEVERITY_HIGH:
			EASEL_ASSERT(0, "");
			return "High";
		case GL_DEBUG_SEVERITY_MEDIUM:
			EASEL_ASSERT(0, "");
			return "Medium";
		case GL_DEBUG_SEVERITY_LOW:
			EASEL_ASSERT(0, "");
			return "Low";
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return "Notification";
		case GL_DEBUG_SOURCE_API:
			return "Source API";
		default:
			return("");
		}
	}

	void APIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length,
		const GLchar* message, const void* userParam) {

		if (!PrintMessage(type))
			return;

		EASEL_CORE_INFO(OPENGLLOG"Message: {0}", message);
		EASEL_CORE_INFO(OPENGLLOG"Type: {0}", GetStringForType(type));
		EASEL_CORE_INFO(OPENGLLOG"Source: {0}", GetStringForSource(source));
		EASEL_CORE_INFO(OPENGLLOG"ID: {0}", id);
		EASEL_CORE_INFO(OPENGLLOG"Severity: {0}", GetStringForSeverity(source));
	}
}

#endif // GL_DEBUG_CALLBACK


namespace Easel {

	namespace Graphics {

		GLContext::GLContext(const WindowProperties& properties, Window* window)
		{

#if defined(EASEL_PLATFORM_WINDOWS) && !defined(EASEL_USE_GLFW_WINDOWS)
			HDC hDc = GetDC(static_cast<HWND>(window->GetHandle()));

			HGLRC tempContext = wglCreateContext(hDc);
			wglMakeCurrent(hDc, tempContext);

			if (!wglMakeCurrent(hDc, tempContext)) {

				EASEL_CORE_ERROR("Failed to initialize OpenGL context");
			}

			if (!gladLoadWGL(hDc))
				EASEL_CORE_ERROR("glad failed to load WGL!");
			if (!gladLoadGL())
				EASEL_CORE_ERROR("glad failed to load OpenGL!");

			const int contextAttribsList[] =
			{
				WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
				WGL_CONTEXT_MINOR_VERSION_ARB, 4,
				WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		#ifdef _DEBUG
				WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		#else
				WGL_CONTEXT_FLAGS_ARB, 0,
		#endif
				0,
			};

			HGLRC hrc = wglCreateContextAttribsARB(hDc, nullptr, contextAttribsList);
			if (hrc == nullptr) {

				EASEL_CORE_ERROR("Failed to create core OpenGL context");
			}
			else {

				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(tempContext);
				wglMakeCurrent(hDc, hrc);
			}
#else
			if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {

				EASEL_CORE_ERROR("Failed to initialize OpenGL context");
			}
#endif

			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

			EASEL_CORE_INFO("----------------------------------");
			EASEL_CORE_INFO(OPENGLLOG);
			EASEL_CORE_INFO(glGetString(GL_VERSION));
			EASEL_CORE_INFO(glGetString(GL_VENDOR));
			EASEL_CORE_INFO(glGetString(GL_RENDERER));
			EASEL_CORE_INFO("----------------------------------");

#if EASEL_DEBUG
#ifdef GL_DEBUG_CALLBACK
			EASEL_CORE_INFO(OPENGLLOG"Registering OpenGL debug callback");

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(openglCallbackFunction, nullptr);
			GLuint unusedIds = 0;
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
#endif
#endif
		}

		GLContext::~GLContext() = default;

		void GLContext::Present() {

		}

		void GLContext::OnImGui() {
			ImGui::TextUnformatted("%s", (const char*)(glGetString(GL_VERSION)));
			ImGui::TextUnformatted("%s", (const char*)(glGetString(GL_VENDOR)));
			ImGui::TextUnformatted("%s", (const char*)(glGetString(GL_RENDERER)));
		}

		void GLContext::MakeDefault() {
			CreateFunc = CreateFuncGL;
		}

		GraphicsContext* GLContext::CreateFuncGL(const WindowProperties& properties, Window* cont) {
			return new GLContext(properties, cont);
		}
	}
}

