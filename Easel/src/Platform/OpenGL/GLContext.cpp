#include "Precompiled.h"
#include "GLContext.h"

#include "GL.h"


#include <GLFW/glfw3.h>

#ifdef EASEL_PLATFORM_WINDOWS
#undef NOGDI
#include <glad/glad_wgl.h>
#include <Windows.h>
#define NOGDI
#endif // EASEL_PLATFORM_WINDOWS

#include <imgui.h>

namespace Easel {

	namespace Graphics {

		GLContext::GLContext(const WindowProperties& properties, Window* window)
		{

#if defined(EASEL_PLATFORM_WINDOWS) && !defined(EASEL_USE_GLFW_WINDOWS)
			HDC hDc = GetDC(static_cast<HWND>(window->GetHandle()));

			HGLRC tempContext = wglCreateContext(hDc);
			wglMakeCurrent(hDc, tempContext);

			if (!wglMakeCurrent(hDc, tempContext))
			{
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
			if (hrc == nullptr)
			{
				EASEL_CORE_ERROR("Failed to create core OpenGL context");
			}
			else
			{
				wglMakeCurrent(NULL, NULL);
				wglDeleteContext(tempContext);
				wglMakeCurrent(hDc, hrc);
			}
#else
			if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
			{
				EASEL_CORE_ERROR("Failed to initialize OpenGL context");
			}
#endif

			glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

			EASEL_CORE_INFO("----------------------------------");
			EASEL_CORE_INFO(glGetString(GL_VERSION));
			EASEL_CORE_INFO(glGetString(GL_VENDOR));
			EASEL_CORE_INFO(glGetString(GL_RENDERER));
			EASEL_CORE_INFO("----------------------------------");
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

