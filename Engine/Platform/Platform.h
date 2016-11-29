#pragma once

// #ifdef WIN32
// #define FARLOR_WIN32
// #define WIN32_LEAN_AND_MEAN
// #include <windows.h>
// #include <Core\Win32Window.h>
// #endif

// #ifdef __LINUX__
// #define FARLOR_LINUX
// #endif

// #ifdef __APPLE__
// #define FALOR_APPLE
// #endif

namespace Farlor
{
    enum class PlatformType
    {
        Win32,
        Linux,
        MacOS,
        NUM_PLATFORMS
    };
}