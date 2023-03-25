module;

#include "glad/glad.h"
#include "GLFW/glfw3.h"

export module EmuBase.IO.Window;

import EmuBase.Core.Common;
import EmuBase.IO.KeyCodes;
import EmuBase.IO.Event;
import EmuBase.Graphics.Context;

export import <functional>;
export import <string>;
export import <iostream>;

struct GLFWwindow;

export namespace Emu {

	class ImGuiHandler;

	using EventCallback = std::function<void(Event&)>;

	struct WindowProps 
	{
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

		void update();
		void beginImGui();
		void endImGui();

		void onEvent(Event& event);
		void blockEvents(bool block);

		void setEventCallback(const EventCallback& callback) { mData.eventCallback = callback; }

		inline uint getWidth() const { return mData.width; }
		inline uint getHeight() const { return mData.height; }

		void setTitle(const std::string& title);

		//Attributes
		void setVSync(bool enabled);
		bool isVSync() const;

		bool isKeyPressed(KeyCode key);

		GLFWwindow* getNativeWindow() const { return mWindow; }
		GraphicsContext* getNativeContext() const { return mContext; }

	private:
		void Init(const WindowProps& props);
		void Shutdown();

		void RegisterCallbacks();

	private:
		GLFWwindow* mWindow;
		GraphicsContext* mContext;
		ImGuiHandler* mImGuiHandler;

		struct WindowData
		{
			std::string title;
			uint width, height;
			bool vSync;

			EventCallback eventCallback;
		};

		WindowData mData;
	};

}