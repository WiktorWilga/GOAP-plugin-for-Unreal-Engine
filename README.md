# Goal Oriented Action Planning Plugin for Unreal Engine
This is a convenient-to-use and easy-to-expand plugin with which you can create advanced behaviors for NPCs in your game based on goal-oriented planning. You don't need to know C++ - all logic can be embedded in Blueprints. The plugin has been prepared in such a way that extending its functionality is as simple as possible. For this purpose, for example, custom nodes have been prepared, thanks to which adding a new data type to the world state (what is a world state - see [What is GOAP?](#what-is-goap) section) is limited only to creating the appropriate class, and it will automatically appear in the list of available types in nodes.
>gif GetWorldStateValue

>gif SetWorldStateValue

## Table of Contents
- [What is GOAP?](#what-is-goap)
- [Plugin structure](#plugin-structure)
- [World state](#world-state)
- - [Payloads](#payloads)
- - [World state data provider](#world-state-data-provider)
- - [World state atoms](#world-state-atoms)
- - [Memory component](#memory-component)
- [Goals](#goals)
- [Actions](#actions)
- [GOAP planner](#goap-planner)
- - [Solver](#solver)
- [Agent](#agent)
- [Actions executor](#actions-executor)
- [GOAP log](#goap-log)

## What is GOAP?
GOAP is a system for planning agent actions. It presents a completely different approach to controlling NPCs than the default behavioral trees available in the Unreal Engine. Both solutions have their advantages and disadvantages, so before choosing one of them, it is important to understand the principle of operation and choose a solution that will work better in a given situation.
GOAP uses two basic elements to operate, which are goals and actions. It makes decisions based on the information it has, called the world state, which should be the current representation of all planning-relevant data, and represented, for example, as an array of values. Goals define what a character will want to achieve, but in itself does not define how to achieve it, they are only the desired world state. Their achievement is defined as bringing the current state to the one indicated by the goal. An action, i.e. the possibility for AI units to take a certain action, defines what results its execution brings, or more precisely, how its activation will change the world state. The conditions that determine whether it is possible to take a particular action must also be represented by the world state. Thanks to this simplification - treating actions as objects that require a certain world state and change the state accordingly, while goals as a set of states that we want to achieve - GOAP is able to accomplish goals by selecting appropriate actions, building more complex sequences out of them. The selection of goals itself is done on the basis of a value (score) that determines how important a given goal is to the character controlled by the computer. The goal with the highest score value is always selected.
An important fact is that all goals and actions are independent of each other, which means that the implementation of further objects of this type does not require any changes to the others, and as a result, maintaining the project in terms of artificial intelligence becomes much easier. The created goal or action only needs to be assigned to specific character. GOAP itself is already responsible for deciding whether and when to use a given action or complete a given goal.
>image GOAP

## Plugin structure
The most important classes you will use when using the plugin are IGOAPAgent, UGOAPPlanner, UGOAPWorldStateProvider, UGOAPMemoryComponent, UGOAPGoal, IGOAPAction, UGOAPWorldStateAtom. The diagram below shows the dependencies between them.
>image PluginArchitecture

## World state
The world state entry is represented by the FGOAPWorldStateData structure. It stores a single value for a specific object in the environment. FGOAPWorldStateData consists of FGOAPWorldStateKey (storing info of the actor it refers to and its value in the form of a tag) and FGOAPWorldStateValue (storing the actual value).
>image GOAPStructs

The value of FGOAPWorldStateValue can be set using the SetWorldStateValue node:
>image SetWSV

The GetWorldStateValue node is used to check the current value of FGOAPWorldStateValue:
>image GetWSV

The type stored by FGOAPWorldStateValue can be checked with node:
>image IsWSVOT

### Payloads
World sate value types are closely related to the UGOAPWorldStatePayload class. Plugin defines basic types such as bool, int, float, actor, vector etc, which should be sufficient in most cases, but new types can be added if needed by creating a class inheriting from UGOAPWorldStatePayload.
The following psueudocode shows how to create a new payload named GOAPWorldStatePayloadTYPE, which stores the TYPE type:

````c++
UCLASS()
class UGOAPWorldStatePayloadTYPE : public UGOAPWorldStatePayload
{
GENERATED_BODY()

public:

	UPROPERTY()
	TYPE Value;

	virtual void SetPayloadValueFromRawData(void* DataValueData) override
	{
		Value = *(static_cast<TYPE*>(DataValueData));
	}
	virtual bool IsEqual(UGOAPWorldStatePayloadTYPE* OtherPayload) override
	{
		const UGOAPWorldStatePayloadTYPE* Other = Cast<UGOAPWorldStatePayloadTYPE>(OtherPayload);
		if(!Other)
			return false;
		return Value == Other->Value;
	}
	
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static TYPE GetPayloadValue(FGOAPWorldStateValue WorldStateValue)
	{
		return Cast<UGOAPWorldStatePayloadTYPE>(WorldStateValue.Payload)->Value;
	}
	UFUNCTION(BlueprintCallable, BlueprintInternalUseOnly)
	static FGOAPWorldStateValue SetPayloadValue(UPARAM(ref) FGOAPWorldStateValue& WorldStateValue, bool InValue)
	{
		return UGOAPWorldStateFunctionLibrary::SetPayload(WorldStateValue, StaticClass(), InValue);
	}
};
````

The new type will be automatically added to the list in the node in Blueprints.

### World state data provider
Each actor to be considered for planning proccess must have a UGOAPWorldStateProvider component. This applies to the agents themselves as well as the objects they interact with. The UGOAPWorldStateProvider can be used to specify what data is made available to the world state for this actor (via the UGOAPWorldStateAtom list).
>image AtomsList

Remember that if you do not assign a specific atom to a provider, then from the GOAP perspective he does not have this feature. So if an enemy character does not have an atom representing his health, it will automatically not be possible to plan to attack him.
You can check the world state through the following functions:

bool HasWorldStateValue(const FGameplayTag WorldStateAtomTag) - returns true if provider has specified data (has specified world state atom).

FGOAPWorldStateValue GetWorldStateValue(const FGameplayTag WorldStateAtomTag) - allows get specified world state value.

### World state atoms
UGOAPWorldStateAtom corresponds to one parameter of an actor. E.g. one atom may correspond to a character's location, another to its health state, etc. The atom checks this state and returns the current value in the form of FGOAPWorldStateValue on each query. To create an atom create a class derived from UGOAPWorldStateAtom. Set the appropriate tag representing the data type and implement the UpdateWorldStateAtomData() function.
>image AtomImplementation

### Memory component
Every agent must have this component. In it are recorded all the actors that the agent "knows" about. The agent can take into account in planning and perform actions only on actors that are in his memory, e.g. to attack an enemy character the agent must have this character registered in this component.

void RegisterActorInMemory(AActor* Actor) - adds new data (actor must to have UGOAPWorldStateProvider) to memory. You can e.g. assign this function to Unreals perception system, so each actor noticed by agent will be added to memory.

void UnregisterActorFromMemory(AActor* Actor) - removes data from memory.

TArray<AActor*> GetMemory() const - returns all current memory data.

bool IsActorInMemory(AActor* Actor) - returns true if has specified actor registered in memory.

FMemoryChangedDelegate OnMemoryChangedDelegate - called every time when any actor is registered in memory or registered from memory.

## Goals
Defines what AI want to achieve and how important it is. Represented by the UGOAPGoal class. To create a new goal, add a class inheriting from UGOAPGoal and implement 3 functions in it:

void UpdateDesiredWorldState() - calling this function should refresh the value of the DesiredWorldState variable so that it has current data.

bool IsGoalValid() - should returns true if goal is valid in this moment and can be considered in planning.

float GetGoalScore() - should returns value in range <0,1>. 0 - goal isn't important. 1 - goal is very important.

In goal you have access to agent by variable AgentActor.

## Actions
In order for an agent to plan anything and then execute that plan it needs actions, which it will sequence to fulfill a specific goal. Actions can be any UObject (especially GameplayAbility) implementing the IGOAPAction interface. It is necessary to override the following functions:

bool CanChangeWorldState(FGOAPWorldStateData DesiredWorldState, AActor* AgentActor) – indicates whether the action is able to produce a certain result (DesiredWorldState) on a given agent (AgentActor).

TArray<FGOAPWorldStateData> GetWorldStatePreconditions(FGOAPWorldStateData ForDesiredWorldState, AActor* AgentActor) – returns a list of preconditions that must be met for the action to be activated.

void SetActionTargetData(FGOAPWorldStateData TargetData) – allows you to provide input to the action, this data will be used during the execution of the action, the planner can provide the context of the action through this value.

int32 GetActionCost(FGOAPWorldStateData DesiredWorldState, AActor* AgentActor, const TArray<FGOAPWorldStateData>& WithCurrentWorldState) – returns an integer value that is the cost of performing an action that will produce a specified result (DesiredWorldState) by a specified agent (AgentActor) under specified world state conditions (WithCurrentWorldState). The higher the cost, the less favorable it is to take the action.

bool GetActionEffectWithContextActor(AActor* AgentActor, AActor* TargetActor, FGOAPWorldStateData& EffectWorldState) – should returns true if action can be applicable on specified TargetActor and give a specific effect (EffectWorldState).

bool CanBeCanceled() - checks whether it is possible to cancel the execution of an action at that moment, e.g. it is possible that in your game a character's movement can be canceled at any time, but an attack can no longer be canceled after the execution of an attack.

## GOAP planner
The UGOAPPlanner component is a must for every agent. It is the heart of the system. It decides what goal to achieve, based on the available actions in the current conditions of the world state builds a plan, which is then passed on for execution (that is, to the Actions executor).
After adding this component to the agent, it is still necessary to fill in the list of goals that the agent will be able to pursue (GoalsClasses):
>image GoalsList

The planner set up this way still needs to be initialized on BeginPlay through the following function

void InitializePlanner(UGOAPMemoryComponent* InMemoryComponent, UObject* InActionExecutor, TArray<UObject*> InActions) where we successively specify agent's MemoryComponent, the object responsible for executing the action (see [Actions executor](#actions-executor)), the list of actions available for the agent

By default, the planner checks the validity of the selected goal and, if necessary, changes the goal and sets a new plan every 0.5s. You can change this value through the TickInterval variable.

The planner has several public functions so that you can dynamically change the available goals for an agent and see what goal is currently being pursued:

void AddGoal(TSubclassOf<UGOAPGoal> GoalClass)

void RemoveGoal(TSubclassOf<UGOAPGoal> GoalClass)

UGOAPGoal* GetPursuedGoal() const

### Solver
Solver is an object that directly implements planning. By default there are two algorithms available: backward planning (recommended) and forward planning (which is much less efficient, in the plugin it is only included as an example to show a different version of the algorithm). Both algorithms give the same results (they return the same plan for the same situation), but planning backwards is much more efficient, so this is the one you should use. If necessary, you can add your version of the algorithm inheriting from UGOAPSolver based on the code of the UGOAPSolver_Backward and UGOAPSolver_Forward classes.
You can change the planning algorithm in the planner through the SolverImplementationClass variable:
>image SolversList

## Agent
Each character to be controlled by AI must implement the IGOAPAgent interface, with two functions in it:

GOAPPlanner* GetGOAPPlanner() - should returns agent's planner component.

UGOAPMemoryComponent* GetGOAPMemoryComponent() - should returns agent's memory component.

## Actions executor
The result of the planner is a plan (a sequence of actions with the necessary data to execute them). The execution of the action itself rests on an agent object implementing the IGOAPActionsExecutor interface (if you are using GAS then GameplayAbilityComponent is a great candidate for this). This object must call OnActionEnded after each action is completed, and implement the following functions:

void CancelAction(UObject* Action) - should stops performing given action (if that action is currently being performed).

UObject* GetActiveAction() - should returns action that is currently being performed.

bool TryActivateActionByClass(TSubclassOf<UObject> ActionClass) - should activates action if possible.

## GOAP log
The work of the planner can be previewed in the logs (GOAPLog category). Information about setting a new target, founded plans, their scores, etc. is written out there:
>image GOAPLog