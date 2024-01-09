// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPTypes.h"
#include "GOAPMemoryComponent.h"
#include "GOAPSolver_Backward.h"
#include "Components/ActorComponent.h"
#include "GOAPPlanner.generated.h"

class UGOAPSolver;
class UGOAPGoal;
class IGOAPAction;
class UGOAPAction;

/**
 * Helper struct for storing planned actions and theirs target data.
 */
USTRUCT()
struct FGOAPActionWithTargetData
{
	GENERATED_BODY()
	
	FGOAPActionWithTargetData() {}
	FGOAPActionWithTargetData(UObject* InAction, FGOAPWorldStateData InTargetData)
		: Action(InAction), TargetData(InTargetData) {}

	/** Planned action (ability). */
	UObject* Action = nullptr;
	/** Data required for action. Always is the same as desired world state data, which action needs achieve. */
	UPROPERTY()
	FGOAPWorldStateData TargetData;
};

/**
 * GOAP heart. Manage world state, goals and actions. Make decisions what goal will be considered and after
 * that how to achieve this goal. Planning sequence of few actions and execute them. Check validity of goals
 * and actions. Change current priority or way to achieve it if need.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOAP_API UGOAPPlanner : public UActorComponent
{
	GENERATED_BODY()

public:

	UGOAPPlanner();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Pass all required data to planner before use. */
	UFUNCTION(BlueprintCallable)	void InitializePlanner(UGOAPMemoryComponent* InMemoryComponent, UObject* InActionExecutor, TArray<UObject*> InActions);
	
	/** Return reference to agent which is controlled by this planner. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetAgent() const { return GetOwner(); }
	/** Return agent's memory system component. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UGOAPMemoryComponent* GetAgentsMemoryComponent() const { return AgentsMemoryComponent; }
	/** Return reference to current goal. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UGOAPGoal* GetPursuedGoal() const { return PursuedGoal; }

	/** Add goal to planner at runtime. */
	UFUNCTION(BlueprintCallable)
	void AddGoal(TSubclassOf<UGOAPGoal> GoalClass);
	/** Remove goal from planner at runtime. If removed goals is current pursued goal planner immediately change goal. */
	UFUNCTION(BlueprintCallable)
	void RemoveGoal(TSubclassOf<UGOAPGoal> GoalClass);

	/** Return all actions available for planner. */
	TArray<UObject*> GetActions();
	
protected:

	/** Searches all available goals and return goal with best score. */
	UFUNCTION(BlueprintCallable)
	UGOAPGoal* FindBestScoredGoal();
	/** Set pursued goal, try build plan for it and execute. */
	UFUNCTION(BlueprintCallable)
	void SetPursuedGoal(UGOAPGoal* Goal);
	/** Start performing given plan. Return true if successfully started. */
	bool ExecutePlan(TArray<FGOAPActionWithTargetData> Plan);
	/** Activate action of index ExecutingPlanActionIndex from ExecutingPlan. Return true if successfully activated. */
	bool ExecuteCurrentAction();
	/** Return true if any plan is currently executing. */
	bool IsCurrentlyExecutingPlan() const;
	/** Called to finish executing plan. */
	void FinishExecutePlan();
	
	/** Called when ability finished (properly or canceled). */
	UFUNCTION()
	void OnAbilityEnded(const UObject* Action, bool bSuccess);

	virtual void BeginPlay() override;

private:

	/** Reference to owner's (agent's) actions executor. */
	UPROPERTY()
	UObject* ActionsExecutor;
	/** Reference to owner's (agent's) memory component. */
	UPROPERTY()
	UGOAPMemoryComponent* AgentsMemoryComponent;
	/** Reference to owner's (agent's) actions that can be used in planning.*/
	UPROPERTY()
	TArray<UObject*> Actions;

	/** Solver which will be used to finding plan. */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGOAPSolver> SolverImplementationClass = UGOAPSolver_Backward::StaticClass();
	/** Reference to solver, which is created on BeginPlay by planner. */
	UPROPERTY()
	UGOAPSolver* Solver;

	/** List of potential goals, that planner can consider. */
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGOAPGoal>> GoalsClasses;

	/** List of actual available goals. */
	UPROPERTY()
	TArray<UGOAPGoal*> Goals;

	/** Currently realised goal. */
	UPROPERTY()
	UGOAPGoal* PursuedGoal = nullptr;

	/** If currently is executing plan, here is current action index; -1 otherwise. */
	int32 ExecutingPlanActionIndex = -1;
	/** Currently realized plan. Can be empty if isn't realized any plan. */
	UPROPERTY()
	TArray<FGOAPActionWithTargetData> ExecutingPlan;

	/** Delegate handle for ability end. Can also be used to check if currently is realizing any action. */
	FDelegateHandle CurrentActionHandle;
	
};