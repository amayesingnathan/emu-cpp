#pragma once

#include "KeyCodes.h"

namespace Emu {

    using Action = uint16_t;

    struct KeyMapping
    {
        KeyCode keyAction;
        Action Action;
    };

    using InputMappings = std::vector<KeyMapping>;

    using ActionMap = std::unordered_map<KeyCode, Action>;
    using ActionCallback = std::function<void(Action, bool)>;

#define BIND_ACTION_FUNC(func) std::bind(&func, this, std::placeholders::_1, std::placeholders::_2);

    class Input
    {
    public:
        static void Init(const InputMappings& ioMap);

        static bool IsValid(KeyCode key) { return sActionMap.contains(key); }
        static Action GetEmuButton(KeyCode key);

    private:
        inline static ActionMap sActionMap;
    };
}