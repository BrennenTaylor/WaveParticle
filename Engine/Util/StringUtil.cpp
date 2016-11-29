#include "StringUtil.h"

#include <locale>
#include <codecvt>

using namespace std;

namespace Farlor
{
    wstring StringToWideString(const string& str)
    {
        using convertType = codecvt_utf8<wchar_t>;
        wstring_convert<convertType, wchar_t> converter;

        return converter.from_bytes(str);
    }

    string WideStringToString(const wstring& wstr)
    {
        using convertType = codecvt_utf8<wchar_t>;
        wstring_convert<convertType, wchar_t> converter;

        return converter.to_bytes(wstr);
    }
}
