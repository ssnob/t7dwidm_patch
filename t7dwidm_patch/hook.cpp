#include "hook.h"

Hook Hook::IDMHook;

size_t Hook::GetRestoreValue()
{
	return OriginalValue;
}

void Hook::InstallHook(void* func)
{
    auto kiuserexceptiondispatcher = (uintptr_t)GetProcAddress(GetModuleHandleA("ntdll.dll"), "KiUserExceptionDispatcher");
    auto distance = *(DWORD*)(kiuserexceptiondispatcher + 4); // LdrParentRtlInitializeNtUserPfn
    auto ptr = (kiuserexceptiondispatcher + 8) + distance;

    auto OldProtection = 0ul;
    VirtualProtect(reinterpret_cast<void*>(ptr), 8, PAGE_EXECUTE_READWRITE, &OldProtection);
    *reinterpret_cast<void**>(ptr) = func;
    VirtualProtect(reinterpret_cast<void*>(ptr), 8, OldProtection, &OldProtection);
}

void Hook::nlog(const wchar_t* str, ...)
{
    va_list ap;
    HWND notepad, edit;
    wchar_t buf[256];

    va_start(ap, str);
    vswprintf(buf, str, ap);
    va_end(ap);
    wcscat(buf, L"\r\n");
    notepad = FindWindow(NULL, L"Untitled - Notepad");
    edit = FindWindowEx(notepad, NULL, L"EDIT", NULL);
    SendMessage(edit, EM_REPLACESEL, TRUE, (LPARAM)buf);
}
