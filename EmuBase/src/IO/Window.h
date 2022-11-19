#pragma once

#include "Core/Common.h"

struct GLFWwindow;

namespace Emu {

	namespace GL { class GraphicsContext; }

	struct WindowProps {
		std::string title;
		uint width;
		uint height;
		bool fullscreen;

		WindowProps(const std::string& t = "emu-cpp",
			uint w = 800,
			uint h = 600,
			bool f = false)
			: title(t), width(w), height(h), fullscreen(f) {}
	};

	class Window
	{
	public:

		Window(const WindowProps& props = WindowProps());
		~Window();

		void onUpdate();

		inline uint getWidth() const { return mData.width; }
		inline uint getHeight() const { return mData.height; }

		void setTitle(const std::string& title);

		//Attributes
		void setVSync(bool enabled);
		bool isVSync() const;

		GLFWwindow* getNativeWindow() const { return mWindow; }
		GL::GraphicsContext* getNativeContext() const { return mContext; }

	private:
		void Init(const WindowProps& props);
		void Shutdown();

	private:
		GLFWwindow* mWindow;
		GL::GraphicsContext* mContext;

		struct WindowData
		{
			std::string title;
			uint width, height;
			bool vSync;
		};

		WindowData mData;
	};

}