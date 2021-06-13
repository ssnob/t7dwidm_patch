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
void IDMFix::HandleIDMExcept2(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ContextRecord)
{    
    ContextRecord->Dr0 = DEBUG_ADDRESS;
    ContextRecord->Dr1 = PTR_CL_HandleVoiceTypePacket;
    ContextRecord->Dr7 = 1 | 2;

    if (ExceptionRecord->ExceptionAddress == (PVOID)CRASH_SPOT)
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

        // user is valid if they are a friend or its a loopback dispatch
        is_valid_user = !senderXuid || // loopback
            LiveFriends_IsFriendByXUID(0, senderXuid) || // is a friend
            (localXuid == senderXuid); // is ourself

        if (is_valid_user)
        {
            Hook::nlog(L"XUID GOOD: %p", senderXuid);
            ContextRecord->Rip += 4;
            ContextRecord->Rsp -= 0x60;
            ZwContinue(ContextRecord, false);
            return;
        }

        Hook::nlog(L"XUID BAD: %p", senderXuid);
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
}