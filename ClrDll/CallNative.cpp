#include "CallNative.h"
#include "nativedll.h"
#include <cstring>

using namespace System::Text;
using namespace System::Runtime::InteropServices;

String^ CallNative::GetNativeString()
{
    const char* cstr = GetString();
    String^ mstr = gcnew String(cstr);
    return mstr;
}
