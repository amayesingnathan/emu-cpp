#pragma once

#include "Core/Types.h"

namespace GB {

    struct Pixel
    {
        Byte b, g, r;
    };

    class Screen
    {
    private:
        GB_CONST USize _Width = 160;
        GB_CONST USize _Height = 144;
        GB_CONST USize _BufSize = _Width * _Height * sizeof(Pixel);
        

       GB_CONST USize _Map(USize x, USize y) { return ((y * _Width) + x); }

    public:
        Screen() : mPixels(new Pixel[_Width * _Height]) {}
        ~Screen() { delete[] mPixels; }

    public:
        Pixel& operator()(USize x, USize y) { return mPixels[_Map(x, y)]; }
        const Pixel& operator()(USize x, USize y) const { return mPixels[_Map(x, y)]; }

        const Byte* getBuffer(USize& outSize) const { outSize = _BufSize; return (Byte*)mPixels; }

    private:
        Pixel* mPixels = nullptr;
    };
}