#pragma once

#include "..\Platform\Platform.h"

// #ifdef WIN32
// #include <windows.h>
// typedef HWND WindowHandle;
// #else
// typedef void* WindowHandle;
// #endif

namespace Farlor
{
    template <typename Window>
    class WindowBase
    {
    public:
        unsigned int GetHeight()
        {
            return static_cast<Window*>(this)->m_height;
        }
        unsigned int GetWidth()
        {
            return static_cast<Window*>(this)->m_width;
        }

        bool IsRunning()
        {
            return static_cast<Window*>(this)->m_isRunning;
        }

        bool IsFullscreen()
        {
            return static_cast<Window*>(this)->m_fullscreen;
        }

    };

    template<PlatformType p>
    class Window
    {
    };
}

#ifdef WIN32

#include "Win32Window.h"
typedef Farlor::Window<Farlor::PlatformType::Win32> GameWindow;

#endif