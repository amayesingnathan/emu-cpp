module;

#include <imgui.h>

module Application;

import Gameboy;

#define BIND_EVENT_FUNC(func) [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

namespace Emu {

    void Application::Run()
    {
        Application& emu = Get();
        while (emu.mRunning)
        {
            Time::Begin();

            if (emu.mGameInstance)
                emu.mGameInstance->update();
            emu.RenderUI();
            emu.mWindow->update();

            Duration ts = Time::Elapsed();
            if (ts < emu.mEmuSettings.frameLength)
                Time::Sleep(emu.mEmuSettings.frameLength - ts);
            else
                ;//EMU_CORE_WARN("Frame running behind by {:03.2f}ms!", (ts - emu.mEmuSettings.frameLength).count());
        }
    }

    Application& Application::Get()
    {
        static Application sInstance;
        return sInstance;
    }

    Application::Application()
    {
        mWindow = new Window();

        Emu::Log::Init(mLogWidget);
        mWindow->setEventCallback(BIND_EVENT_FUNC(Application::OnEvent));
    }

    Application::~Application()
    {
        delete mGameInstance;
        delete mWindow;
    }

    void Application::RenderUI()
    {
        static bool dockspaceOpen = true;

        mWindow->beginImGui();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, windowFlags);
        ImGui::PopStyleVar(3);

        ImGuiIO& io = ImGui::GetIO();
        ImGuiStyle& style = ImGui::GetStyle();
        float minWinSizeX = style.WindowMinSize.x;
        style.WindowMinSize.x = 370.0f;

        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceID = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
        }

        style.WindowMinSize.x = minWinSizeX;

        UI_MenuBar();
        UI_Viewport();

        mLogWidget.draw();

        if (mGameInstance)
            mGameInstance->uiRender();

        ImGui::End();

        mWindow->endImGui();
    }

    void Application::UI_MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open"))
                    LaunchEmulator();
                if (ImGui::MenuItem("Reset"))
                    ResetEmulator();
                if (ImGui::MenuItem("Close"))
                    CloseEmulator();
                if (ImGui::MenuItem("Exit"))
                    mRunning = false;
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void Application::UI_Viewport()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        bool focused = ImGui::IsWindowFocused();
        bool hovered = ImGui::IsWindowHovered();
        mWindow->blockEvents(!focused && !hovered);

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        uint tex = mGameInstance ? mGameInstance->getDisplayTex() : 0;
        ImGui::Image((ImTextureID)(intptr_t)tex, viewportSize);

        ImGui::End();
        ImGui::PopStyleVar();
    }

    void Application::LaunchEmulator()
    {
        fs::path openFile = File::OpenFile({ "GB ROM (*.gb)", "*.gb" });
        if (openFile.empty())
            return;

        GetROMData(openFile);

        if (mGameInstance)
            delete mGameInstance;

        switch (mEmuSettings.type)
        {
        case EmulatorType::GB:
            std::filesystem::current_path("../Gameboy");
            mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }

        mEmuSettings.frameLength = mGameInstance->getFrameTime();
        Input::Init(mEmuSettings.ioSettings.at(mEmuSettings.type));
    }

    void Application::CloseEmulator()
    {
        delete mGameInstance;
        mGameInstance = nullptr;

        mEmuSettings.frameLength = std::chrono::milliseconds(16);
        mEmuSettings.gamePath = "";
        mEmuSettings.type = EmulatorType::None;
    }

    void Application::ResetEmulator()
    {
        if (!mGameInstance)
            return;

        delete mGameInstance;
        mGameInstance = nullptr;

        switch (mEmuSettings.type)
        {
        case EmulatorType::GB:
            std::filesystem::current_path("../Gameboy");
            mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }
    }

    void Application::GetROMData(const fs::path& file)
    {
        mEmuSettings.type = EmulatorType::None;
        mEmuSettings.gamePath = file.string();

        if (file.extension() == ".gb")
            mEmuSettings.type = EmulatorType::GB;
    }

    void Application::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNC(Application::OnWindowClose));
        if (event.handled)
            return;

        mWindow->onEvent(event);
        if (event.handled)
            return;

        dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FUNC(Application::OnKeyPressed));
        if (event.handled)
            return;

        if (mGameInstance)
            mGameInstance->onEvent(event);
    }

    bool Application::OnWindowClose(WindowCloseEvent& e)
    {
        mRunning = false;
        return true;
    }

    bool Application::OnKeyPressed(KeyPressedEvent& e)
    {
        switch (e.getKeyCode())
        {
        case Key::Escape:
            CloseApp();
            break;

        case Key::O:
        {
            bool ctrl = mWindow->isKeyPressed(Key::LeftControl) || mWindow->isKeyPressed(Key::RightControl);
            if (ctrl)
            {
                LaunchEmulator();
                return true;
            }
            break;
        }

        default:
            break;
        }
        return false;
    }
}