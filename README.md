
# Hello CLR/CLI

 Simple example/starter project for calling native C/C++ code from managed C# code and vice versa, using a CLR/CLI DLL as an intermediary.

## Getting Started

This is not meant to be a full tutorial, just a sort of template to get you started quickly, so I wont go into too much detail, but I will explain some basics.

Technically C# is also "[CLR][CLR]", and managed C++ is [CLI][CLI], but since visual studio calls them CLR projects, when I say CLR I am specifically referring to managed C++.

If you just need to make a few simple calls to native methods(Especially Win32), you might be better off using the [DllImport attribute][DllImport], or the [PInvoke api][PInvoke]. CLR is definitely better suited for larger more complex projects.

### Project Structure

When you open the solution you will see 3 Solution folders, containing 5 projects.

**Folders**
1. Applications
2. CLI
3. DLLs

The Applications use CLI to call into the DLLs.

**Projects**
1. NativeConsole
2. ManagedConsole
3. ClrDll
4. ManagedDll
5. NativeDll

You can think of them as 2 different solutions, with 3 projects each, because in a normal scenario, you would only be calling from either Managed to Native, or Native to Managed.

- Managed to Native uses *ManagedConsole->ClrDll->NativeDll*.
- Native to Managed uses *NativeConsole->ClrDll->ManagedDll*.

### CLR DLL

The CLR DLL is a DLL which contains both native and managed C++ code.
This is how to interop between managed and native, and vice versa.

Think of 3 friends, and 2 of them are kind of immature and pissed off at each other.
They refuse to speak directly to each other, and instead tell the neutral friend to pass along a message.
CLR/CLI is the neutral friend in the middle.

With CLR you have full access to the C++ language, as well as access to .NET data types.
To access .NET types, you must first add `using namespace System` (or other namespaces) where you would add your includes.

To call native code from C#(or other .NET languages), you make calls to the CLR DLL, which can either call into other native DLLs, or can contain all your native code. And it is the exact opposite for calling from native to managed. You get to decide which source files are managed, and which ones are native.

#### Calling from C#

When calling a CLR DLL from C#, you will be calling functions inside of `ref class`.
For example *ManagedConsole* calls into *ClrDLL* through the *CallNative* class, which looks like this...
``` C++
public ref class CallNative sealed abstract
{
public:
    static String^ GetNativeString();
};
```
The `sealed abstract` just means it is a static class. I decided to use a static class for this situation, but regular class objects that are defined in CLR can be used in C# as well.

To call `GetNativeString()` from C#, is exactly the same as calling any regular static class.
ManagedConsole calls it like `string str = CallNative.GetNativeString();`

You might notice the **^** after String. This is not a C++ *std::String,* it is a managed string, just like in C#.
The **^** operator is just a pointer to managed memory, and all managed variables must be declared with it.
If we look inside *CallNative.cpp* from inside ClrDll, you can see how a C string (const char*) is converted to managed.
``` C++
const char* cstr = GetString();
String^ mstr = gcnew String(cstr);
```
`GetString()` is a regular C++ function, exported like a regular C++ DLL inside of *NativeDll,* and just contains `return "Hello Native!\r\n";`.
`gcnew` may be a new operator to you, it works exactly like `new` in C#, it creates a managed object which is managed by the garbage collector.

### Calling from C++

Calling from native C++ to a CLR DLL is exactly the same as calling another native DLL.
The CLR DLL must export the methods using `__declspec(dllexport)`.
Example, ClrDLL exports 1 function in CallManaged.h as seen below...
``` C++
extern "C" __declspec(dllexport) const char* __stdcall GetManagedString();
```

And then from *NativeConsole,* it just had to `#include "CallManaged.h"`, and then call like normal C++
`const char* core = GetManagedString();`
It probably goes without saying, but before you can include, you will first need to reference the DLL in the project, which is out of the scope of this little tutorial.

Now I'll quickly go over `GetManagedString()` 
``` C++
const char* __stdcall GetManagedString()
{
    String^ str = ManagedDll::ManagedClass::GetString();
    IntPtr ptr = Marshal::StringToHGlobalAnsi(str);
    const char* cstr = (const char*)(void*)ptr;
    return cstr;
}
```

`ManagedDll::ManagedClass::GetString()` is inside a C# DLL (*ManagedClass*), all it does is return a string like so `return "Hello .NET!\r\n";`.

Converting from managed to native is a little bit more work than native to managed(at least for strings), and this is not the only way to do so.
To convert it to a `const char*` we first will allocate some unmanaged memory, just like in regular C#, we can do this with the [Marshal class][Marshal].
Then all we have to do is cast our `IntPtr` to a `const char*`, but we can't directly cast to it, so we first cast to a `void*`.

Once we are done with the string, we must free the memory so we don't get a memory leak.
Because it was allocated with the [Marshal class][Marshal] and not [malloc][Malloc], we can't use [`free()`][Free], and since we are in the pure native DLL we cant use [Marshal.FreeHGlobal(IntPtr)](FreeHG).
So instead we must use [`LocalFree(HLOCAL)`](LocFree), which is actually what [Marshal.FreeHGlobal(IntPtr)](FreeHG) calls.
One thing to note, that if you allocate a BSTR using [Marshal.StringToBSTR(String)][BSTR] or the other BSTR methods, you cannot use LocalFree, and must use [SysFreeString][SysFreeStr], which is what [Marshal.FreeBSTR(IntPtr)][FreeBSTR] uses.

### End

Well those are the basics, and hopefully enough for you to get started. 
I am certainly no CLI master who knows every in and out of the language, so hopefully everything I've said is accurate. 
If you have any questions, feel free to open an issue and I will try to answer as best I can.

Learning CLR at first can be pretty frustrating, and its quirks can be confusing, but once you get used to them it can be a very useful and powerful tool. 





## License
- [MIT](https://github.com/AddioElectronics/Hello-CLR-CLI/LICENSE)
## Author

- Author : Addio

[DllImport]:https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.dllimportattribute?view=net-8.0
[PInvoke]:https://github.com/dotnet/pinvoke
[CLR]:https://en.wikipedia.org/wiki/Common_Language_Runtime
[CLI]:https://en.wikipedia.org/wiki/C%2B%2B/CLI
[Malloc]:https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloc?view=msvc-170
[Free]:https://en.cppreference.com/w/c/memory/free
[FreeHG]:https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.marshal.freehglobal?view=net-8.0
[LocFree]:https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-localfree
[SysFreeStr]:https://learn.microsoft.com/en-us/windows/win32/api/oleauto/nf-oleauto-sysfreestring
[FreeBSTR]:https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.marshal.freebstr?view=net-8.0
[Marshal]:https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.marshal?view=net-8.0
[BSTR]:https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.marshal.stringtobstr?view=net-8.0