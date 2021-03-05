#include "Precompiled.h"

#include "GLFWWindow.h"
#include "Easel/Graphics/API/GraphicsContext.h"
#include "Easel/Utils/LoadImage.h"

#include "GLFWKeyCodes.h"
#include "GLFWMouseCodes.h"

#include "Easel/Core/OS/Input.h"
#include "Easel/Core/Application.h"

#include "Easel/Events/ApplicationEvent.h"
#include "Easel/Events/MouseEvent.h"
#include "Easel/Events/KeyEvent.h"

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <imgui.h>

static GLFWcursor* g_MouseCursors[ImGuiMouseCursor_COUNT] = { 0 };

namespace Easel {

	static bool s_GLFWInitialized = false;
	static int s_NumGLFWWindows = 0;

	static void GLFWErrorCallback(int error, const char* description) {
		EASEL_CORE_ERROR("GLFW Error - {0} : {1}", error, description);
	}

	GLFWWindow::GLFWWindow(const WindowProperties& properties) {

		m_Init = false;
		m_VSync = properties.VSync;

		EASEL_CORE_INFO("VSync : {0}", m_VSync ? "True" : "False");
		m_HasResized = true;
		m_Data.m_RenderAPI = static_cast<Graphics::RenderAPI>(properties.RenderAPI);

		m_Init = Init(properties);

		Graphics::GraphicsContext::SetRenderAPI(static_cast<Graphics::RenderAPI>(properties.RenderAPI));
		Graphics::GraphicsContext::Create(properties, this);
		Graphics::GraphicsContext::GetContext()->Init();
	}

	GLFWWindow::~GLFWWindow() {
		for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++) {
			glfwDestroyCursor(g_MouseCursors[cursor_n]);
			g_MouseCursors[cursor_n] = NULL;
		}

		glfwDestroyWindow(m_Handle);

		Graphics::GraphicsContext::Release();

		s_NumGLFWWindows--;

		if (s_NumGLFWWindows < 1) {
			s_GLFWInitialized = false;
			glfwTerminate();
		}
	}

	bool GLFWWindow::Init(const WindowProperties& properties) {
		
		EASEL_PROFILE_FUNCTION();
		EASEL_CORE_INFO("Creating window - Title : {0}, Width : {1}, Height : {2}", properties.Title, properties.Width, properties.Height);

		if (!s_GLFWInitialized) {
			int success = glfwInit();
			EASEL_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			s_GLFWInitialized = true;
		}
		
		s_NumGLFWWindows++;

#ifdef EASEL_RENDER_API_OPENGL
		if (m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL) {

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		}
#endif // EASEL_RENDER_API_OPENGL

		SetBorderlessWindow(properties.Borderless);

		const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

		uint32_t ScreenWidth = 0;
		uint32_t ScreenHeight = 0;

		if (properties.Fullscreen) {
			ScreenWidth = mode->width;
			ScreenHeight = mode->height;
		}
		else {
			ScreenWidth = properties.Width;
			ScreenHeight = properties.Height;
		}

		m_Data.Title = properties.Title;
		m_Data.Width = ScreenWidth;
		m_Data.Height = ScreenHeight;
		m_Data.Exit = false;

		m_Handle = glfwCreateWindow(ScreenWidth, ScreenHeight, properties.Title.c_str(), nullptr, nullptr);

#ifdef EASEL_RENDER_API_OPENGL
		if (m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL)
			glfwMakeContextCurrent(m_Handle);
#endif // EASEL_RENDER_API_OPENGL

		glfwSetWindowUserPointer(m_Handle, &m_Data);

#ifdef EASEL_PLATFORM_WINDOWS
		SetIcon("//Textures/icon.png", "//Textures/icon32.png");
#endif

		glfwSetWindowPos(m_Handle, mode->width / 2 - ScreenWidth / 2, mode->height / 2 - ScreenHeight / 2);
		glfwSetInputMode(m_Handle, GLFW_STICKY_KEYS, true);

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Handle, [](GLFWwindow* window, int width, int height) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Handle, [](GLFWwindow* window) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));
			WindowCloseEvent event;
			data.EventCallback(event);
			data.Exit = true;
		});

		glfwSetWindowFocusCallback(m_Handle, [](GLFWwindow* window, int focused) {
			Input::GetInput()->SetWindowFocus(focused);
		});

		glfwSetKeyCallback(m_Handle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			switch (action) {

			case GLFW_PRESS: {
				KeyPressedEvent event(GLFWKeyCodes::GLFWToEaselKeyboardKey(key), 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				KeyReleasedEvent event(GLFWKeyCodes::GLFWToEaselKeyboardKey(key));
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT: {
				KeyPressedEvent event(GLFWKeyCodes::GLFWToEaselKeyboardKey(key), 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetMouseButtonCallback(m_Handle, [](GLFWwindow* window, int button, int action, int mods) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			switch (action) {

			case GLFW_PRESS: {
				MouseButtonPressedEvent event(GLFWMouseCodes::GLFWToEaselMouseKey(button));
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE: {
				MouseButtonReleasedEvent event(GLFWMouseCodes::GLFWToEaselMouseKey(button));
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Handle, [](GLFWwindow* window, double xOffset, double yOffset) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Handle, [](GLFWwindow* window, double xPos, double yPos) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});

		glfwSetCursorEnterCallback(m_Handle, [](GLFWwindow* window, int enter) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			MouseEnterEvent event(enter > 0);
			data.EventCallback(event);
		});

		glfwSetCharCallback(m_Handle, [](GLFWwindow* window, unsigned int keycode) {
			WindowData& data = *static_cast<WindowData*>((glfwGetWindowUserPointer(window)));

			KeyTypedEvent event(GLFWKeyCodes::GLFWToEaselKeyboardKey(keycode), char(keycode));
			data.EventCallback(event);
		});

		g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
		g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);

		EASEL_CORE_INFO("Initialised GLFW version : {0}", glfwGetVersionString());

		return true;
	}

	void GLFWWindow::SetIcon(const std::string& file, const std::string& smallIconFilePath) {

		uint32_t width, height;
		uint8_t* pixels = Easel::LoadImageFromFile(file, &width, &height, nullptr, nullptr, true);

		std::vector<GLFWimage> images;
		GLFWimage image;
		image.height = height;
		image.width = width;
		image.pixels = static_cast<unsigned char*>(pixels);
		images.push_back(image);

		if (smallIconFilePath != "") {
			pixels = Easel::LoadImageFromFile(smallIconFilePath, &width, &height, nullptr, nullptr, true);
			image.height = height;
			image.width = width;
			image.pixels = static_cast<unsigned char*>(pixels);
			images.push_back(image);
		}

		glfwSetWindowIcon(m_Handle, int(images.size()), images.data());

		delete[] pixels;
	}

	void GLFWWindow::SetWindowTitle(const std::string& title) {
		
		EASEL_PROFILE_FUNCTION();
		glfwSetWindowTitle(m_Handle, title.c_str());
	}

	void GLFWWindow::ToggleVSync() {

		EASEL_PROFILE_FUNCTION();
		if (m_VSync) {
			SetVSync(false);
		}
		else {
			SetVSync(true);
		}

		EASEL_CORE_INFO("VSync : {0}", m_VSync ? "True" : "False");
	}

	void GLFWWindow::SetVSync(bool set) {

		EASEL_PROFILE_FUNCTION();
		if (set) {
			m_VSync = true;
			glfwSwapInterval(1);
		}
		else {
			m_VSync = false;
			glfwSwapInterval(0);
		}

		EASEL_CORE_INFO("VSync : {0}", m_VSync ? "True" : "False");
	}

	void GLFWWindow::OnUpdate() {

		EASEL_PROFILE_FUNCTION();
#ifdef EASEL_RENDER_API_OPENGL
		if (m_Data.m_RenderAPI == Graphics::RenderAPI::OPENGL) {
			EASEL_PROFILE_SCOPE("GLFW SwapBuffers");
			glfwSwapBuffers(m_Handle);
		}	
#endif
		{
			EASEL_PROFILE_SCOPE("GLFW PollEvents");
			glfwPollEvents();
		}
	}

	void GLFWWindow::SetBorderlessWindow(bool borderless) {

		EASEL_PROFILE_FUNCTION();
		if (borderless) {
			glfwWindowHint(GLFW_DECORATED, false);
		}
		else {
			glfwWindowHint(GLFW_DECORATED, true);
		}
	}

	void GLFWWindow::HideMouse(bool hide) {

		EASEL_PROFILE_FUNCTION();
		if (hide) {
			glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else {
			glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	void GLFWWindow::SetMousePosition(const Maths::Vector2& pos) {
		
		EASEL_PROFILE_FUNCTION();
		glfwSetCursorPos(m_Handle, pos.x, pos.y);
	}

	void GLFWWindow::MakeDefault() {

		CreateFunc = CreateFuncGLFW;
	}

	Window* GLFWWindow::CreateFuncGLFW(const WindowProperties& properties) {
		return new GLFWWindow(properties);
	}

	void GLFWWindow::UpdateCursorImGui() {
		
		EASEL_PROFILE_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();

		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(m_Handle, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
			return;

		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else {
			glfwSetCursor(m_Handle, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(m_Handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
}

