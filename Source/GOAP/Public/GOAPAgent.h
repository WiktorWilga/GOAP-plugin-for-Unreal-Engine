// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPMemoryComponent.h"
#include "GOAPPlanner.h"
#include "UObject/Interface.h"
#include "GOAPAgent.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UGOAPAgent : public UInterface
{
	GENERATED_BODY()
};

/**
 * This interface have to be implemented by each actor class that want to use GOAP planner.
 */
class GOAP_API IGOAPAgent
{
	GENERATED_BODY()

public:

	/** Return agent's planner component. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UGOAPPlanner* GetGOAPPlanner();

	/** Return agent's memory component. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UGOAPMemoryComponent* GetGOAPMemoryComponent();
};
