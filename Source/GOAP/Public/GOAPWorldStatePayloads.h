// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "GOAPWorldStateFunctionLibrary.h"
#include "UObject/NoExportTypes.h"
#include "GOAPWorldStatePayloads.generated.h"

/**
 * Base class for BHWorldStateValue payload which contain data. Inherit from this class to create specified value type.
 */
UCLASS(Abstract)
class GOAP_API UGOAPWorldStatePayload : public UObject
{
	GENERATED_BODY()

public:

	void* Value = nullptr;

	/** Set payload value form pointer to void.
	 * @warning not check if pointer points to valid data. */
	virtual void SetPayloadValueFromRawData(void* DataValueData) PURE_VIRTUAL(UBHWorldStatePayload::SetValueFromRawData);
	
	/** Check if given payload has the same value as this payload. */
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) PURE_VIRTUAL(UBHWorldStatePayload::IsEqual, return false; ); 
	
	/** Return true if given world state value is of given type. */
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool IsWorldStateValueOfThisType(FGOAPWorldStateValue WorldStateValue, FName TypeName)
	{
		if(WorldStateValue.Payload == nullptr)
			return false;

		return (WorldStateValue.Payload->GetClass()->GetFName() == TypeName);
	}
};

/**
 * Payload to store a bool variable.
 */
UCLASS()
class UGOAPWorldStatePayloadBool : public UGOAPWorldStatePayload
{
	GENERATED_BODY()

public:

	UPROPERTY()
	bool Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<bool*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadBool* Other = Cast<UGOAPWorldStatePayloadBool>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static bool GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadBool>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, bool InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};

/**
 * Payload to store a int32 variable.
 */
UCLASS()
class UGOAPWorldStatePayloadInt : public UGOAPWorldStatePayload
{
	GENERATED_BODY()

public:

	UPROPERTY()
	int32 Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<int32*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadInt* Other = Cast<UGOAPWorldStatePayloadInt>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static int32 GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadInt>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, int32 InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};

/**
 * Payload to store a float variable.
 * @warning: in fact this payload is for double variable in UE5 and real float in UE4. this because in UE5 in BP float
 * variable is really of type double.
 */
UCLASS()
class UGOAPWorldStatePayloadFloat : public UGOAPWorldStatePayload
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	double Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<double*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadFloat* Other = Cast<UGOAPWorldStatePayloadFloat>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static double GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadFloat>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, double InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}

	// for UE4
	// UPROPERTY()
	// float Value;
	//
	// virtual void SetPayloadValueFromRawData(void* DataValueData) override
	// {
	// 	Value = *(static_cast<float*>(DataValueData));
	// }
	// virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	// {
	// 	const UGOAPWorldStatePayloadFloat* Other = Cast<UGOAPWorldStatePayloadFloat>(OtherPayload);
	// 	if(!Other)
	// 		return false;
	// 	return Value == Other->Value;
	// }
	//
	// UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	// static float GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	// {
	// 	return Cast<UGOAPWorldStatePayloadFloat>(WorldStateValue.Payload)->Value;
	// }
	// UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	// static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, float InValue)
	// {
	// 	return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	// }

};

/**
 * Payload to store a FVector variable.
 */
UCLASS()
class UGOAPWorldStatePayloadVector : public UGOAPWorldStatePayload
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FVector Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<FVector*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadVector* Other = Cast<UGOAPWorldStatePayloadVector>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FVector GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadVector>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, FVector InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};

/**
 * Payload to store a FString variable.
 */
UCLASS()
class UGOAPWorldStatePayloadString : public UGOAPWorldStatePayload
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	FString Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<FString*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadString* Other = Cast<UGOAPWorldStatePayloadString>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FString GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadString>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, FString InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};

/**
 * Payload to store a AActor reference.
 */
UCLASS()
class UGOAPWorldStatePayloadActor : public UGOAPWorldStatePayload
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	AActor* Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<AActor**>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayload* OtherPayload) override
	{
		const UGOAPWorldStatePayloadActor* Other = Cast<UGOAPWorldStatePayloadActor>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}

	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static AActor* GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadActor>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, AActor* InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};