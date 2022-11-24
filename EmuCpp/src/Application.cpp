#include "Application.h"

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
            emu.mWindow->update();

            Duration ts = Time::Elapsed();
            if (ts < FrameLength)
                Time::Sleep(FrameLength - ts);
            else
                ; // Debug output frame overran!
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

        mWindow->setActionCallback(mGameInstance->getActionCallback());
        mWindow->setDisplayTex(mGameInstance->getDisplayTex())
    }

    Application::~Application()
    {
        Renderer::Shutdown();

        delete mGameInstance;
        delete mWindow;
    }
}