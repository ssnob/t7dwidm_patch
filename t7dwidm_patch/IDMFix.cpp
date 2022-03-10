#include "IDMFix.h"
#include "hook.h"

tLiveFriends_IsFriendByXUID IDMFix::LiveFriends_IsFriendByXUID = NULL;

// handle the exception when it occurs
LONG __stdcall IDMFix::HandleIDMExcept(EXCEPTION_POINTERS* pExceptionInfo)
{
    return EXCEPTION_CONTINUE_SEARCH;
}

typedef unsigned long long(__fastcall* ZwContinue_t)(PCONTEXT ThreadContext, BOOLEAN RaiseAlert);
ZwContinue_t ZwContinue = reinterpret_cast<ZwContinue_t>(GetProcAddress(GetModuleHandleA("ntdll.dll"), "ZwContinue"));

#define qnoff(x) ((int64_t)(x) - (int64_t)GetModuleHandle(NULL))

#define PTR_CL_MigrationPacket ((size_t)GetModuleHandleA(NULL) + (size_t)0x1361ede)
#define PTR_MigrationVTable ((size_t)GetModuleHandleA(NULL) + (size_t)0x2fbaeb0)
void IDMFix::HandleIDMExcept2(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ContextRecord)
{    
    ContextRecord->Dr0 = DEBUG_ADDRESS;
    ContextRecord->Dr1 = PTR_CL_HandleVoiceTypePacket;
    ContextRecord->Dr2 = PTR_CL_MigrationPacket;
    ContextRecord->Dr7 = 1 | 4;
    ContextRecord->Dr7 |= (1 << 4);

    if ((INT64)ExceptionRecord->ExceptionAddress == PTR_CL_MigrationPacket)
    {
        char* message = (char*)ContextRecord->Rdx;
        if (!strcmp(message, "mstart") || !strcmp(message, "mdata") || !strcmp(message, "mhead") || !strcmp(message, "mstate") || !strcmp(message, "connectResponseMigration") || !strcmp(message, "rcon") || !strcmp(message, "requeststats"))
        {
            ContextRecord->Rip = ((size_t)GetModuleHandleA(NULL) + (size_t)0x1361f6a);
            ZwContinue(ContextRecord, false);
        }

        ContextRecord->Rip += 7;
        ContextRecord->Rax = PTR_MigrationVTable;
        ZwContinue(ContextRecord, false);
        return;
    }

    if ((ExceptionRecord->ExceptionAddress >= (PVOID)CRASH_MIN) && (ExceptionRecord->ExceptionAddress <= (PVOID)CRASH_MAX))
    {
        ContextRecord->Rip = SKIPTO; // just skip all this bs
        ZwContinue(ContextRecord, false);
        return;
    }

    // handler for filtering of the xuids who can send you an IM
    if (ExceptionRecord->ExceptionAddress == (PVOID)DEBUG_ADDRESS)
    {
        bool is_valid_user = false;

        // define call address
        if (!LiveFriends_IsFriendByXUID)
        {
            LiveFriends_IsFriendByXUID = (tLiveFriends_IsFriendByXUID)PTR_LiveFriends_IsFriendByXUID;
        }

        int64_t senderXuid = ContextRecord->Rcx;
        int64_t localXuid = *(int64_t*)PTR_xuid;

        //// user is valid if they are a friend or its a loopback dispatch
        //is_valid_user = !senderXuid || // loopback
        //    LiveFriends_IsFriendByXUID(0, senderXuid) || // is a friend
        //    (localXuid == senderXuid); // is ourself

        if (true || is_valid_user)
        {
            // Hook::nlog(L"XUID GOOD: %p", senderXuid);
            ContextRecord->Rip += 4;
            ContextRecord->Rsp -= 0x60;
            ZwContinue(ContextRecord, false);
            return;
        }

        // Hook::nlog(L"XUID BAD: %p", senderXuid);
        // redirect to a nearby return because we are not dispatching this message
        ContextRecord->Rip = DEBUG_EXIT;
        ZwContinue(ContextRecord, false);
        return;
    }

    if (ExceptionRecord->ExceptionAddress == (PVOID)PTR_CL_HandleVoiceTypePacket)
    {
        ContextRecord->Rip = PTR_CL_HandleVoiceTypePacket_Ret;
        ContextRecord->Rax = 0;
        ZwContinue(ContextRecord, false);
        return;
    }

    if (ExceptionRecord->ExceptionCode == 0xC0000005)
    {
        Hook::nlog(L"[%p]Exception at (%p)", GetModuleHandle(NULL), qnoff(ExceptionRecord->ExceptionAddress));
        Hook::nlog(L"[%p]Rcx: (%p) Rdx: (%p) R8: (%p) R9: (%p)", qnoff(ExceptionRecord->ExceptionAddress), qnoff(ContextRecord->Rcx), qnoff(ContextRecord->Rdx), qnoff(ContextRecord->R8), qnoff(ContextRecord->R9));
        Hook::nlog(L"[%p]Rax: (%p) Rbx: (%p) Rsi: (%p) Rdi: (%p)", qnoff(ExceptionRecord->ExceptionAddress), qnoff(ContextRecord->Rax), qnoff(ContextRecord->Rbx), qnoff(ContextRecord->Rsi), qnoff(ContextRecord->Rdi));
        for (int i = 0; i < 0x1000; i += 0x10)
        {
            Hook::nlog(L"[%p] %p %p", ContextRecord->Rsp + i, *(int64_t*)(ContextRecord->Rsp + i), *(int64_t*)(ContextRecord->Rsp + i + 8));
        }
    }
}