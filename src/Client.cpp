#include "Client.h"

#include "Networking.h"
#include "PixelDecoderShader.cpp"

#ifdef DrawText // without this raylib::DrawText is read as raylib::DrawTextA, which doesn't exist
#undef DrawText
#endif

raylib::Color Client::Palette[] = {
    raylib::WHITE, raylib::BLACK, raylib::RED, raylib::GREEN, raylib::BLUE, raylib::YELLOW, raylib::ORANGE, raylib::PURPLE,
    raylib::GRAY, raylib::DARKGRAY, raylib::MAROON, raylib::DARKGREEN, raylib::DARKBLUE, raylib::GOLD, raylib::BROWN, raylib::PINK
};

bool Client::ShouldClose = false;
std::string Client::Title = "FPS: ";
std::array<uint32_t, Client::PixelsSize> Client::PixelBuffer{};
bool Client::UpdateScreen = false;

void Client::Init() {
    raylib::InitWindow(ScreenWidth*PixelScale, ScreenHeight*PixelScale + 2*PixelScale, "RetroClient");
    raylib::SetExitKey(raylib::KEY_NULL);
    raylib::BeginDrawing(); raylib::ClearBackground(raylib::Color {12, 12 ,12 ,255}); raylib::EndDrawing();

    const raylib::Shader shader = raylib::LoadShaderFromMemory(nullptr,
        isLittleEndian() ? pixelDecoderShaderLittleEndian : pixelDecoderShaderBigEndian);
    const int pixelDataLoc = raylib::GetShaderLocation(shader, "pixelData");

    while (!raylib::WindowShouldClose() && !ShouldClose) {
        for (uint8_t i = 0; i < controlsLength; ++i) {
            if (raylib::IsKeyPressed(controls[i])) {
                Networking::SendInput(i, true);
            } else if (raylib::IsKeyReleased(controls[i])) {
                Networking::SendInput(i, false);
            }
        }

        raylib::BeginDrawing();
        raylib::ClearBackground(raylib::Color {24, 24, 24, 255});

        // draw FPS and join code
        raylib::DrawText((Title + std::to_string(raylib::GetFPS())).c_str(), 2, 2, 20, raylib::GRAY);

        if (UpdateScreen) { // copy pixelbuffer into vram
            // glsl only supports uints, but raylib doesnt support SHADER_UNIFORM_UINT (why does that not exist?)
            // so we pretend PixelBuffer is a uint array, and it has a size of 384 (same size in bytes)
            raylib::SetShaderValueV(shader, pixelDataLoc, PixelBuffer.begin(), raylib::SHADER_UNIFORM_INT, 384);
        }

        // draw the shader (conversions are done in the shader!)
        raylib::BeginShaderMode(shader);
        raylib::DrawRectangle(0, 24, ScreenWidth*PixelScale, ScreenHeight*PixelScale, raylib::WHITE);
        raylib::EndShaderMode();

        // old draw code, has ~7x worse performance (just drew each pixel with a DrawRectangle)
        // for (int i = 0; i < ScreenArea; ++i) {
        //     const bool even = !(i % 2);
        //
        //     uint8_t pixel = PixelBuffer[i/2];
        //     if (even) {
        //         pixel = (pixel & 0b11110000) >> 4;
        //     } else {
        //         pixel = pixel & 0b00001111;
        //     }
        //
        //     raylib::DrawRectangle((i%ScreenWidth)*PixelScale, (i/ScreenWidth)*PixelScale + 2*PixelScale,
        //         PixelScale, PixelScale, Palette[pixel]);
        // }

        raylib::EndDrawing();
    }

    ShouldClose = true;
    raylib::CloseWindow();
}

void Client::UpdatePixelBuffer(const std::array<unsigned char, 8192>& recvBuffer) {
    std::memcpy(PixelBuffer.begin(), recvBuffer.begin()+1, PixelsSize);
    UpdateScreen = true;
}

bool Client::isLittleEndian() {
    union { // read first byte of a uint and see if its 1 or 0
        uint16_t i;
        uint8_t c[2];
    } constexpr test = {0x0100};

    return test.c[0] == 1;
}
