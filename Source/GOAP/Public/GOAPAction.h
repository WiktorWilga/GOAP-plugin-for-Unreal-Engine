// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GOAPWorldStateAtom.h"
#include "GOAPAction.generated.h"

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UGOAPAction : public UInterface
{
	GENERATED_BODY()
};

/**
 * Object representing GOAP action has to implement this interface to can be used by NPC with GOAP planner.
 * If you use GAS this interface probably should be implemented by Gameplay Ability.
 */
class GOAP_API IGOAPAction
{
	GENERATED_BODY()

public:

	/** Return true if this ability change world state to desired value. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanChangeWorldState(FGOAPWorldStateData DesiredWorldState, AActor* AgentActor);

	/** Return preconditions that need to be satisfied to run ability. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FGOAPWorldStateData> GetWorldStatePreconditions(FGOAPWorldStateData ForDesiredWorldState, AActor* AgentActor);

	/** Set desired world state value which this ability will try to achieve after activation. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetActionTargetData(FGOAPWorldStateData TargetData);

	/** Return cost of use this ability to achieve DesiredWorldState; valid only if CanChangeWorldState returns true
	 * for the same parameters. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetActionCost(FGOAPWorldStateData DesiredWorldState, AActor* AgentActor,
		const TArray<FGOAPWorldStateData>& WithCurrentWorldState);

	/** Return true if action can be applicable on specified target actor and if possible effect world state. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool GetActionEffectWithContextActor(AActor* AgentActor, AActor* TargetActor, FGOAPWorldStateData& EffectWorldState);

	/** Return true if action can be safely canceled at this moment. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanBeCanceled();
};
