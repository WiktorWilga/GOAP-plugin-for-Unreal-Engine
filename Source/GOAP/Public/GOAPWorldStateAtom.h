// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "GameplayTagContainer.h"
#include "GOAPWorldStateAtom.generated.h"

/**
 * Object responsible for checking and returning one actor's parameter.
 */
UCLASS(Blueprintable, BlueprintType)
class GOAP_API UGOAPWorldStateAtom : public UObject
{
	GENERATED_BODY()

public:

	friend class UGOAPWorldStateProvider;

	/**
	 * Update world state atom data.
	 * @warning have to be implemented in all classes
	 * @warning have to update value of WorldStateValue
	 */
	UFUNCTION(BlueprintNativeEvent)
	void UpdateWorldStateAtomData();

protected:

	/** Tag which identify which type of data is this atom return. */
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag WorldStateAtomTag;

	/** Reference to actor for which this atom checks the state. */
	UPROPERTY(BlueprintReadOnly)
	AActor* OwnerActor = nullptr;

private:

	/** World state atom data, its can be data of any type. UGOAPWorldStateProvider used it to return specified type. */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FGOAPWorldStateValue WorldStateValue;
	
};
