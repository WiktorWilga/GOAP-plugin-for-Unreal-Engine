// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GOAPActionsExecutor.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FActionEndedDelegate, const UObject*, bool);

UINTERFACE(MinimalAPI)
class UGOAPActionsExecutor : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface representing GOAP actions executor that will perform plan.
 * If you use GAS this interface probably should be implemented by Ability System Component.
 */
class GOAP_API IGOAPActionsExecutor
{
	GENERATED_BODY()

public:

	/** Delegate that should be called when action ended. */
	FActionEndedDelegate OnActionEnded;

	/** Stop performing given action (if that action is currently being performed). */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void CancelAction(UObject* Action);
	
	/** Return action that is currently being performed. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UObject* GetActiveAction();

	/** Activate action if possible. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool TryActivateActionByClass(TSubclassOf<UObject> ActionClass);
	
};
