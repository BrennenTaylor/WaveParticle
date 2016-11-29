#pragma once

#include <string>

namespace Farlor
{
    std::wstring StringToWideString(const std::string& str);
    std::string WideStringToString(const std::wstring& wstr);
}
