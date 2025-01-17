#include "Dynamic/FDynamicEnumGenerator.h"
#include "CoreMacro/ClassMacro.h"
#include "CoreMacro/FunctionMacro.h"
#include "CoreMacro/MonoMacro.h"
#include "CoreMacro/NamespaceMacro.h"
#include "Domain/FMonoDomain.h"
#include "Template/TGetArrayLength.inl"
#include "Common/FUnrealCSharpFunctionLibrary.h"
#include "Dynamic/FDynamicClassGenerator.h"
#include "Dynamic/FDynamicGeneratorCore.h"
#if WITH_EDITOR
#include "BlueprintActionDatabase.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#endif
#include "UEVersion.h"

TMap<FString, UEnum*> FDynamicEnumGenerator::DynamicEnumMap;

TSet<UEnum*> FDynamicEnumGenerator::DynamicEnumSet;

void FDynamicEnumGenerator::Generator()
{
	const auto AttributeMonoClass = FMonoDomain::Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_DYNAMIC), CLASS_U_ENUM_ATTRIBUTE);

	const auto AttributeMonoType = FMonoDomain::Class_Get_Type(AttributeMonoClass);

	const auto AttributeMonoReflectionType = FMonoDomain::Type_Get_Object(AttributeMonoType);

	const auto UtilsMonoClass = FMonoDomain::Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_CORE_UOBJECT), CLASS_UTILS);

	void* InParams[2] = {
		AttributeMonoReflectionType,
		FMonoDomain::GCHandle_Get_Target_V2(FMonoDomain::AssemblyGCHandles[1])
	};

	const auto GetTypesWithAttributeMethod = FMonoDomain::Class_Get_Method_From_Name(
		UtilsMonoClass, FUNCTION_UTILS_GET_TYPES_WITH_ATTRIBUTE, TGetArrayLength(InParams));

	const auto Types = reinterpret_cast<MonoArray*>(FMonoDomain::Runtime_Invoke(
		GetTypesWithAttributeMethod, nullptr, InParams));

	const auto Length = FMonoDomain::Array_Length(Types);

	for (auto Index = 0; Index < Length; ++Index)
	{
		const auto ReflectionType = ARRAY_GET(Types, MonoReflectionType*, Index);

		const auto Type = FMonoDomain::Reflection_Type_Get_Type(ReflectionType);

		const auto Class = FMonoDomain::Type_Get_Class(Type);

		Generator(Class);
	}
}

#if WITH_EDITOR
void FDynamicEnumGenerator::CodeAnalysisGenerator()
{
	static FString CSharpEnum = TEXT("CSharpEnum");

	auto EnumNames = FDynamicGeneratorCore::GetDynamic(
		FString::Printf(TEXT(
			"%s/%s.json"),
		                *FUnrealCSharpFunctionLibrary::GetCodeAnalysisPath(),
		                *CSharpEnum),
		CSharpEnum
	);

	for (const auto& EnumName : EnumNames)
	{
		if (!DynamicEnumMap.Contains(EnumName))
		{
			GeneratorCSharpEnum(FDynamicGeneratorCore::GetOuter(), EnumName);
		}
	}
}
#endif

void FDynamicEnumGenerator::Generator(MonoClass* InMonoClass)
{
	if (InMonoClass == nullptr)
	{
		return;
	}

	const auto ClassName = FString(FMonoDomain::Class_Get_Name(InMonoClass));

	const auto Outer = FDynamicGeneratorCore::GetOuter();

#if WITH_EDITOR
	const UEnum* OldEnum{};
#endif

	UEnum* Enum{};

	if (DynamicEnumMap.Contains(ClassName))
	{
		Enum = DynamicEnumMap[ClassName];

		GeneratorEnum(ClassName, Enum, [InMonoClass](UEnum* InEnum)
		{
			ProcessGenerator(InMonoClass, InEnum);
		});

#if WITH_EDITOR
		OldEnum = Enum;
#endif
	}
	else
	{
		Enum = GeneratorCSharpEnum(Outer, ClassName,
		                           [InMonoClass](UEnum* InEnum)
		                           {
			                           ProcessGenerator(InMonoClass, InEnum);
		                           });
	}

#if WITH_EDITOR
	if (OldEnum != nullptr)
	{
		ReInstance(Enum);
	}
#endif
}

bool FDynamicEnumGenerator::IsDynamicEnum(MonoClass* InMonoClass)
{
	const auto AttributeMonoClass = FMonoDomain::Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_DYNAMIC), CLASS_U_ENUM_ATTRIBUTE);

	const auto Attrs = FMonoDomain::Custom_Attrs_From_Class(InMonoClass);

	return !!FMonoDomain::Custom_Attrs_Has_Attr(Attrs, AttributeMonoClass);
}

bool FDynamicEnumGenerator::IsDynamicEnum(const UEnum* InEnum)
{
	return DynamicEnumSet.Contains(InEnum);
}

void FDynamicEnumGenerator::BeginGenerator(const UEnum* InEnum)
{
	InEnum->SetInternalFlags(EInternalObjectFlags::Native);
}

void FDynamicEnumGenerator::ProcessGenerator(MonoClass* InMonoClass, UEnum* InEnum)
{
#if WITH_EDITOR
	GeneratorMetaData(InMonoClass, InEnum);
#endif

	GeneratorEnumerator(InMonoClass, InEnum);
}

void FDynamicEnumGenerator::EndGenerator(UEnum* InEnum)
{
#if WITH_EDITOR
	if (GEditor)
	{
		FBlueprintActionDatabase& ActionDatabase = FBlueprintActionDatabase::Get();

		ActionDatabase.ClearAssetActions(InEnum);

		ActionDatabase.RefreshAssetActions(InEnum);
	}
#endif

#if UE_NOTIFY_REGISTRATION_EVENT
#if !WITH_EDITOR
	NotifyRegistrationEvent(*InEnum->GetPackage()->GetName(),
	                        *InEnum->GetName(),
	                        ENotifyRegistrationType::NRT_Enum,
	                        ENotifyRegistrationPhase::NRP_Finished,
	                        nullptr,
	                        false,
	                        InEnum);
#endif
#endif
}

void FDynamicEnumGenerator::GeneratorEnum(const FString& InName, UEnum* InEnum,
                                          const TFunction<void(UEnum*)>& InProcessGenerator)
{
	DynamicEnumMap.Add(InName, InEnum);

	DynamicEnumSet.Add(InEnum);

	BeginGenerator(InEnum);

	InProcessGenerator(InEnum);

	EndGenerator(InEnum);
}

UEnum* FDynamicEnumGenerator::GeneratorCSharpEnum(UPackage* InOuter, const FString& InName)
{
	return GeneratorCSharpEnum(InOuter, InName,
	                           [](UEnum*)
	                           {
	                           });
}

UEnum* FDynamicEnumGenerator::GeneratorCSharpEnum(UPackage* InOuter, const FString& InName,
                                                  const TFunction<void(UEnum*)>& InProcessGenerator)
{
	const auto Enum = NewObject<UEnum>(InOuter, *InName, RF_Public);

	Enum->AddToRoot();

	GeneratorEnum(InName, Enum, InProcessGenerator);

	return Enum;
}

#if WITH_EDITOR
void FDynamicEnumGenerator::ReInstance(UEnum* InEnum)
{
	TArray<UBlueprintGeneratedClass*> BlueprintGeneratedClasses;

	FDynamicGeneratorCore::IteratorObject<UBlueprintGeneratedClass>(
		[InEnum](const TObjectIterator<UBlueprintGeneratedClass>& InBlueprintGeneratedClass)
		{
			if (FDynamicClassGenerator::IsDynamicBlueprintGeneratedClass(*InBlueprintGeneratedClass))
			{
				return false;
			}

			for (TFieldIterator<FProperty> It(*InBlueprintGeneratedClass, EFieldIteratorFlags::ExcludeSuper,
			                                  EFieldIteratorFlags::ExcludeDeprecated); It; ++It)
			{
				if (const auto EnumProperty = CastField<FEnumProperty>(*It))
				{
					if (EnumProperty->GetEnum() == InEnum)
					{
						return true;
					}
				}
				else if (const auto ByteProperty = CastField<FByteProperty>(*It))
				{
					if (ByteProperty->Enum == InEnum)
					{
						return true;
					}
				}
			}

			return false;
		},
		[&BlueprintGeneratedClasses](const TObjectIterator<UBlueprintGeneratedClass>& InBlueprintGeneratedClass)
		{
			if (!FUnrealCSharpFunctionLibrary::IsSpecialClass(*InBlueprintGeneratedClass))
			{
				BlueprintGeneratedClasses.AddUnique(*InBlueprintGeneratedClass);
			}
		});


	for (const auto BlueprintGeneratedClass : BlueprintGeneratedClasses)
	{
		const auto Blueprint = Cast<UBlueprint>(BlueprintGeneratedClass->ClassGeneratedBy);

		Blueprint->Modify();

		FBlueprintEditorUtils::RefreshAllNodes(Blueprint);

		FBlueprintEditorUtils::MarkBlueprintAsModified(Blueprint);

		constexpr auto BlueprintCompileOptions = EBlueprintCompileOptions::SkipGarbageCollection |
			EBlueprintCompileOptions::SkipSave;

		FKismetEditorUtilities::CompileBlueprint(Blueprint, BlueprintCompileOptions);
	}
}

void FDynamicEnumGenerator::GeneratorMetaData(MonoClass* InMonoClass, UEnum* InEnum)
{
	if (InMonoClass == nullptr || InEnum == nullptr)
	{
		return;
	}

	const auto AttributeMonoClass = FMonoDomain::Class_From_Name(
		COMBINE_NAMESPACE(NAMESPACE_ROOT, NAMESPACE_DYNAMIC), CLASS_U_ENUM_ATTRIBUTE);

	if (const auto Attrs = FMonoDomain::Custom_Attrs_From_Class(InMonoClass))
	{
		if (!!FMonoDomain::Custom_Attrs_Has_Attr(Attrs, AttributeMonoClass))
		{
			FDynamicGeneratorCore::SetMetaData(InEnum, Attrs);
		}
	}
}
#endif

void FDynamicEnumGenerator::GeneratorEnumerator(MonoClass* InMonoClass, UEnum* InEnum)
{
	if (InMonoClass == nullptr || InEnum == nullptr)
	{
		return;
	}

	TArray<TPair<FName, int64>> InNames;

	void* Iterator = nullptr;

	while (const auto Field = FMonoDomain::Class_Get_Fields(InMonoClass, &Iterator))
	{
		static auto value__ = FName(TEXT("value__"));

		const auto FieldName = FMonoDomain::Field_Get_Name(Field);

		if (FieldName != value__)
		{
			auto FieldValue = *(int64*)FMonoDomain::Object_Unbox(
				FMonoDomain::Field_Get_Value_Object(FMonoDomain::Domain, Field, (MonoObject*)InMonoClass));

			InNames.Add({FieldName, FieldValue});
		}
	}

	InEnum->SetEnums(InNames, UEnum::ECppForm::Regular);
}
