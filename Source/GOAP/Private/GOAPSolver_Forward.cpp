// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPSolver_Forward.h"

#include "GOAPWorldStateFunctionLibrary.h"
#include "GOAPGoal.h"
#include "GOAPAction.h"
#include "GOAPWorldStatePayloads.h"

TArray<FGOAPActionWithTargetData> UGOAPSolver_Forward::FindPlanForGoal(UGOAPGoal* Goal)
{
	if(!Goal)
	{
		return TArray<FGOAPActionWithTargetData>();
	}

	UE_LOG(LogGOAP, Log, TEXT("Start looking for solution for goal: %s (forward planning)"), *Goal->GetName());
	
	TArray<FGOAPTreeNode> KnownNodes;
	TArray<int32> AvailableNodes;
	int32 CurrentNodeIndex;

	FGOAPTreeNode InitNode;
	InitNode.CurrentWorldStates = TArray<FGOAPWorldStateData>();
	
	if(!UGOAPWorldStateFunctionLibrary::IsWorldStateActual(Goal->GetDesiredWorldState()))
	{
		InitNode.DesiredWorldStates.Add(Goal->GetDesiredWorldState());
	}

	InitNode.Cost = 0;
	InitNode.Heuristic = InitNode.DesiredWorldStates.Num();
	if(InitNode.IsGoalSatisfiedInNode())
	{
		// goal is satisfied without any actions
		return TArray<FGOAPActionWithTargetData>();
	}
	InitNode.PathToNode.Add(0);
	KnownNodes.Add(InitNode);
	CurrentNodeIndex = 0;

	ExpandNode(InitNode, KnownNodes, AvailableNodes);
	int32 VisitedNodesNum = 1;
	while(AvailableNodes.Num() > 0)
	{
		const int32 BestNodeIndex = FindBestNode(KnownNodes, AvailableNodes);
		CurrentNodeIndex = BestNodeIndex;
		if(KnownNodes[CurrentNodeIndex].IsGoalSatisfiedInNode())
			break;
		AvailableNodes.Remove(CurrentNodeIndex);
		ExpandNode(KnownNodes[CurrentNodeIndex], KnownNodes, AvailableNodes);
		
		UE_LOG(LogGOAP, Log, TEXT("Visited nodes number: %d; current cost: %d; current heuristic: %d"), VisitedNodesNum,
			KnownNodes[CurrentNodeIndex].Cost, KnownNodes[CurrentNodeIndex].Heuristic);

		++VisitedNodesNum;
	}
	UE_LOG(LogGOAP, Log, TEXT("Summary visited nodes number: %d"), VisitedNodesNum);
	
	return KnownNodes[CurrentNodeIndex].IsGoalSatisfiedInNode() ?
		BuildActionsPlanForPath(KnownNodes, KnownNodes[CurrentNodeIndex].PathToNode) : TArray<FGOAPActionWithTargetData>();
}

bool UGOAPSolver_Forward::ExpandNode(FGOAPTreeNode Node, TArray<FGOAPTreeNode>& KnownNodes, TArray<int32>& AvailableNodes)
{
	bool bAnyNodeAdded = false;
	// check all available actions on each context actor
	for(auto Action : Planner->GetActions())
	{
		for(auto ContextActor : Planner->GetAgentsMemoryComponent()->GetMemory())
		{
			FGOAPWorldStateData ActionEffect;
			if(IGOAPAction::Execute_GetActionEffectWithContextActor(Action, Planner->GetAgent(), ContextActor, ActionEffect))
			{
				// preconditions check
				TArray<FGOAPWorldStateData> ActionPreconditions = IGOAPAction::Execute_GetWorldStatePreconditions(Action, ActionEffect, Planner->GetAgent());
				bool bAllPreconditionsMet = true;
				for(auto& Precondition : ActionPreconditions)
				{
					if(!IsDesiredWorldStateSatisfiedForNode(Precondition, Node))
					{
						bAllPreconditionsMet = false;
						break;
					}
				}
				if(!bAllPreconditionsMet)
					continue;
				// action is applicable on context actor - prepare new node
				FGOAPTreeNode NewNode = Node;
				// direct action leading to this node and associated target data
				NewNode.DirectAction = Action;
				NewNode.DirectTargetData = ActionEffect;
				// current world state
				UGOAPWorldStateFunctionLibrary::AddDataToWorldStateArray(NewNode.CurrentWorldStates, ActionEffect);
				// cost
				NewNode.Cost += bUseSimplifiedActionCost ? 1 : IGOAPAction::Execute_GetActionCost(Action, ActionEffect,
					Planner->GetAgent(), NewNode.CurrentWorldStates);
				// desired states need to be iterated to check if is still valid
				for(int32 Index = NewNode.DesiredWorldStates.Num()-1; Index >= 0; --Index)
				{
					if(IsDesiredWorldStateSatisfiedForNode(NewNode.DesiredWorldStates[Index], NewNode))
					{
						NewNode.DesiredWorldStates.RemoveAt(Index);
					}
				}
				// calculate heuristic
				NewNode.Heuristic = NewNode.DesiredWorldStates.Num();
				// add to arrays and update path to node
				int32 NewNodeIndex = KnownNodes.Add(NewNode);
				KnownNodes[NewNodeIndex].PathToNode.Add(NewNodeIndex);
				AvailableNodes.Add(NewNodeIndex);
				bAnyNodeAdded = true;
			}
		}
	}
	return bAnyNodeAdded;
}

bool UGOAPSolver_Forward::IsDesiredWorldStateSatisfiedForNode(const FGOAPWorldStateData& DesiredWorldState, const FGOAPTreeNode& Node)
{
	// check if node's current world state satisfy desired world state
	for(auto& WorldState : Node.CurrentWorldStates)
	{
		if(DesiredWorldState.WorldStateKey == WorldState.WorldStateKey &&
			DesiredWorldState.WorldStateValue.Payload->IsEqual(WorldState.WorldStateValue.Payload))
		{
			return true;
		}
	}
	// check if desired state is actual world state
	return UGOAPWorldStateFunctionLibrary::IsWorldStateActual(DesiredWorldState);
}

int32 UGOAPSolver_Forward::FindBestNode(const TArray<FGOAPTreeNode>& KnownNodes, const TArray<int32>& AvailableNodes)
{
	int32 BestIndex = -1;
	int32 MinFx = MAX_int32;
	for(auto NodeIndex : AvailableNodes)
	{
		const int32 NodeFx = KnownNodes[NodeIndex].GetNodeFx();
		if(NodeFx < MinFx)
		{
			BestIndex = NodeIndex;
			MinFx = NodeFx;
		}
	}
	return BestIndex;
}

TArray<FGOAPActionWithTargetData> UGOAPSolver_Forward::BuildActionsPlanForPath(const TArray<FGOAPTreeNode>& KnownNodes,
	TArray<int32> Path)
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
