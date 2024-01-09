// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPSolver.h"

void UGOAPSolver::InitializeSolver(UGOAPPlanner* ForPlanner)
{
	Planner = ForPlanner;
}

TArray<FGOAPActionWithTargetData> UGOAPSolver::FindPlanForGoal(UGOAPGoal* Goal)
{
	return TArray<FGOAPActionWithTargetData>();
}

