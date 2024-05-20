#ifndef UI_H
#define UI_H

#include <array>
#include <cstdint>
#include <string>

#include "../deps/raylib.h"

class Client {
public:
    static void Init();

    static constexpr int ScreenWidth = 64;
    static constexpr int ScreenHeight = 48;
    static constexpr int ScreenArea = ScreenWidth*ScreenHeight;
    static constexpr int PixelsSize = ScreenArea/2;
    static constexpr int PixelScale = 12;
    static constexpr int controlsLength = 8;
    static constexpr raylib::KeyboardKey controls[] = {
        raylib::KEY_W, raylib::KEY_S, raylib::KEY_A, raylib::KEY_D, raylib::KEY_COMMA,
        raylib::KEY_PERIOD, raylib::KEY_SLASH, raylib::KEY_ESCAPE
    };

    static bool ShouldClose;
    static std::string Title;

    static void UpdatePixelBuffer(const std::array<unsigned char, 8192>& recvBuffer);

private:
    static std::array<uint32_t, PixelsSize> PixelBuffer;
    static raylib::Color Palette[16];
    static bool UpdateScreen;

    static bool isLittleEndian();
};



#endif //UI_H
