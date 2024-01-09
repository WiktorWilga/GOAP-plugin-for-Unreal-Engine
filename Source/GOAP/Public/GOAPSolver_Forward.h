// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPSolver.h"
#include "GOAPSolver_Forward.generated.h"

/**
 * Forward planning implementation.
 */
UCLASS()
class GOAP_API UGOAPSolver_Forward : public UGOAPSolver
{
	GENERATED_BODY()
	
public:

	/** Return complete plan for specified goal. Can return empty array if goal can't be satisfied. */
	virtual TArray<FGOAPActionWithTargetData> FindPlanForGoal(UGOAPGoal* Goal) override;

private:

	/**
	 * If true solver will be use cost = 1 for each action. Action's original cost is ignored, but thanks that
	 * forward planning is more efficient.
	 */
	bool bUseSimplifiedActionCost = true;

	/**
	 * Helper struct representing node of decision tree.
	 */
	struct FGOAPTreeNode
	{
		/** Indexes leading to this node (parents). */
		TArray<int32> PathToNode;
		/** Action immediately preceding this node (action to be performed to get to this node from the parent). */
		UObject* DirectAction = nullptr;
		/** Target data for DirectAction. */
		FGOAPWorldStateData DirectTargetData;

		/** Current world state in this node (contains only values which was changed by this node or parents nodes). */
		TArray<FGOAPWorldStateData> CurrentWorldStates;
		/** Desired world state for this node. */
		TArray<FGOAPWorldStateData> DesiredWorldStates;
		/** Total cost of the need to reach this node from init node. */
		int32 Cost = 0;
		/** Heuristic (for A*) which is equal to number of desired states. */
		int32 Heuristic = 0;

		/** Return true if this node satisfy goal (all desired states are met). */
		bool IsGoalSatisfiedInNode() const { return Heuristic == 0; }
		/** Return f(x) value for A* (f(x)=g(x)+h(x)). */
		int32 GetNodeFx() const { return Cost + Heuristic; }
	};

	/** Find all possible actions to perform in Node, create nodes from them and add to KnownNodes and AvailableNodes. */
	bool ExpandNode(FGOAPTreeNode Node, TArray<FGOAPTreeNode>& KnownNodes, TArray<int32>& AvailableNodes);
	/**
	 * Return true if DesiredWorldState is met in given Node (the state was satisfied by some planned actions or
	 * this state is actual state and not changed by any planned action).
	 */
	bool IsDesiredWorldStateSatisfiedForNode(const FGOAPWorldStateData& DesiredWorldState, const FGOAPTreeNode& Node);
	/** Return index of best node (node of min value of A* f(x)=g(x)+h(x)). */
	int32 FindBestNode(const TArray<FGOAPTreeNode>& KnownNodes, const TArray<int32>& AvailableNodes);
	/** Return list of actions from given indexes (Path) and nodes (KnownNodes). */
	TArray<FGOAPActionWithTargetData> BuildActionsPlanForPath(const TArray<FGOAPTreeNode>& KnownNodes, TArray<int32> Path);
};
