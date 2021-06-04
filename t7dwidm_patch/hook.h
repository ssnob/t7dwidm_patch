#pragma once
#include "framework.h"

class Hook
{
public:
	size_t GetRestoreValue();
	static void nlog(const wchar_t* str, ...);
	static void InstallHook(void* func);
	static Hook IDMHook;

private:
	void* ExceptHandler = NULL;
	void* OriginalPtr = NULL;
	size_t OriginalValue = NULL;
};