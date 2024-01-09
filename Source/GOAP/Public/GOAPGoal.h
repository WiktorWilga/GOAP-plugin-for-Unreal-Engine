// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "GOAPPlanner.h"
#include "GOAPGoal.generated.h"

class IGOAPAgent;

/**
 * Goal for planner. Defines what AI want to achieve and how important it is.
 */
UCLASS(Blueprintable, BlueprintType)
class GOAP_API UGOAPGoal : public UObject
{
	GENERATED_BODY()

public:

	friend class UGOAPPlanner;

	/** Return true if goal is valid and can be considered in planning. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool IsGoalValid();
	
	/** Return value in range <0,1>. 0 - goal isn't important. 1 - goal is very important. */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	float GetGoalScore();

	/** Return world state that satisfy goal. */
	UFUNCTION(BlueprintCallable)
	FGOAPWorldStateData GetDesiredWorldState();

protected:
	
	/**
	 * Update current desired world state.
	 * @warning have to be implemented in all classes
	 * @warning have to update value of DesiredWorldState
	 */
	UFUNCTION(BlueprintNativeEvent)
	void UpdateDesiredWorldState();

	/** Reference to actor for which this goal is generated (planner owner actor). */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TScriptInterface<IGOAPAgent> AgentActor;

	/** Current desired world state. Is updated on each GetDesiredWorldState use. */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	FGOAPWorldStateData DesiredWorldState;
	
};