#include <iostream>
#include <Windows.h>
#include "CallManaged.h"

int main()
{
    const char* core = GetManagedString();
    std::cout << core;
    LocalFree((HLOCAL)core);
}
