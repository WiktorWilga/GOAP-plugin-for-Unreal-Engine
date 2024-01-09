// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPSolver_Backward.h"

#include "GOAPAction.h"
#include "GOAPWorldStateFunctionLibrary.h"
#include "GOAPGoal.h"
#include "GOAPWorldStatePayloads.h"

TArray<FGOAPActionWithTargetData> UGOAPSolver_Backward::FindPlanForGoal(UGOAPGoal* Goal)
{
	if(!Goal)
	{
		return TArray<FGOAPActionWithTargetData>();
	}

	UE_LOG(LogGOAP, Log, TEXT("Start looking for solution for goal: %s (backward planning)"), *Goal->GetName());
	
	TArray<FGOAPTreeNode> KnownNodes;
	TArray<int32> AvailableNodes;
	TArray<TArray<int32>> Solutions;
	TArray<TArray<int32>> PreparedSolutions;

	// initial state
	FGOAPTreeNode InitNode;
	if(!UGOAPWorldStateFunctionLibrary::IsWorldStateActual(Goal->GetDesiredWorldState()))
	{
		InitNode.DesiredWorldStates.Add(FGOAPNodeDesiredWorldState(Goal->GetDesiredWorldState(), 0, -1));
	}
	if(InitNode.IsGoalSatisfiedInNode())
	{
		// goal is satisfied without any actions
		return TArray<FGOAPActionWithTargetData>();
	}
	InitNode.PathToNode.Add(0);
	KnownNodes.Add(InitNode);

	// find all solutions
	ExpandNode(InitNode, KnownNodes, AvailableNodes, Solutions);
	int32 CurrentNodeIndex = 0;
	int32 VisitedNodesNum = 1;
	while(AvailableNodes.Num() > 0)
	{
		CurrentNodeIndex = AvailableNodes.Last();
		AvailableNodes.Remove(CurrentNodeIndex);
		ExpandNode(KnownNodes[CurrentNodeIndex], KnownNodes, AvailableNodes, Solutions);
		++VisitedNodesNum;
	}
	UE_LOG(LogGOAP, Log, TEXT("Summary visited nodes number: %d"), VisitedNodesNum);

	// prepare plans
	for(auto Solution : Solutions)
	{
		PreparedSolutions.Add(PrepareSolution(KnownNodes, Solution));
	}

	// after prepare plans some of which can be the same - remove duplicates
	RemoveIdenticalSolutions(KnownNodes, PreparedSolutions);

	// info log
	for(int32 PlanIndex = 0; PlanIndex < PreparedSolutions.Num(); ++PlanIndex)
	{
		UE_LOG(LogGOAP, Log, TEXT("Plan %d"), PlanIndex+1);
		int32 PlanCost = 0;
		for(auto NodeIndex : PreparedSolutions[PlanIndex])
		{
			PlanCost += KnownNodes[NodeIndex].NodeCost;
			if(NodeIndex != 0)
			{
				UE_LOG(LogGOAP, Log, TEXT("	- %s"), *KnownNodes[NodeIndex].DirectAction->GetName());
			}
		}
		UE_LOG(LogGOAP, Log, TEXT("Plan cost: %d"), PlanCost);
	}

	// find best plan (min cost)
	const int32 BestPlanIndex = GetBestSolution(KnownNodes, PreparedSolutions);
	UE_LOG(LogGOAP, Log, TEXT("Best plan index: %d"), BestPlanIndex+1);

	return BestPlanIndex != -1 ? BuildActionsPlanForPath(KnownNodes, PreparedSolutions[BestPlanIndex]) : TArray<FGOAPActionWithTargetData>();
}

TArray<UObject*> UGOAPSolver_Backward::FindActionChangingWorldState(const FGOAPWorldStateData& DesiredWorldState) const
{
	TArray<UObject*> Result;
	for(auto Action : Planner->GetActions())
	{
		if(Action && IGOAPAction::Execute_CanChangeWorldState(Action, DesiredWorldState, Planner->GetAgent()))
		{
			Result.Add(Action);
		}
	}
	return Result;
}

bool UGOAPSolver_Backward::ExpandNode(FGOAPTreeNode Node, TArray<FGOAPTreeNode>& KnownNodes, TArray<int32>& AvailableNodes,
	TArray<TArray<int32>>& Solutions) const
{
	// check if node is solution - simply add to known solutions and not expand this node
	if(Node.IsGoalSatisfiedInNode())
	{
		Solutions.Add(Node.PathToNode);
		return false;
	}

	// find children nodes
	bool bAnyNodeAdded = false;
	for(int32 DesiredStateIndex = 0; DesiredStateIndex < Node.DesiredWorldStates.Num(); ++DesiredStateIndex)
	{
		// this desired world state is already satisfied
		if(Node.DesiredWorldStates[DesiredStateIndex].SatisfiedByActionIndex != -1)
			continue;
		
		FGOAPWorldStateData DesiredState = Node.DesiredWorldStates[DesiredStateIndex].DesiredState;
		TArray<UObject*> ValidActions = FindActionChangingWorldState(DesiredState);
		for(auto Action : ValidActions)
		{
			// prepare new node
			FGOAPTreeNode NewNode = Node;
			// direct action leading to this node and associated target data
			NewNode.DirectAction = Action;
			NewNode.DirectTargetData = DesiredState;
			// solved desired state
			NewNode.SolvedDesiredWorldStateIndex = DesiredStateIndex;
			// set temporal value for SatisfiedByActionIndex to tread it as satisfied, it will be set to proper value
			// after add to KnownNodes
			NewNode.DesiredWorldStates[DesiredStateIndex].SatisfiedByActionIndex = 0;
			// desired world state - node is not valid if some of action precondition has the same key as some of
			// current node's desired world states
			TArray<FGOAPWorldStateData> Preconditions = IGOAPAction::Execute_GetWorldStatePreconditions(Action, DesiredState, Planner->GetAgent());
			const bool bWillDuplicateDesiredWorldState =
				Preconditions.ContainsByPredicate([&NewNode](const FGOAPWorldStateData& Element)
			{
				for(const auto& NodeDesiredState : NewNode.DesiredWorldStates)
				{
					if(NodeDesiredState.SatisfiedByActionIndex == -1 &&
						NodeDesiredState.DesiredState.WorldStateKey == Element.WorldStateKey)
					{
						return true;
					}
				}
				return false;
			});
			if(bWillDuplicateDesiredWorldState)
			{
				continue;
			}
			// add to arrays and update path to node
			int32 NewNodeIndex = KnownNodes.Add(NewNode);
			for(auto& Precondition : Preconditions)
			{
				KnownNodes[NewNodeIndex].DesiredWorldStates.Add(FGOAPNodeDesiredWorldState(Precondition, NewNodeIndex, -1));
			}
			KnownNodes[NewNodeIndex].DesiredWorldStates[DesiredStateIndex].SatisfiedByActionIndex = NewNodeIndex;
			KnownNodes[NewNodeIndex].PathToNode.Add(NewNodeIndex);
			AvailableNodes.Add(NewNodeIndex);
			bAnyNodeAdded = true;
		}
	}
	return bAnyNodeAdded;
}

TArray<FGOAPActionWithTargetData> UGOAPSolver_Backward::BuildActionsPlanForPath(const TArray<FGOAPTreeNode>& KnownNodes,
	TArray<int32> Path) const
{
	TArray<FGOAPActionWithTargetData> Result;
	for(auto PathPoint : Path)
	{
		UObject* Action = KnownNodes[PathPoint].DirectAction;
		if(Action)
		{
			Result.Add(FGOAPActionWithTargetData(KnownNodes[PathPoint].DirectAction, KnownNodes[PathPoint].DirectTargetData));
		}
	}
	return Result;
}

TArray<int32> UGOAPSolver_Backward::PrepareSolution(TArray<FGOAPTreeNode>& KnownNodes,
	TArray<int32> Solution) const
{
	// its backward planning so result need to be reversed to has proper order
	Algo::Reverse(Solution);
	// remove all not actual actions
	TArray<int32> NodesToRemove;
	bool bContinue;
	do
	{
		bContinue = false;
		// build current world state to time when some action is not actual - remove this action and all associated
		// actions and start building current world state from scratch
		TArray<FGOAPWorldStateData> CurrentWorldState;
		for(auto NodeIndex : Solution)
		{
			// if node already are removed from plan
			if(NodeIndex == 0 || NodesToRemove.Contains(NodeIndex))
				continue;

			FGOAPWorldStateData ActionEffect =
				KnownNodes[NodeIndex].DesiredWorldStates[KnownNodes[NodeIndex].SolvedDesiredWorldStateIndex].DesiredState;
			
			if(IsActionEffectAlreadyActualForPlan(CurrentWorldState, ActionEffect))
			{
				NodesToRemove.Append(GetAllNodesUsedForDesiredState(KnownNodes, Solution, NodeIndex));
				bContinue = true;
				break;
			}
			
			// update node current data
			KnownNodes[NodeIndex].CurrentWorldStates = CurrentWorldState;
			// update current world state for next nodes
			UGOAPWorldStateFunctionLibrary::AddDataToWorldStateArray(CurrentWorldState, ActionEffect);
		}
	} while(bContinue);
	
	TArray<int32> ResultSolution;
	for(auto SolutionNodeIndex : Solution)
	{
		if(!NodesToRemove.Contains(SolutionNodeIndex))
		{
			if(SolutionNodeIndex != 0)
			{
				// calculate cost only for relevant nodes
				KnownNodes[SolutionNodeIndex].NodeCost = IGOAPAction::Execute_GetActionCost(
					KnownNodes[SolutionNodeIndex].DirectAction,KnownNodes[SolutionNodeIndex].DirectTargetData,
					Planner->GetAgent(), KnownNodes[SolutionNodeIndex].CurrentWorldStates);
			}
			ResultSolution.Add(SolutionNodeIndex);
		}
	}
	return ResultSolution;
}

bool UGOAPSolver_Backward::IsActionEffectAlreadyActualForPlan(const TArray<FGOAPWorldStateData>& CurrentWorldState,
	const FGOAPWorldStateData& ActionEffect) const
{
	// check if node's current world state satisfy desired world state
	for(auto& WorldState : CurrentWorldState)
	{
		if(ActionEffect.WorldStateKey == WorldState.WorldStateKey &&
			ActionEffect.WorldStateValue.Payload->IsEqual(WorldState.WorldStateValue.Payload))
		{
			return true;
		}
	}
	// check if desired state is actual world state
	return UGOAPWorldStateFunctionLibrary::IsWorldStateActual(ActionEffect);
}

void UGOAPSolver_Backward::RemoveIdenticalSolutions(const TArray<FGOAPTreeNode>& KnownNodes,
	TArray<TArray<int32>>& PreparedSolutions) const
{
	for(int32 OutIndex = PreparedSolutions.Num()-1; OutIndex >= 1; --OutIndex)
	{
		for(int32 InIndex = OutIndex-1; InIndex >= 0; --InIndex)
		{
			if(AreSolutionsIdentical(KnownNodes, PreparedSolutions[OutIndex], PreparedSolutions[InIndex]))
			{
				PreparedSolutions.RemoveAt(OutIndex);
				break;
			}
		}
	}
}

bool UGOAPSolver_Backward::AreSolutionsIdentical(const TArray<FGOAPTreeNode>& KnownNodes, const TArray<int32>& SolutionOne,
	const TArray<int32>& SolutionTwo) const
{
	if(SolutionOne.Num() != SolutionTwo.Num())
	{
		return false;
	}
	
	for(int32 ActionIndex = 0; ActionIndex < SolutionOne.Num(); ++ActionIndex)
	{
		if(KnownNodes[SolutionOne[ActionIndex]].DirectAction != KnownNodes[SolutionTwo[ActionIndex]].DirectAction ||
			KnownNodes[SolutionOne[ActionIndex]].DirectTargetData.WorldStateKey.WorldStateActor !=
			KnownNodes[SolutionTwo[ActionIndex]].DirectTargetData.WorldStateKey.WorldStateActor)
		{
			return false;
		}
	}
	return true;
}

int32 UGOAPSolver_Backward::GetBestSolution(const TArray<FGOAPTreeNode>& KnownNodes,
	const TArray<TArray<int32>>& PreparedSolutions) const
{
	int32 BestIndex = -1;
	int32 MinCost = MAX_int32;
	for(int32 SolutionIndex = 0; SolutionIndex < PreparedSolutions.Num(); ++SolutionIndex)
	{
		int32 PlanCost = 0;
		for(auto& NodeIndex : PreparedSolutions[SolutionIndex])
		{
			PlanCost += KnownNodes[NodeIndex].NodeCost;
		}
		if(PlanCost < MinCost)
		{
			MinCost = PlanCost;
			BestIndex = SolutionIndex;
		}
	}
	return BestIndex;
}

TArray<int32> UGOAPSolver_Backward::GetAllNodesUsedForDesiredState(const TArray<FGOAPTreeNode>& KnownNodes,
	TArray<int32> Solution, int32 StartingNodeIndex) const
{
	TArray<int32> Result;
	Result.Add(StartingNodeIndex);
	for(auto NodePreconditionIndex : KnownNodes[StartingNodeIndex].GetActionPreconditionsIndexes())
	{
		for(auto SolutionNodeIndex : Solution)
		{
			if(KnownNodes[SolutionNodeIndex].SolvedDesiredWorldStateIndex == NodePreconditionIndex)
			{
				// @warning: recursion - for complicated situations performance can be impacted
				Result.Append(GetAllNodesUsedForDesiredState(KnownNodes, Solution, SolutionNodeIndex));
			}
		}
	}
	return Result;
}
