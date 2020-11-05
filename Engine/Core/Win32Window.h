#pragma once

#include "Window.h"

#include "Engine.h"

#include "..\Platform\Platform.h"

#define WINDOWS_LEAN_AND_MEAN
#include <Windows.h>
#include <string>

namespace Farlor
{
    template<>
    class Window<PlatformType::Win32> : public WindowBase<Window<PlatformType::Win32>>
    {
    public:
        friend class WindowBase<Window<PlatformType::Win32>>;

        Window();
        bool Initialize(bool fullscreen);
        void ProcessSystemMessages();
        void Shutdown();

        void ShowGameWindow();
        void ShowGameWindowCursor();

        void SetWindowTitleText(std::string windowText);

        HWND GetWindowHandle()
        {
            return m_windowHandle;
        }

    public:
        unsigned int m_width;
        unsigned int m_height;

    private:
        Engine* m_engine;

    protected:
        bool m_isRunning;
        HWND m_windowHandle;
        bool m_fullscreen;
    };

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam);
}
