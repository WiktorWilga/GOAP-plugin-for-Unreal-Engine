// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#include "GOAPNodes.h"
#include "GOAPPayloadTypePinFactory.h"

#define LOCTEXT_NAMESPACE "FGOAPNodesModule"

void FGOAPNodesModule::StartupModule()
{
	// create and register pin factory
	TSharedPtr<FGOAPPayloadTypePinFactory> Factory = MakeShareable(new FGOAPPayloadTypePinFactory());
	FEdGraphUtilities::RegisterVisualPinFactory(Factory);
}

void FGOAPNodesModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGOAPNodesModule, GOAPNodes)