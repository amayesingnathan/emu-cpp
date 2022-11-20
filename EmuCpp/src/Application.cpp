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
        Renderer::Init();
        mImGuiHandler = new ImGuiHandler(mWindow);

        switch (mEmuSettings.type)
        {
        case EmulatorType::GB:
            std::filesystem::current_path("../Gameboy");
            mGameInstance = new GB::Gameboy(mEmuSettings.gamePath);
            break;

        default:
            assert(false);
        }

        mImGuiHandler->setFBO(mGameInstance->getFBO());
        mWindow->setActionCallback(mGameInstance->getActionCallback());
    }

    Application::~Application()
    {
        delete mGameInstance;
        delete mImGuiHandler;
        delete mWindow;
    }
}