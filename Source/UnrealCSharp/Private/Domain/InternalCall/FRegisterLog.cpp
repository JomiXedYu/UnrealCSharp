﻿#include "Binding/Class/FClassBuilder.h"
#include "Log/UnrealCSharpLog.h"
#include "CoreMacro/NamespaceMacro.h"

struct FRegisterLog
{
	/**
	* https://github.com/mono-ue/UnrealEngine/blob/monoue/Engine/Plugins/MonoUE/Source/MonoRuntime/Private/MonoLogTextWriter.cpp#L14
	*/
	static void LogImplementation(const UTF16CHAR* InBuffer, const unsigned int InReadOffset)
	{
#if !NO_LOGGING
		if (UE_LOG_ACTIVE(LogUnrealCSharp, Log))
		{
			GLog->Serialize(StringCast<TCHAR>(InBuffer + 2 * sizeof(void*)).Get() + InReadOffset, ELogVerbosity::Log,
			                LogUnrealCSharp.GetCategoryName());
		}
#endif
	}

	FRegisterLog()
	{
		FClassBuilder(TEXT("Log"), NAMESPACE_LIBRARY)
			.Function("Log", LogImplementation)
			.Register();
	}
};

static FRegisterLog RegisterLog;
