// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GOAPSolver.generated.h"

class UGOAPGoal;

/**
 * Solver for finding solution (plan) for specified problem (goal). Used by BHPlanner.
 */
UCLASS(Abstract)
class GOAP_API UGOAPSolver : public UObject
{
	GENERATED_BODY()

public:

	/** Prepare solver to works for specified planner. */
	virtual void InitializeSolver(class UGOAPPlanner* ForPlanner);

	/** Return complete plan for specified goal. Can return empty array if goal can't be satisfied. */
	virtual TArray<struct FGOAPActionWithTargetData> FindPlanForGoal(UGOAPGoal* Goal);

protected:

	/** Reference to planner for which this solver is working. */
	UGOAPPlanner* Planner;
	
};