#include "CallManaged.h"

using namespace System;
using namespace System::Text;
using namespace System::Runtime::InteropServices;

const char* __stdcall GetManagedString()
{
    String^ str = ManagedDll::ManagedClass::GetString();
    IntPtr ptr = Marshal::StringToHGlobalAnsi(str);
    const char* cstr = (const char*)(void*)ptr;
    return cstr;
}