#pragma once

namespace Farlor
{
    class SystemCheck
    {
    public:
        bool RunChecks();

        bool CheckRamSize();
        bool CheckVRAMSize();
        bool CheckWindowsVersion();
    };
}
