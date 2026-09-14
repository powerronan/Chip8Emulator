#include "Chip8.h"
#include "Platform.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0]
            << " <Scale> <Delay> <ROM>\n";
        return EXIT_FAILURE;
    }

    constexpr int VIDEO_WIDTH = 64;
    constexpr int VIDEO_HEIGHT = 32;

    int videoScale = std::stoi(argv[1]);
    int cycleDelay = std::stoi(argv[2]);
    char const* romFilename = argv[3];

    Platform platform(
        "CHIP-8 Emulator",
        VIDEO_WIDTH * videoScale,
        VIDEO_HEIGHT * videoScale,
        VIDEO_WIDTH,
        VIDEO_HEIGHT
    );

    Chip8 chip8;
    chip8.openRom(romFilename);

    int videoPitch =
        sizeof(chip8.registers.video[0]) * VIDEO_WIDTH;

    auto lastCycleTime = std::chrono::steady_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.registers.keys);

        if (quit)
            break;

        auto currentTime = std::chrono::steady_clock::now();
        float dt = std::chrono::duration<float, std::milli>(
            currentTime - lastCycleTime
        ).count();

        if (dt >= cycleDelay)
        {
            lastCycleTime = currentTime;

            chip8.Cycle();
            platform.Update(chip8.registers.video, videoPitch);
        }
    }

    return 0;
}