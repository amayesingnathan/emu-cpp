#pragma once

#include "Core/Types.h"

namespace GB {

    class Screen
    {
    public:
        GB_CONST USize _Width = 160;
        GB_CONST USize _Height = 144;
        
    private:
       GB_CONST USize _Map(USize x, USize y) { return ((y * _Width) + x); }

    public:
        Screen() = default;
        Screen(Byte* buffer)
            : mPixels((Emu::Pixel*)buffer)
        {}

    public:
        Emu::Pixel& operator()(USize x, USize y) { return mPixels[_Map(x, y)]; }
        const Emu::Pixel& operator()(USize x, USize y) const { return mPixels[_Map(x, y)]; }

    private:
        Emu::Pixel* mPixels = nullptr;
    };
}