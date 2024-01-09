// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GOAPWorldStateFunctionLibrary.generated.h"

class UGOAPWorldStatePayload;

/**
 * Utilities for world state.
 */
UCLASS()
class GOAP_API UGOAPWorldStateFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Add world state data to world state array. If value of specified key is already in array its value will
	 * be override. If there isn't this key - will be added.
	 */
	UFUNCTION(BlueprintCallable)
	static void AddDataToWorldStateArray(UPARAM(ref) TArray<FGOAPWorldStateData>& WorldStateArray, const FGOAPWorldStateData& NewData);
	/** Return true if DesiredWorldState is currently met. */
	UFUNCTION(BlueprintCallable)
	static bool IsWorldStateActual(const FGOAPWorldStateData& DesiredWorldState);
	/**
	 * Return world state value for given key. If this key is in WithCurrentWorldState it return value from this array,
	 * otherwise return actual (current) world state value. 
	 */
	UFUNCTION(BlueprintCallable)
	static FGOAPWorldStateValue GetActualWorldStateValue(const FGOAPWorldStateKey& Key,
		const TArray<FGOAPWorldStateData>& WithCurrentWorldState);

	/** Helper template to set data in FGOAPWorldStateValue. */
	template <typename ValueType>
	static FGOAPWorldStateValue SetPayload(FGOAPWorldStateValue& WorldStateValue, UClass* PayloadType, ValueType InValue)
	{
		// Payload object of proper type is already crated - simply override value
		if(WorldStateValue.Payload && WorldStateValue.Payload->GetClass() == PayloadType)
		{
			WorldStateValue.Payload->SetPayloadValueFromRawData(&InValue);
		}
		// There isn't payload object or is of other type - create new payload and set value
		else
		{
			UGOAPWorldStatePayload* Payload = NewObject<UGOAPWorldStatePayload>(GetTransientPackage(), PayloadType);
			Payload->SetPayloadValueFromRawData(&InValue);
			WorldStateValue.Payload = Payload;
		}
		
		return WorldStateValue;
	}
};