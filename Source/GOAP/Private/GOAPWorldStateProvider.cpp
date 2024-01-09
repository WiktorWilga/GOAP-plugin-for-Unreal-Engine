// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPWorldStateProvider.h"

UGOAPWorldStateProvider::UGOAPWorldStateProvider()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGOAPWorldStateProvider::BeginPlay()
{
	Super::BeginPlay();

	// create atoms objects
	for(auto AtomClass : WorldSateAtomsClasses)
	{
		if(!IsValid(AtomClass))
			continue;

		UGOAPWorldStateAtom* NewAtom = NewObject<UGOAPWorldStateAtom>(this, AtomClass);
		if(!NewAtom)
		{
			UE_LOG(LogGOAP, Error, TEXT("Cant create World State Atom of class %s!"), *AtomClass->GetName());
			continue;
		}

		NewAtom->OwnerActor = GetOwner();
		WorldSateAtoms.Add(NewAtom);
		NewAtom->UpdateWorldStateAtomData();
	}
}

bool UGOAPWorldStateProvider::HasWorldStateValue(const FGameplayTag WorldStateAtomTag)
{
	return WorldSateAtoms.ContainsByPredicate([WorldStateAtomTag](const UGOAPWorldStateAtom* Atom)
	{
		return Atom->WorldStateAtomTag.MatchesTagExact(WorldStateAtomTag);
	});
}

FGOAPWorldStateValue UGOAPWorldStateProvider::GetWorldStateValue(const FGameplayTag WorldStateAtomTag)
{
	for(const auto Atom : WorldSateAtoms)
	{
		if(Atom->WorldStateAtomTag.MatchesTagExact(WorldStateAtomTag))
		{
			Atom->UpdateWorldStateAtomData();
			return Atom->WorldStateValue;
		}
	}
	return FGOAPWorldStateValue();
}