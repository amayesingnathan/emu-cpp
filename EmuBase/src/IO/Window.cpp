#include "glpch.h"
#include "Window.h"

#include "GLFW/glfw3.h"

#include "Graphics/Context.h"
#include "ImGui/ImGuiHandler.h"

namespace Emu {

	static uint sGLFWWindowCount = 0;

	static void GLFWErrorCallback(int error, const char* description)
	{
		std::cout << "GLFW Error (" << error << "): " << description;
	}


	Window::Window(const WindowProps& props)
	{
		Init(props);
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::update()
	{
		glfwPollEvents();
		mContext->swapBuffers();
	}

	void Window::beginImGui() { mImGuiHandler->begin(); }
	void Window::endImGui()   { mImGuiHandler->end(); }
	void Window::onEvent(Event& event)   { mImGuiHandler->onEvent(event); }
	void Window::blockEvents(bool block) { mImGuiHandler->blockEvents(block); }

	void Window::setTitle(const std::string& title)
	{
		mData.title = title;
		glfwSetWindowTitle(mWindow, title.c_str());
	}

	void Window::setVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		mData.vSync = enabled;
	}

	bool Window::isVSync() const
	{
		return mData.vSync;
	}

	bool Window::isKeyPressed(KeyCode key)
	{
		return (glfwGetKey(mWindow, static_cast<int32_t>(key)) == GLFW_PRESS);
	}



	void Window::Init(const WindowProps& props)
	{
		mData.title = props.title;
		mData.width = props.width;
		mData.height = props.height;

		if (sGLFWWindowCount == 0)
		{
			int success = glfwInit();
			GL_ASSERT(success, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
		}

		if (props.fullscreen)
		{	// Set Window size to be full size of primary monitor and set borderless (prefer this to actual fullscreen)
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			mData.width = mode->width;
			mData.height = mode->height;
			glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		}

		// TODO LOG ("Creating window {0} ({1}, {2})", mData.title, mData.width, mData.height);

#if defined(GL_DEBUG)
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		mWindow = glfwCreateWindow((int)mData.width, (int)mData.height, mData.title.c_str(), nullptr, nullptr);
		GL_ASSERT(mWindow, "Could not create GLFW window!");

		++sGLFWWindowCount;
		mContext = new GraphicsContext(mWindow);

		glfwSetWindowUserPointer(mWindow, &mData);
		setVSync(true);

		RegisterCallbacks();

		mImGuiHandler = new ImGuiHandler(this);
	}

	void Window::Shutdown()
	{
		delete mImGuiHandler;
		delete mContext;

		glfwDestroyWindow(mWindow);
		--sGLFWWindowCount;

		if (sGLFWWindowCount == 0)
			glfwTerminate();
		// TODO LOG ("Shutdown complete");
	}

	void Window::RegisterCallbacks()
	{
		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key);
				data.eventCallback(event);
				break;
			}

			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.eventCallback(event);
				break;
			}
			};
		});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.eventCallback(event);
		});
	}
}