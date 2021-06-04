// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "hook.h"
#include "IDMFix.h"


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    PVOID handler = NULL;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //handler = AddVectoredExceptionHandler(true, IDMFix::HandleIDMExcept);
        //Hook::nlog(L"Registered VEH %p", handler);
        Hook::nlog(L"Dll by serious. Special thanks to SyGnUs. Additional thanks to Extinct and ItsFebiven for helping test.");
        Hook::InstallHook(IDMFix::HandleIDMExcept2);
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

