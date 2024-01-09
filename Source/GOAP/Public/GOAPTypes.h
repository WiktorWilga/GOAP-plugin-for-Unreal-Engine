// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "GOAPTypes.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGOAP, Log, All);

class UGOAPWorldStatePayload;

/**
 * Uniquely identifies world state data.
 */
USTRUCT(BlueprintType)
struct FGOAPWorldStateKey
{
	GENERATED_BODY()

	FGOAPWorldStateKey() {}

	FGOAPWorldStateKey(AActor* InWorldStateActor, FGameplayTag InWorldStateDataTag)
		: WorldStateActor(InWorldStateActor), WorldStateDataTag(InWorldStateDataTag) {}

	FGOAPWorldStateKey(const FGOAPWorldStateKey& Other)
	: FGOAPWorldStateKey(Other.WorldStateActor, Other.WorldStateDataTag) {}

	bool operator==(const FGOAPWorldStateKey& Other) const
	{
		return Equals(Other);
	}

	bool Equals(const FGOAPWorldStateKey& Other) const
	{
		return WorldStateActor == Other.WorldStateActor &&
			WorldStateDataTag.MatchesTagExact(Other.WorldStateDataTag);
	}

	/** Actor which has world state data. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	AActor* WorldStateActor = nullptr;

	/** World state data tag for WorldStateActor. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FGameplayTag WorldStateDataTag;
	
};

/** FGOAPWorldStateKey needs has hash to be key in TMap. */
FORCEINLINE  uint32 GetTypeHash(const FGOAPWorldStateKey& Thing)
{
	return FCrc::MemCrc32(&Thing, sizeof(FGOAPWorldStateKey));
}

/**
 * Atom world state value. In BP accessible by UBHAIFunctionLibrary functions.
 */
USTRUCT(BlueprintType)
struct FGOAPWorldStateValue
{
	GENERATED_BODY()

	FGOAPWorldStateValue() : Payload(nullptr) {}
	
	explicit FGOAPWorldStateValue(UGOAPWorldStatePayload* InPayload)
		: Payload(InPayload) {}

	/** Payload object with value. */
	UPROPERTY()
	UGOAPWorldStatePayload* Payload;
};

/**
 * Complete world state data - key with value.
 */
USTRUCT(BlueprintType)
struct FGOAPWorldStateData
{
	GENERATED_BODY()

	FGOAPWorldStateData() {}
	FGOAPWorldStateData(FGOAPWorldStateKey Key, FGOAPWorldStateValue Value)
		: WorldStateKey(Key), WorldStateValue(Value) {}

	/** World state ID. */
	UPROPERTY(BlueprintReadWrite)
	FGOAPWorldStateKey WorldStateKey;
	/** World state value. */
	UPROPERTY(BlueprintReadWrite)
	FGOAPWorldStateValue WorldStateValue;
};

