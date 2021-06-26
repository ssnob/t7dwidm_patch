#pragma once
#include "framework.h"

typedef bool(__fastcall* tLiveFriends_IsFriendByXUID)(int controllerIndex, int64_t XUID);

class IDMFix
{
public:
	static LONG WINAPI HandleIDMExcept(EXCEPTION_POINTERS* pExceptionInfo);
	static void HandleIDMExcept2(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT ContextRecord);
	static tLiveFriends_IsFriendByXUID LiveFriends_IsFriendByXUID;
};

#define PTR_LiveFriends_IsFriendByXUID ((size_t)GetModuleHandle(NULL) + 0x1DF9430)
#define DEBUG_ADDRESS ((size_t)GetModuleHandle(NULL) + 0x143A606)
#define DEBUG_EXIT ((size_t)GetModuleHandle(NULL) + 0x143A69A)

#define CRASH_SPOT ((size_t)GetModuleHandle(NULL) + 0x1E7E981)
#define CRASH_MIN ((size_t)GetModuleHandle(NULL) + 0x1E7E900)
#define CRASH_MAX ((size_t)GetModuleHandle(NULL) + 0x1E7EB9F)
#define SKIPTO ((size_t)GetModuleHandle(NULL) + 0x1E7EB86)

#define PTR_xuid ((size_t)GetModuleHandle(NULL) + 0x14FB7B98)

#define PTR_CL_HandleVoiceTypePacket ((size_t)GetModuleHandle(NULL) + 0x1359310)
#define PTR_CL_HandleVoiceTypePacket_Ret ((size_t)GetModuleHandle(NULL) + 0x1359379)