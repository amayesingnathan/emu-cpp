module EmuBase.IO.Input;

import EmuBase.Core.Common;

namespace Emu {

    void Input::Init(const InputMappings& ioMap)
    {
        sActionMap.clear();
        for (const auto& [key, emuButton] : ioMap)
            sActionMap[key] = emuButton;
    }

    Action Input::GetEmuButton(KeyCode key)
    {
        Assert(IsValid(key));
        return sActionMap[key];
    }
}