﻿#include "Reflection/Property/ObjectProperty/FWeakObjectPropertyDescriptor.h"
#include "Environment/FCSharpEnvironment.h"
#include "Bridge/FTypeBridge.h"
#include "UEVersion.h"

void FWeakObjectPropertyDescriptor::Get(void* Src, void** Dest) const
{
	if (WeakObjectProperty != nullptr)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment()->GetMultiObject<TWeakObjectPtr<UObject>>(Src);

		if (SrcMonoObject == nullptr)
		{
			SrcMonoObject = Object_New(Src);
		}

		*Dest = SrcMonoObject;
	}
}

void FWeakObjectPropertyDescriptor::Set(void* Src, void* Dest) const
{
	if (WeakObjectProperty != nullptr)
	{
		const auto SrcMonoObject = static_cast<MonoObject*>(Src);

		const auto SrcMulti = FCSharpEnvironment::GetEnvironment()->GetMulti<TWeakObjectPtr<UObject>>(SrcMonoObject);

		FCSharpEnvironment::GetEnvironment()->RemoveMultiReference<TWeakObjectPtr<UObject>>(Dest);

		WeakObjectProperty->InitializeValue(Dest);

		WeakObjectProperty->SetObjectPropertyValue(Dest, SrcMulti.Get());

		Object_New(Dest);
	}
}

bool FWeakObjectPropertyDescriptor::Identical(const void* A, const void* B, const uint32 PortFlags) const
{
	if (WeakObjectProperty != nullptr)
	{
		const auto ObjectA = WeakObjectProperty->GetObjectPropertyValue(A);

		const auto ObjectB = FCSharpEnvironment::GetEnvironment()->GetMulti<TWeakObjectPtr<UObject>>(
			static_cast<MonoObject*>(const_cast<void*>(B))).Get();

#if UE_OBJECT_PROPERTY_STATIC_IDENTICAL
		return WeakObjectProperty->StaticIdentical(ObjectA, ObjectB, PortFlags);
#else
		return ObjectA == ObjectB;
#endif
	}

	return false;
}

MonoObject* FWeakObjectPropertyDescriptor::Object_New(void* InAddress) const
{
	const auto SrcObject = WeakObjectProperty->GetObjectPropertyValue(InAddress);

	const auto GenericClassMonoClass = FTypeBridge::GetMonoClass(WeakObjectProperty);

	const auto Object = FCSharpEnvironment::GetEnvironment()->GetDomain()->Object_New(GenericClassMonoClass);

	FCSharpEnvironment::GetEnvironment()->AddMultiReference<TWeakObjectPtr<UObject>>(InAddress, Object, SrcObject);

	return Object;
}
