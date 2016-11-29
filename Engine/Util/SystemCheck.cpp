#include "SystemCheck.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <d3d11.h>
#include <atlbase.h>

using namespace std;

namespace Farlor
{
    bool SystemCheck::RunChecks()
    {
        cout << "Run CheckRamSize" << endl;

        if(!CheckRamSize())
        {
            cout << "CheckRAMSize Failed" << endl;
            return false;
        }

        cout << "CheckRamSize succeeded" << endl;

        cout << "Run CheckVRamSize" << endl;

        if(!CheckVRAMSize())
        {
            cout << "CheckVRAMSize Failed" << endl;
            return false;
        }

        cout << "CheckVRamSize succeeded" << endl;

        return true;
    }

    bool SystemCheck::CheckRamSize()
    {
        const long ONE_GIG = 1073741824;
        MEMORYSTATUSEX status;
        ZeroMemory(&status, sizeof(MEMORYSTATUSEX));
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        if(status.ullTotalPhys < ONE_GIG)
        {
            MessageBox(0, "You dont have enough ram, 1GB is needed", "Fail", MB_OK);
        }

        cout << "You have enough RAM" << endl;

        return true;
    }

    bool SystemCheck::CheckVRAMSize()
    {
        HRESULT hr;
        D3D_FEATURE_LEVEL FeatureLevel;

        CComPtr<IDXGIFactory1> DXGIFactory;

        hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&DXGIFactory);
        if(SUCCEEDED(hr))
        {
            CComPtr<IDXGIAdapter1> Adapter;

            hr = DXGIFactory->EnumAdapters1(0, &Adapter);
            if(SUCCEEDED(hr))
            {
                CComPtr<ID3D11Device> Device;
                CComPtr<ID3D11DeviceContext> DeviceContext;

                hr = D3D11CreateDevice(Adapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr,
                    D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, D3D11_SDK_VERSION,
                    &Device, &FeatureLevel, &DeviceContext);

                if(SUCCEEDED(hr))
                {
                    DXGI_ADAPTER_DESC adapterDesc;
                    Adapter->GetDesc(&adapterDesc);

                    std::wstring Description = adapterDesc.Description;
                    INT64 VideoRam = adapterDesc.DedicatedVideoMemory;
                    INT64 SystemRam = adapterDesc.DedicatedSystemMemory;
                    INT64 SharedRam = adapterDesc.SharedSystemMemory;

                    std::wcout << "***************** GRAPHICS ADAPTER DETAILS ***********************" << endl;;
                    std::wcout << "Adapter Description: " << Description << endl;
                    std::wcout << "Dedicated Video RAM: " << VideoRam << endl;
                    std::wcout << "Dedicated System RAM: " << SystemRam << endl;
                    std::wcout << "Shared System RAM: " << SharedRam << endl;
                    std::wcout << "PCI ID: " << Description << endl;
                    std::wcout << "Feature Level: " << FeatureLevel << endl;
                }
            }
        }    

        return true;
    }
}