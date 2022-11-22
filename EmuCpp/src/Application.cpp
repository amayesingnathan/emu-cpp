#include "Application.h"

#include "Gameboy.h"

namespace Emu {

    void Application::Run()
    {
        Application& emu = Get();

        while (emu.mRunning)
        {
            emu.mGameInstance->update();
            emu.mImGuiHandler->update();
            emu.mWindow->update();
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

        uint displayWidth, displayHeight;
        switch (mEmuSettings.type)
        {
        case EmulatorType::GB:
            std::filesystem::current_path("../Gameboy");
            displayWidth = 160;
            displayHeight = 144;
            mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }

        std::filesystem::current_path("../EmuCpp");
        Renderer::Init(displayWidth, displayHeight);
        mImGuiHandler = new ImGuiHandler(mWindow);

        mImGuiHandler->setFBO(mGameInstance->getFBO());
        mWindow->setActionCallback(mGameInstance->getActionCallback());
    }

    Application::~Application()
    {
        Renderer::Shutdown();

        delete mGameInstance;
        delete mImGuiHandler;
        delete mWindow;
    }
}