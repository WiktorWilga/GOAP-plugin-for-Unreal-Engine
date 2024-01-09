// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPPlanner.h"

#include "GOAPAction.h"
#include "GOAPActionsExecutor.h"
#include "GOAPAgent.h"
#include "GOAPSolver.h"
#include "GOAPGoal.h"

UGOAPPlanner::UGOAPPlanner()
{
	// planner tick settings - init to tick two times per second; on each tick goal will be validated and new plan
	// will be generated if required
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.5f;
}

void UGOAPPlanner::BeginPlay()
{
	Super::BeginPlay();

	// create solver
	Solver = NewObject<UGOAPSolver>(this, SolverImplementationClass);
	ensureMsgf(Solver, TEXT("GOAPPlanner can't create solver of given class!"));
	Solver->InitializeSolver(this);
	
	// create goals objects
	for(auto GoalClass : GoalsClasses)
	{
		if(!IsValid(GoalClass))
			continue;
	
		UGOAPGoal* NewGoal = NewObject<UGOAPGoal>(this, GoalClass);
		if(!NewGoal)
		{
			UE_LOG(LogGOAP, Error, TEXT("Cant create Goal of class %s!"), *GoalClass->GetName());
			continue;
		}
	
		NewGoal->AgentActor = TScriptInterface<IGOAPAgent>(GetOwner());
		Goals.Add(NewGoal);
	}
}

void UGOAPPlanner::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// // @warning: by default it not tick every frame (and shouldn't tick every frame) - see constructor
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// validate if current goal is best goal on each tick
	UGOAPGoal* CurrentBestGoal = FindBestScoredGoal();
	if(PursuedGoal != CurrentBestGoal)
	{
		UObject* ActiveAction = IGOAPActionsExecutor::Execute_GetActiveAction(ActionsExecutor);
		if(ActiveAction)
		{
			// can't cancel ability so can't change goal at this moment
			if(!IGOAPAction::Execute_CanBeCanceled(ActiveAction))
				return;
			// cancel ability if possible to switch goal; must be CDO object
			IGOAPActionsExecutor::Execute_CancelAction(ActionsExecutor, ActiveAction);
		}
		// switch to other goal
		SetPursuedGoal(CurrentBestGoal);
	}
}

void UGOAPPlanner::InitializePlanner(UGOAPMemoryComponent* InMemoryComponent, UObject* InActionExecutor,
	TArray<UObject*> InActions)
{
	AgentsMemoryComponent = InMemoryComponent;
	ensureMsgf(AgentsMemoryComponent, TEXT("Not valid UGOAPMemoryComponent passed to InitializePlanner!"));
	
	ActionsExecutor = InActionExecutor;
	ensureMsgf(ActionsExecutor, TEXT("Not valid ActionsExecutor passed to InitializePlanner!"));
	// check if given ActionExecutor implements interface
	ensureMsgf(ActionsExecutor->Implements<UGOAPActionsExecutor>(), TEXT("Object passed to InitializePlanner as "
																  "ActionsExecutor have to be implement IGOAPActionsExecutor!"));

	// check which objects implement IGOAPAction interface and save them
	for(UObject* Action : InActions)
	{
		if(Action && Action->Implements<UGOAPAction>())
		{
			Actions.Add(Action);
		}
	}
}

void UGOAPPlanner::AddGoal(TSubclassOf<UGOAPGoal> GoalClass)
{
	if(!IsValid(GoalClass))
		return;
	
	// check if this goal is not already in the array
	const int32 GoalIndex = Goals.IndexOfByPredicate([GoalClass](const UGOAPGoal* Goal)
	{
		return Goal->GetClass() == GoalClass;
	});
	
	if(GoalIndex != INDEX_NONE)
		return;
	
	UGOAPGoal* NewGoal = NewObject<UGOAPGoal>(this, GoalClass);
	if(!NewGoal)
	{
		UE_LOG(LogGOAP, Error, TEXT("Cant create Goal of class %s!"), *GoalClass->GetName());
		return;
	}
	
	NewGoal->AgentActor = TScriptInterface<IGOAPAgent>(GetOwner());
	Goals.Add(NewGoal);
}

void UGOAPPlanner::RemoveGoal(TSubclassOf<UGOAPGoal> GoalClass)
{
	if(!IsValid(GoalClass))
		return;

	// find goal
	const int32 GoalIndex = Goals.IndexOfByPredicate([GoalClass](const UGOAPGoal* Goal)
	{
		return Goal->GetClass() == GoalClass;
	});

	if(GoalIndex == INDEX_NONE)
		return;

	if(GetPursuedGoal() == Goals[GoalIndex])
	{
		// current goal is goal to remove
		UObject* ActiveAction = IGOAPActionsExecutor::Execute_GetActiveAction(ActionsExecutor);
		if(ActiveAction)
		{
			// cancel ability if possible to switch goal; must be CDO object
			IGOAPActionsExecutor::Execute_CancelAction(ActionsExecutor, ActiveAction);
			// reset pursued goal to force the planner to find another plan
			PursuedGoal = nullptr;
		}
	}

	Goals.RemoveAt(GoalIndex);
}

TArray<UObject*> UGOAPPlanner::GetActions()
{
	TArray<UObject*> Result;
	for(UObject* Action : Actions)
	{
		if(Action && Action->GetClass()->ImplementsInterface(UGOAPAction::StaticClass()))
		{
			Result.Add(Action);
		}
	}
	return Result;
}

UGOAPGoal* UGOAPPlanner::FindBestScoredGoal()
{
	float BestScore = 0.0f;
	UGOAPGoal* BestGoal = nullptr;
	for(const auto Goal : Goals)
	{
		if(!Goal->IsGoalValid())
			continue;
			
		const float GoalScore = Goal->GetGoalScore();
		if(GoalScore > BestScore)
		{
			BestScore = GoalScore;
			BestGoal = Goal;
		}
	}
	return BestGoal;
}

void UGOAPPlanner::SetPursuedGoal(UGOAPGoal* Goal)
{
	PursuedGoal = Goal;
	TArray<FGOAPActionWithTargetData> Plan = Solver->FindPlanForGoal(PursuedGoal);
	if(Plan.Num() > 0)
	{
		// info log
		UE_LOG(LogGOAP, Log, TEXT("New goal set: %s"), *PursuedGoal->GetName());
		for(auto Action : Plan)
		{
			UE_LOG(LogGOAP, Log, TEXT("	- %s"), *Action.Action->GetName());
		}
		// execute plan
		const bool ExecutionBegun = ExecutePlan(Plan);
		if(ExecutionBegun)
		{
			UE_LOG(LogGOAP, Log, TEXT("Execution started!"));
		}
		else
		{
			UE_LOG(LogGOAP, Log, TEXT("Execution failed!"));
		}
	}
}

bool UGOAPPlanner::ExecutePlan(TArray<FGOAPActionWithTargetData> Plan)
{
	if(Plan.Num() == 0 )
		return false;

	ExecutingPlan = Plan;
	ExecutingPlanActionIndex = 0;
	CurrentActionHandle.Reset();

	return ExecuteCurrentAction();
}

bool UGOAPPlanner::ExecuteCurrentAction()
{
	// can't activate action if another isn't finished
	if(CurrentActionHandle.IsValid())
		return false;
	
	CurrentActionHandle = Cast<IGOAPActionsExecutor>(ActionsExecutor)->OnActionEnded.AddUObject(this, &UGOAPPlanner::OnAbilityEnded);

	// set target data for ability before activation
	IGOAPAction::Execute_SetActionTargetData(ExecutingPlan[ExecutingPlanActionIndex].Action,
		ExecutingPlan[ExecutingPlanActionIndex].TargetData);
	const bool ActivationSuccess =
		IGOAPActionsExecutor::Execute_TryActivateActionByClass(ActionsExecutor,
			ExecutingPlan[ExecutingPlanActionIndex].Action->GetClass());
	if(!ActivationSuccess)
		return false;

	return true;
}

void UGOAPPlanner::OnAbilityEnded(const UObject* Action, bool bSuccess)
{
	Cast<IGOAPActionsExecutor>(ActionsExecutor)->OnActionEnded.Remove(CurrentActionHandle);
	CurrentActionHandle.Reset();
	
	if(!bSuccess)
	{
		FinishExecutePlan();
		return;
	}
	
	if(ExecutingPlanActionIndex == ExecutingPlan.Num()-1)
	{
		FinishExecutePlan();
	}
	else
	{
		++ExecutingPlanActionIndex;
		const bool ActivationSuccess = ExecuteCurrentAction();
		if(!ActivationSuccess)
			FinishExecutePlan();
	}
}

bool UGOAPPlanner::IsCurrentlyExecutingPlan() const
{
	return ExecutingPlan.IsValidIndex(ExecutingPlanActionIndex);
}

void UGOAPPlanner::FinishExecutePlan()
{
	ExecutingPlan.Reset();
	ExecutingPlanActionIndex = -1;
	
	PursuedGoal = nullptr;
}