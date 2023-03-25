export module EmuBase.IO.Input;

import EmuBase.IO.KeyCodes;
export import <vector>;
export import <unordered_map>;

export namespace Emu {

    using Action = uint16_t;

    struct KeyMapping
    {
        KeyCode keyAction;
        Action Action;
    };

    using InputMappings = std::vector<KeyMapping>;
    using ActionMap = std::unordered_map<KeyCode, Action>;

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