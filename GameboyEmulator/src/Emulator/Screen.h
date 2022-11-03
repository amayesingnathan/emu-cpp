#pragma once

#include "Core/Types.h"

namespace GB {

    struct Pixel
    {
        Byte r;
        Byte g;
        Byte b;
    };

    class Screen
    {
    public:
        enum class Colour
        {
            RED, GREEN, BLUE
        };

        static constexpr USize _Width = 160;
        static constexpr USize _Height = 144;

        static constexpr USize _Map(USize x, USize y) { return ((y * _Width) + x); }

    public:
        Pixel& operator()(USize x, USize y) { return mPixels[_Map(x, y)]; }
        const Pixel& operator()(USize x, USize y) const { return mPixels[_Map(x, y)]; }

        Byte& operator()(USize x, USize y, Colour colour)
        {
            Pixel& pixel = mPixels[_Map(x, y)];

            switch (colour)
            {
            case Colour::RED:   return pixel.r;
            case Colour::GREEN: return pixel.g;
            case Colour::BLUE:  return pixel.b;
            }

            GB_ASSERT(false, "Did not pass a valid colour!");
            return pixel.r;
        }
        const Byte& operator()(USize x, USize y, Colour colour) const
        {
            const Pixel& pixel = mPixels[_Map(x, y)];

            switch (colour)
            {
            case Colour::RED:   return pixel.r;
            case Colour::GREEN: return pixel.g;
            case Colour::BLUE:  return pixel.b;
            }

            GB_ASSERT(false, "Did not pass a valid colour!");
            return pixel.r;
        }

    private:
        Pixel mPixels[_Width * _Height];
    };
}