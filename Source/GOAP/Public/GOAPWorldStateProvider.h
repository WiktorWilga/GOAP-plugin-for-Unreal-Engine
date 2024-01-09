// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "GOAPWorldStateAtom.h"
#include "Components/ActorComponent.h"
#include "GOAPWorldStateProvider.generated.h"

/**
* Each actor which can be considered by planner need has this component. Contains all world state data associated
* with owner actor. Manage world state atoms.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOAP_API UGOAPWorldStateProvider : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGOAPWorldStateProvider();

	/** Return true if provider has specified data (has specified world state atom). */
	UFUNCTION(BlueprintCallable)
	bool HasWorldStateValue(const FGameplayTag WorldStateAtomTag);
	/** Allows get specified world state value. This value can be later casted to specified value by
	 * GetWorldStateValue node. */
	UFUNCTION(BlueprintCallable)
	FGOAPWorldStateValue GetWorldStateValue(const FGameplayTag WorldStateAtomTag);
	
protected:
	
	virtual void BeginPlay() override;

private:

	/** Actor with this component will has only world state data which is specified in this array. */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGOAPWorldStateAtom>> WorldSateAtomsClasses;

	/** Instances of atoms to check world state. */
	UPROPERTY()
	TArray<UGOAPWorldStateAtom*> WorldSateAtoms;
	
};
