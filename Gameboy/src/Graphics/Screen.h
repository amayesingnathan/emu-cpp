#pragma once

#include "Graphics/PixelBuffer.h"

#include "Core/Types.h"

namespace GB {

    struct Sprite
    {
        Byte xPos;
        Byte yPos;
        Byte tileLoc;
        BitField flags;
    };

    class Screen
    {
    public:
        GB_CONST USize _Width = 160;
        GB_CONST USize _Height = 144;
        
    private:
       GB_CONST USize _Map(USize x, USize y) { return ((y * _Width) + x); }

    public:
        Screen()
            : mPixels((Emu::Pixel*)::operator new(Screen::_Width* Screen::_Height * sizeof(Emu::Pixel)))
        {}
        Screen(Byte* buffer)
            : mPixels((Emu::Pixel*)buffer)
        {}
        ~Screen() { ::operator delete(mPixels); }

        operator Emu::Pixel* () { return mPixels; }

    public:
        Emu::Pixel& operator()(USize x, USize y) { return mPixels[_Map(x, y)]; }
        const Emu::Pixel& operator()(USize x, USize y) const { return mPixels[_Map(x, y)]; }

    private:
        Emu::Pixel* mPixels = nullptr;
    };
}