﻿#include "Reflection/Property/ContainerProperty/FArrayPropertyDescriptor.h"
#include "Bridge/FTypeBridge.h"
#include "Environment/FCSharpEnvironment.h"
#include "Macro/ClassMacro.h"
#include "Macro/FunctionMacro.h"
#include "Macro/MonoMacro.h"
#include "Macro/NamespaceMacro.h"
#include "Reflection/Container/FArrayHelper.h"

void FArrayPropertyDescriptor::Get(void* Src, void** Dest) const
{
	if (ArrayProperty != nullptr)
	{
		auto SrcMonoObject = FCSharpEnvironment::GetEnvironment()->GetContainerObject(Src);

		if (SrcMonoObject == nullptr)
		{
			SrcMonoObject = Object_New(Src);
		}

		*Dest = SrcMonoObject;
	}
}

void FArrayPropertyDescriptor::Set(void* Src, void* Dest) const
{
	if (ArrayProperty != nullptr)
	{
		const auto SrcMonoObject = static_cast<MonoObject*>(Src);

		const auto SrcContainer = FCSharpEnvironment::GetEnvironment()->GetContainer<FArrayHelper>(SrcMonoObject);

		FCSharpEnvironment::GetEnvironment()->RemoveContainerReference(Dest);

		ArrayProperty->CopyCompleteValue(Dest, SrcContainer->GetScriptArray());

		Object_New(Dest);
	}
}

MonoObject* FArrayPropertyDescriptor::Object_New(void* InAddress) const
{
	const auto FoundArrayMonoClass = FCSharpEnvironment::GetEnvironment()->GetDomain()->Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_COMMON), CLASS_T_ARRAY);

	const auto FoundArrayMonoType = FCSharpEnvironment::GetEnvironment()->GetDomain()->Class_Get_Type(
		FoundArrayMonoClass);

	const auto FoundArrayReflectionType = FCSharpEnvironment::GetEnvironment()->GetDomain()->Type_Get_Object(
		FoundArrayMonoType);

	const auto FoundGenericMonoClass = FTypeBridge::GetMonoClass(ArrayProperty->Inner);

	const auto FoundGenericMonoType = FCSharpEnvironment::GetEnvironment()->GetDomain()->Class_Get_Type(
		FoundGenericMonoClass);

	const auto FoundGenericReflectionType = FCSharpEnvironment::GetEnvironment()->GetDomain()->Type_Get_Object(
		FoundGenericMonoType);

	void* InParams[3];

	InParams[0] = FoundArrayReflectionType;

	const auto GenericReflectionTypeMonoArray = FCSharpEnvironment::GetEnvironment()->GetDomain()->Array_New(
		FCSharpEnvironment::GetEnvironment()->GetDomain()->Get_Object_Class(), 1);

	ARRAY_SET(GenericReflectionTypeMonoArray, MonoReflectionType*, 0, FoundGenericReflectionType);

	InParams[1] = GenericReflectionTypeMonoArray;

	InParams[2] = GenericReflectionTypeMonoArray;

	const auto UtilsMonoClass = FCSharpEnvironment::GetEnvironment()->GetDomain()->Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_COMMON), CLASS_UTILS);

	const auto CreateGenericTypeMethod = FCSharpEnvironment::GetEnvironment()->GetDomain()->Class_Get_Method_From_Name(
		UtilsMonoClass,
		FUNCTION_UTILS_MAKE_GENERIC_TYPE_INSTANCE,
		3);

	const auto GenericClassMonoObject = FCSharpEnvironment::GetEnvironment()->GetDomain()->Runtime_Invoke(
		CreateGenericTypeMethod, nullptr, InParams, nullptr);

	const auto GenericClassMonoClass = FCSharpEnvironment::GetEnvironment()->GetDomain()->Object_Get_Class(
		GenericClassMonoObject);

	auto FoundArrayReflectionTypeParam = static_cast<void*>(FoundArrayReflectionType);

	const auto Object = FCSharpEnvironment::GetEnvironment()->GetDomain()->Object_New(
		GenericClassMonoClass, 1, &FoundArrayReflectionTypeParam);

	const auto ArrayHelper = new FArrayHelper(ArrayProperty->Inner, InAddress);

	const auto OwnerObject = reinterpret_cast<UObject*>(static_cast<uint8*>(InAddress) - ArrayProperty->
		GetOffset_ForInternal());

	const auto OwnerGarbageCollectionHandle = FCSharpEnvironment::GetEnvironment()->
		GetGarbageCollectionHandle(OwnerObject);

	FCSharpEnvironment::GetEnvironment()->AddContainerReference(OwnerGarbageCollectionHandle, InAddress, ArrayHelper,
	                                                            Object);

	return Object;
}
