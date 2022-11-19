#include "glpch.h"
#include "Input.h"

namespace Emu {

    void Input::Init(const InputMappings& ioMap)
    {
        sActionMap.clear();
        for (const auto& [key, emuButton] : ioMap)
            sActionMap[key] = emuButton;
    }

    Action Input::GetEmuButton(KeyCode key)
    {
        GL_ASSERT(IsValid(key));
        return sActionMap[key];
    }
}