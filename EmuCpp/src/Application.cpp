#include "Application.h"

#include "imgui.h"

#include "Gameboy.h"

namespace Emu {

    void Application::Run()
    {
        Application& emu = Get();
        const Duration FrameLength = emu.mGameInstance->getFrameTime();

        while (emu.mRunning)
        {
            Time::Begin();

            emu.mGameInstance->update();
            emu.RenderUI();
            emu.mWindow->update();

            Duration ts = Time::Elapsed();
            if (ts < FrameLength)
                Time::Sleep(FrameLength - ts);
            else
                EMU_CORE_WARN("Frame running behind by {:03.2f}ms!", (ts - FrameLength).count());
        }
    }

    Application& Application::Get()
    {
        static Application sInstance;
        return sInstance;
    }

    Application::Application()
    {
        Input::Init(mEmuSettings.ioSettings.at(mEmuSettings.type));

        mWindow = new Window();
        mImGuiHandler = new ImGuiHandler(mWindow);

        uint displayWidth, displayHeight;
        std::string emuName;
        switch (mEmuSettings.type)
        {
        case EmulatorType::GB:
            std::filesystem::current_path("../Gameboy");
            displayWidth = 160;
            displayHeight = 144;
            emuName = "DMG";
            mGameInstance = new GB::Gameboy(mWindow, mEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }

        std::filesystem::current_path("../EmuCpp");

        mWindow->setActionCallback(mGameInstance->getActionCallback());

        Emu::Log::Init(emuName, mLogWidget);
    }

    Application::~Application()
    {
        delete mGameInstance;
        delete mImGuiHandler;
        delete mWindow;
    }

    void Application::RenderUI()
    {
        static bool dockspaceOpen = true;

        mImGuiHandler->begin();

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
        mGameInstance->imguiRender();

        ImGui::End();

        mImGuiHandler->end();
    }

    void Application::UI_MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
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

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        ImGui::Image((ImTextureID)(intptr_t)mGameInstance->getDisplayTex(), viewportSize, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        ImGui::End();
        ImGui::PopStyleVar();
    }
}