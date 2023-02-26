﻿#pragma once

template <typename T>
T& TGarbageCollectionHandleMapping<T>::operator[](const FGarbageCollectionHandle& InKey)
{
	return GarbageCollectionHandle2T[InKey];
}

template <typename T>
const T& TGarbageCollectionHandleMapping<T>::operator[](const FGarbageCollectionHandle& InKey) const
{
	return GarbageCollectionHandle2T[InKey];
}

template <typename T>
void TGarbageCollectionHandleMapping<T>::Empty()
{
	GarbageCollectionHandle2T.Empty();
}

template <typename T>
T& TGarbageCollectionHandleMapping<T>::Emplace(FGarbageCollectionHandle&& InKey, T&& InValue)
{
	return GarbageCollectionHandle2T.Emplace(Forward<FGarbageCollectionHandle>(InKey), Forward<T>(InValue));
}

template <typename T>
int32 TGarbageCollectionHandleMapping<T>::Remove(const FGarbageCollectionHandle& InKey)
{
	return GarbageCollectionHandle2T.Remove(InKey);
}

template <typename T>
int32 TGarbageCollectionHandleMapping<T>::Remove(const MonoObject* InKey)
{
	for (const auto& Pair : GarbageCollectionHandle2T)
	{
		if (Pair.Key == InKey)
		{
			return GarbageCollectionHandle2T.Remove(Pair.Key);
		}
	}

	return 0;
}

template <typename T>
T* TGarbageCollectionHandleMapping<T>::Find(const MonoObject* InMonoObject)
{
	for (auto& Pair : GarbageCollectionHandle2T)
	{
		if (Pair.Key == InMonoObject)
		{
			return &Pair.Value;
		}
	}

	return nullptr;
}

template <typename T>
T* TGarbageCollectionHandleMapping<T>::Find(const FGarbageCollectionHandle& InKey)
{
	return GarbageCollectionHandle2T.Find(InKey);
}

template <typename T>
bool TGarbageCollectionHandleMapping<T>::Contains(const FGarbageCollectionHandle& InKey) const
{
	return GarbageCollectionHandle2T.Contains(InKey);
}

template <typename T>
TMap<FGarbageCollectionHandle, T>& TGarbageCollectionHandleMapping<T>::Get()
{
	return GarbageCollectionHandle2T;
}
