﻿#include "Binding/Class/FClassBuilder.h"
#include "Environment/FCSharpEnvironment.h"
#include "Macro/NamespaceMacro.h"
#include "Async/Async.h"

struct FRegisterSoftObjectPtr
{
	static void RegisterImplementation(MonoObject* InMonoObject, const MonoObject* InObject)
	{
		const auto FoundObject = FCSharpEnvironment::GetEnvironment().GetObject(InObject);

		const auto SoftObjectPtr = new TSoftObjectPtr<UObject>(FoundObject);

		FCSharpEnvironment::GetEnvironment().AddMultiReference<TSoftObjectPtr<UObject>>(InMonoObject, SoftObjectPtr);
	}

	static bool IdenticalImplementation(const FGarbageCollectionHandle InA, const FGarbageCollectionHandle InB)
	{
		if (const auto FoundA = FCSharpEnvironment::GetEnvironment().GetMulti<TSoftObjectPtr<UObject>>(InA))
		{
			if (const auto FoundB = FCSharpEnvironment::GetEnvironment().GetMulti<TSoftObjectPtr<UObject>>(InB))
			{
				return *FoundA == *FoundB;
			}
		}

		return false;
	}

	static void UnRegisterImplementation(const FGarbageCollectionHandle InGarbageCollectionHandle)
	{
		AsyncTask(ENamedThreads::GameThread, [InGarbageCollectionHandle]
		{
			(void)FCSharpEnvironment::GetEnvironment().RemoveMultiReference<TSoftObjectPtr<UObject>>(
				InGarbageCollectionHandle);
		});
	}

	static void GetImplementation(const FGarbageCollectionHandle InGarbageCollectionHandle, MonoObject** OutValue)
	{
		const auto Multi = FCSharpEnvironment::GetEnvironment().GetMulti<TSoftObjectPtr<
			UObject>>(InGarbageCollectionHandle);

		*OutValue = FCSharpEnvironment::GetEnvironment().Bind(Multi->Get());
	}

	static void LoadSynchronousImplementation(const FGarbageCollectionHandle InGarbageCollectionHandle,
	                                          MonoObject** OutValue)
	{
		const auto Multi = FCSharpEnvironment::GetEnvironment().GetMulti<TSoftObjectPtr<
			UObject>>(InGarbageCollectionHandle);

		*OutValue = FCSharpEnvironment::GetEnvironment().Bind(Multi->LoadSynchronous());
	}

	FRegisterSoftObjectPtr()
	{
		FClassBuilder(TEXT("SoftObjectPtr"), NAMESPACE_LIBRARY)
			.Function("Register", RegisterImplementation)
			.Function("Identical", IdenticalImplementation)
			.Function("UnRegister", UnRegisterImplementation)
			.Function("Get", GetImplementation)
			.Function("LoadSynchronous", LoadSynchronousImplementation)
			.Register();
	}
};

static FRegisterSoftObjectPtr RegisterSoftObjectPtr;