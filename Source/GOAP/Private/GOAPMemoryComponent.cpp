// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPMemoryComponent.h"

void UGOAPMemoryComponent::RegisterActorInMemory(AActor* Actor)
{
	if(!Actor)
		return;

	Memory.AddUnique(Actor);
	OnMemoryChangedDelegate.Broadcast(Actor, true);
}

void UGOAPMemoryComponent::UnregisterActorFromMemory(AActor* Actor)
{
	Memory.Remove(Actor);
	OnMemoryChangedDelegate.Broadcast(Actor, false);
}

bool UGOAPMemoryComponent::IsActorInMemory(AActor* Actor) const
{
	return Memory.Contains(Actor);
}

