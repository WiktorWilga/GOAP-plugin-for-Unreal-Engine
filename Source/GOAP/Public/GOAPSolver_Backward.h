// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "GOAPSolver.h"
#include "GOAPSolver_Backward.generated.h"

/**
 * Backward planning implementation.
 */
UCLASS()
class GOAP_API UGOAPSolver_Backward : public UGOAPSolver
{
	GENERATED_BODY()

public:

	/** Return complete plan for specified goal. Can return empty array if goal can't be satisfied. */
	virtual TArray<FGOAPActionWithTargetData> FindPlanForGoal(UGOAPGoal* Goal) override;

private:

	/**
	 * Helper struct for desired world state value. Used to tracking which node produce specified desired state and
	 * which node satisfied this state.
	 */
	struct FGOAPNodeDesiredWorldState
	{
		FGOAPNodeDesiredWorldState() {};
		FGOAPNodeDesiredWorldState(const FGOAPWorldStateData& InDesiredState, const int32 InInstigatorActionIndex,
			const int32 InSatisfiedByActionIndex) : DesiredState(InDesiredState),
			InstigatorActionIndex(InInstigatorActionIndex), SatisfiedByActionIndex(InSatisfiedByActionIndex) {}

		/** Relevant data. */
		FGOAPWorldStateData DesiredState;
		/** Node index (from KnowNodes) which add this desired world state. */
		int32 InstigatorActionIndex = -1;
		/** Node index (from KnowNodes) which satisfied this desired world state. */
		int32 SatisfiedByActionIndex = -1;
	};

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

		/**
		 * Current world state in this node (contains only values which was changed by this node or parents nodes).
		 * For backward planning it is used after building decision tree, to calculate actions costs.
		 */
		TArray<FGOAPWorldStateData> CurrentWorldStates;
		/** Desired world state for this node. */
		TArray<FGOAPNodeDesiredWorldState> DesiredWorldStates;
		/** DesiredWorldStates index which this node solves. */
		int32 SolvedDesiredWorldStateIndex = -1;
		/** This node's action cost. Value calculated after build tree.*/
		int32 NodeCost = 0;

		/** Return true if this node satisfy goal (all desired states are met). */
		bool IsGoalSatisfiedInNode() const
		{
			for(auto& DesiredState : DesiredWorldStates)
			{
				if(DesiredState.SatisfiedByActionIndex == -1)
					return false;
			}
			return true;
		}
		/** Return indexes of all desired states that was added by this node's associated action. */
		TArray<int32> GetActionPreconditionsIndexes() const
		{
			TArray<int32> Result;
			for(int32 DesiredStateIndex = 0; DesiredStateIndex < DesiredWorldStates.Num(); ++DesiredStateIndex)
			{
				if(DesiredWorldStates[DesiredStateIndex].InstigatorActionIndex == PathToNode.Last())
				{
					Result.Add(DesiredStateIndex);
				}
			}
			return Result;
		}
	};
	
	/** Return all actions the use of which will lead to given world state, or nullptr if there isn't corresponding action. */
	TArray<UObject*> FindActionChangingWorldState(const FGOAPWorldStateData& DesiredWorldState) const;
	/** Find all possible actions to perform in Node, create nodes from them and add to KnownNodes and AvailableNodes. */
	bool ExpandNode(FGOAPTreeNode Node, TArray<FGOAPTreeNode>& KnownNodes, TArray<int32>& AvailableNodes,
		TArray<TArray<int32>>& Solutions) const;
	/** Return list of actions from given indexes (Path) and nodes (KnownNodes). */
	TArray<FGOAPActionWithTargetData> BuildActionsPlanForPath(const TArray<FGOAPTreeNode>& KnownNodes, TArray<int32> Path) const;
	/**
	 * It reverses actions (backward planning) and remove each action which effect is already satisfied by previous
	 * actions or actual world state.
	 */
	TArray<int32> PrepareSolution(TArray<FGOAPTreeNode>& KnownNodes, TArray<int32> Solution) const;
	/**
	 * Return true if DesiredWorldState is met in given Node (the state was satisfied by some planned actions or
	 * this state is actual state and not changed by any planned action).
	 */
	bool IsActionEffectAlreadyActualForPlan(const TArray<FGOAPWorldStateData>& CurrentWorldState,
		const FGOAPWorldStateData& ActionEffect) const;
	/** Remove identical solutions from array. */
	void RemoveIdenticalSolutions(const TArray<FGOAPTreeNode>& KnownNodes, TArray<TArray<int32>>& PreparedSolutions) const;
	/** Return true if given solutions are identical (the same actions in the same order on the same targets). */
	bool AreSolutionsIdentical(const TArray<FGOAPTreeNode>& KnownNodes, const TArray<int32>& SolutionOne,
		const TArray<int32>& SolutionTwo) const;
	/** Return index of best solution (solution which actions summary cost is min). */
	int32 GetBestSolution(const TArray<FGOAPTreeNode>& KnownNodes, const TArray<TArray<int32>>& PreparedSolutions) const;
	/**
	 * Return all indexes of nodes from solution which are used in plan to satisfy specified desired state and all
	 * derivative states.
	 * @warning: recursion - for complicated situations performance can be impacted
	 */
	TArray<int32> GetAllNodesUsedForDesiredState(const TArray<FGOAPTreeNode>& KnownNodes, TArray<int32> Solution,
		int32 StartingNodeIndex) const;
};