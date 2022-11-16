#pragma once

#include "Core/Common.h"
#include "OpenGL/Context.h"

struct GLFWwindow;

namespace GL {

	struct WindowProps {
		std::string title;
		uint width;
		uint height;
		bool fullscreen;

		WindowProps(const std::string& t = "emu-cpp",
			uint w = 1600,
			uint h = 900,
			bool f = false)
			: title(t), width(w), height(h), fullscreen(f) {}
	};

	class Window
	{
	public:

		Window(const WindowProps& props);
		~Window();

		void onUpdate();

		inline uint getWidth() const { return mData.width; }
		inline uint getHeight() const { return mData.height; }

		void setTitle(const std::string& title);

		//Attributes
		void setVSync(bool enabled);
		bool isVSync() const;

		inline GLFWwindow* getNativeWindow() const { return mWindow; }
		inline GraphicsContext* getNativeContext() const { return mContext; }

	private:
		void init(const WindowProps& props);
		void shutdown();

	private:
		GLFWwindow* mWindow;
		GraphicsContext* mContext;

		struct WindowData
		{
			std::string title;
			uint width, height;
			bool vSync;
		};

		WindowData mData;
	};

}