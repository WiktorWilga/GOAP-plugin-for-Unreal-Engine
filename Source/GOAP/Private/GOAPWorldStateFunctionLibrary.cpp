// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPWorldStateFunctionLibrary.h"

#include "GOAPWorldStatePayloads.h"
#include "GOAPWorldStateProvider.h"


void UGOAPWorldStateFunctionLibrary::AddDataToWorldStateArray(TArray<FGOAPWorldStateData>& WorldStateArray,
	const FGOAPWorldStateData& NewData)
{
	bool bWorldStateFound = false;
	for(auto& WorldState : WorldStateArray)
	{
		// if we already have value for this key - update it
		if(WorldState.WorldStateKey == NewData.WorldStateKey)
		{
			WorldState.WorldStateValue = NewData.WorldStateValue;
			bWorldStateFound = true;
		}
	}
	// new key
	if(!bWorldStateFound)
	{
		WorldStateArray.Add(NewData);
	}
}

bool UGOAPWorldStateFunctionLibrary::IsWorldStateActual(const FGOAPWorldStateData& DesiredWorldState)
{
	const AActor* WorldStateActor = DesiredWorldState.WorldStateKey.WorldStateActor;
	if(!WorldStateActor)
		return false;
	
	UGOAPWorldStateProvider* WorldStateProvider =
		Cast<UGOAPWorldStateProvider>(WorldStateActor->GetComponentByClass(UGOAPWorldStateProvider::StaticClass()));
	if(!WorldStateProvider)
		return false;

	const FGOAPWorldStateValue CurrentWorldState =
		WorldStateProvider->GetWorldStateValue(DesiredWorldState.WorldStateKey.WorldStateDataTag);

	return DesiredWorldState.WorldStateValue.Payload->IsEqual(CurrentWorldState.Payload);
}

FGOAPWorldStateValue UGOAPWorldStateFunctionLibrary::GetActualWorldStateValue(const FGOAPWorldStateKey& Key,
                                                                          const TArray<FGOAPWorldStateData>& WithCurrentWorldState)
{
	if(Key.WorldStateActor == nullptr || !Key.WorldStateDataTag.IsValid())
		return FGOAPWorldStateValue();
	
	for(auto CurrentWorldState : WithCurrentWorldState)
	{
		if(CurrentWorldState.WorldStateKey == Key)
		{
			return CurrentWorldState.WorldStateValue;
		}
	}

	const auto WorldStateProvider = Cast<UGOAPWorldStateProvider>(Key.WorldStateActor->GetComponentByClass(UGOAPWorldStateProvider::StaticClass()));
	if(!WorldStateProvider)
		return FGOAPWorldStateValue();
	
	if(!WorldStateProvider->HasWorldStateValue(Key.WorldStateDataTag))
		return FGOAPWorldStateValue();

	return  WorldStateProvider->GetWorldStateValue(Key.WorldStateDataTag);
}