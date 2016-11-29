#pragma once


#include "../Physics/ParticleSystem.h"

#include "Win32Window.h"
#include <windowsx.h>

#include "../Core/Events/EventManager.h"
#include "../Core/Events/KeyboardEvent.h"

#include "../NewRenderer/Camera.h"

namespace Farlor
{
    extern EventManager g_EventManager;
    extern Camera g_MainCamera;
    extern GameWindow m_gameWindow;

    extern ParticleSystem g_WaveParticles;

    int g_PrevMouseX = 0;
    int g_PrevMouseY = 0;
    bool g_LPressed = false;

    Window<PlatformType::Win32>::
    Window()
    {
        m_windowHandle = 0;
        m_fullscreen;
    }

    bool Window<PlatformType::Win32>::
    Initialize(bool fullscreen)
    {
        m_fullscreen = fullscreen;

        // Set the main window
        WNDCLASSEX windowClass = {0};
        windowClass.cbSize = sizeof(WNDCLASSEX);
        windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        windowClass.lpfnWndProc = WndProc;
        windowClass.hInstance = GetModuleHandle(0);
        windowClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        windowClass.hIconSm = windowClass.hIcon;
        windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        windowClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        windowClass.lpszClassName = "WindowClass";

        RegisterClassEx(&windowClass);

        DEVMODE screenSettings = {0};
        int posX, posY;

        m_width = GetSystemMetrics(SM_CXSCREEN);
        m_height = GetSystemMetrics(SM_CYSCREEN);

        if(m_fullscreen)
        {
            screenSettings.dmSize = sizeof(screenSettings);
            screenSettings.dmPelsWidth = (unsigned long)m_width;
            screenSettings.dmPelsHeight = (unsigned long)m_height;
            screenSettings.dmBitsPerPel = 32;
            screenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

            ChangeDisplaySettings(&screenSettings, CDS_FULLSCREEN);

            posX = posY = 0;
        }
        else
        {
            m_width = 800;
            m_height = 600;

            posX = (GetSystemMetrics(SM_CXSCREEN) - m_width)  / 2;
            posY = (GetSystemMetrics(SM_CYSCREEN) - m_height) / 2;
        }

        m_windowHandle = CreateWindowEx(
            WS_EX_APPWINDOW,
            "WindowClass",
            "Window Title",
            WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP | WS_CAPTION,
            posX, posY,
            m_width, m_height,
            0, 0, GetModuleHandle(0), 0);

        m_isRunning = true;
        return true;
    }

    void Window<PlatformType::Win32>::
    Shutdown()
    {

    }

    void Window<PlatformType::Win32>::
    ShowGameWindow()
    {
        ShowWindow(m_windowHandle, SW_SHOW);
        SetForegroundWindow(m_windowHandle);
        SetFocus(m_windowHandle);
    }

    void Window<PlatformType::Win32>::
    ShowGameWindowCursor()
    {
        ShowCursor(false);
    }

    void Window<PlatformType::Win32>::
    SetWindowTitleText(std::string windowText)
    {
        SetWindowText(m_windowHandle, windowText.c_str());
    }

    void Window<PlatformType::Win32>::
    ProcessSystemMessages()
    {
        MSG msg = {0};

        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if(msg.message == WM_QUIT)
        {
            m_isRunning = false;
        }
    }

    LRESULT CALLBACK WndProc(HWND hwnd, UINT message,
        WPARAM wParam, LPARAM lParam)
    {
        static float speed = 1000;
        switch(message)
        {
            // Windows Messages
            case WM_CREATE:
            {
                return 0;
            } break;

            case WM_PAINT:
            {
            } break;

            case WM_CLOSE:
            {
                PostQuitMessage(0);
            } break;

            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;

            case WM_LBUTTONDOWN:
            {
                int newX = GET_X_LPARAM(lParam);
                int newY = GET_Y_LPARAM(lParam);
                g_LPressed = true;
                g_PrevMouseX = newX;
                g_PrevMouseY = newY;
            } break;

            case WM_LBUTTONUP:
            {
                int newX = GET_X_LPARAM(lParam);
                int newY = GET_Y_LPARAM(lParam);
                g_LPressed = false;
                g_PrevMouseX = newX;
                g_PrevMouseY = newY;
            } break;

            // case WM_MOUSEMOVE:
            // {
            //     int newX = GET_X_LPARAM(lParam);
            //     int newY = GET_Y_LPARAM(lParam);
            // } break;

        case WM_MOUSEMOVE:
        {
            int newX = GET_X_LPARAM(lParam);
            int newY = GET_Y_LPARAM(lParam);

            if (g_LPressed)
            {
                // cout << "New Mouse: <" << newX << ", " << newY << ">" << endl;
                g_MainCamera.m_camYaw += (float)(newX - g_PrevMouseX) * 0.01f;
                g_MainCamera.m_camPitch += (float)(newY - g_PrevMouseY) * 0.01f;
            }

            // SetCursorPos(m_gameWindow.m_width / 2, m_gameWindow.m_height / 2);

            g_PrevMouseX = newX;
            g_PrevMouseY = newY;

            // g_PrevMouseX = m_gameWindow.m_width / 2;
            // g_PrevMouseY = m_gameWindow.m_height / 2;


        } break;

            // Keyboard Messages
            case WM_KEYDOWN:
            {
                // cout << "WM_KEYDOWN" << endl;
                switch(wParam)
                {
                    case VK_ESCAPE:
                    {
                        EventDataPtr newEvent(new KeyboardEvent(Keys::Escape, true));
                        g_EventManager.QueueEvent(newEvent);
                    } break;

                    case VK_SPACE:
                    {
                        g_WaveParticles.StartRandomWave();
                    } break;
                    //
                    // case VK_CONTROL:
                    // {
                    // } break;
                    //
                    // case VK_SHIFT:
                    // {
                    // } break;

                    // Movement

                case VK_CONTROL:
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::Left_Ctrl, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                case VK_SHIFT:
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::Left_Shift, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                // Movement
                case 'A':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::A, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'D':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::D, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'P':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::P, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'S':
                {
                    // g_MainCamera.m_moveBackForward -= speed;
                    EventDataPtr newEvent(new KeyboardEvent(Keys::S, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'W':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::W, true));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                default:
                {
                } break;
                }
            } break;

            case WM_KEYUP:
            {
                // cout << "WM_KEYDOWN" << endl;
                switch(wParam)
                {

                    // case VK_SPACE:
                    // {
                    // } break;
                    //
                    // case VK_CONTROL:
                    // {
                    // } break;
                    //
                    // case VK_SHIFT:
                    // {
                    // } break;

                    // Movement

                case VK_CONTROL:
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::Left_Ctrl, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                case VK_SHIFT:
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::Left_Shift, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                // Movement
                case 'A':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::A, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'D':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::D, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'P':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::P, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'S':
                {
                    // g_MainCamera.m_moveBackForward -= speed;
                    EventDataPtr newEvent(new KeyboardEvent(Keys::S, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;
                case 'W':
                {
                    EventDataPtr newEvent(new KeyboardEvent(Keys::W, false));
                    g_EventManager.QueueEvent(newEvent);
                } break;

                default:
                {
                } break;
                }
            } break;

            // Uncomment for Raw Input. Must have raw input toggeled in Engine.cpp
            // case WM_INPUT:
            // {
            //     // cout << "WM_INPUT has been called" << endl;
            //
            //     u32 bufferSize = 40;
            //     u8 *buffer = new u8[40];
            //     ZeroMemory(buffer, bufferSize);
            //     GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
            //         (LPVOID)buffer, &bufferSize, sizeof(RAWINPUTHEADER));
            //
            //     RAWINPUT *raw = (RAWINPUT*)buffer;
            //
            //     if(raw->header.dwType == RIM_TYPEMOUSE)
            //     {
            //     }
            //     else if(raw->header.dwType == RIM_TYPEKEYBOARD)
            //     {
            //         u16 keyCode = raw->data.keyboard.VKey;
            //
            //         // Handle keyboard event.
            //         // In engine, I create a keyboard event, but might not want this in PBR codebase
            //         // EventDataPtr newEvent(new KeyboardEvent(keyCode, true));
            //         // g_EventManager.QueueEvent(newEvent);
            //
            //         // NOTE: Abstract out
            //         switch(keyCode)
            //         {
            //          case VK_ESCAPE:
            //          {
            //              PostQuitMessage(0);
            //          } break;
            //         }
            //     }
            // } break;

            default:
            {
            }
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
}
