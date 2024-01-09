// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPGoal.h"

FGOAPWorldStateData UGOAPGoal::GetDesiredWorldState()
{
	// update desired world state to always return current data
	UpdateDesiredWorldState();
	
	return DesiredWorldState;
}

void UGOAPGoal::UpdateDesiredWorldState_Implementation()
{
	// have to be implemented
	//unimplemented();
}
