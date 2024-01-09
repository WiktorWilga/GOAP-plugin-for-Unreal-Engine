// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPGetWorldStateValue.h"

#include "EdGraphSchema_K2.h"
#include "KismetCompiler.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "GOAPTypes.h"
#include "GOAPWorldStatePayloads.h"
#include "K2Node_CallFunction.h"

#define LOCTEXT_NAMESPACE "GOAPGetWorldStateValue"

struct FGetPinName_GetWorldState
{

	static const FName& GetTargetPinName()
	{
		static const FName TargetPinName(TEXT("Target"));
		return TargetPinName;
	}

	static const FName& GetTypePinName()
	{
		static const FName TargetPinName(TEXT("Type"));
		return TargetPinName;
	}
	
	static const FName& GetOutputValuePinName()
	{
		static const FName OutputValuePinName(TEXT("Result"));
		return OutputValuePinName;
	}
};

void UGOAPGetWorldStateValue::AllocateDefaultPins()
{
	// Execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Input
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGOAPWorldStateValue::StaticStruct(),
		FGetPinName_GetWorldState::GetTargetPinName());
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FGetPinName_GetWorldState::GetTypePinName());
	
	// Output
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, FGetPinName_GetWorldState::GetOutputValuePinName());

	Super::AllocateDefaultPins();
}

void UGOAPGetWorldStateValue::PinDefaultValueChanged(UEdGraphPin * Pin)
{
	if (Pin)
	{
		if (Pin->PinName == FGetPinName_GetWorldState::GetTypePinName())
		{
			CoerceTypeFromPin();
		}
	}
}

/// These three functions make the node appear as an option;
FText UGOAPGetWorldStateValue::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GOAPGetWorldStateValue_Title", "Get World State Value");
}

FText UGOAPGetWorldStateValue::GetTooltipText() const
{
	return LOCTEXT("GOAPGetWorldStateValue_Tooltip", "Get world state value from given struct.");
}

FText UGOAPGetWorldStateValue::GetMenuCategory() const
{
	return LOCTEXT("GOAPGetWorldStateValue_MenuCategory", "GOAP");
}
/// end

void UGOAPGetWorldStateValue::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphPin* TypePin = GetTypePin();
	UClass* InputClass =  FindObject<UClass>(ANY_PACKAGE, *TypePin->DefaultValue);
	
	const UFunction* BlueprintFunction = FindSetterFunctionByType(InputClass);
	
	if (!BlueprintFunction)
	{
		CompilerContext.MessageLog.Error(*LOCTEXT("InvalidFunctionName", "The function has not been found.").ToString(), this);
		return;
	}
	
	UK2Node_CallFunction* CallFunction = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	
	CallFunction->SetFromFunction(BlueprintFunction);
	CallFunction->AllocateDefaultPins();
	CompilerContext.MessageLog.NotifyIntermediateObjectCreation(CallFunction, this);
	
	// Input
	CompilerContext.MovePinLinksToIntermediate(*FindPin(FGetPinName_GetWorldState::GetTargetPinName()),
		*CallFunction->FindPinChecked(TEXT("WorldStateValue")));
	
	// Output
	CompilerContext.MovePinLinksToIntermediate(*FindPin(FGetPinName_GetWorldState::GetOutputValuePinName()),
		*CallFunction->GetReturnValuePin());
	
	// Exec pins
	UEdGraphPin* NodeExec = GetExecPin();
	UEdGraphPin* NodeThen = FindPin(UEdGraphSchema_K2::PN_Then);
	
	UEdGraphPin* InternalExec = CallFunction->GetExecPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeExec, *InternalExec);
	
	UEdGraphPin* InternalThen = CallFunction->GetThenPin();
	CompilerContext.MovePinLinksToIntermediate(*NodeThen, *InternalThen);

	// After we are done we break all links to this node (not the internally created one)
	BreakAllNodeLinks();
}

// This method adds our node to the context menu
void UGOAPGetWorldStateValue::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);

	const UClass* Action = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		check(Spawner != nullptr);

		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

void UGOAPGetWorldStateValue::NotifyPinConnectionListChanged(UEdGraphPin * Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	
	if(Pin == GetTypePin())
	{
		CoerceTypeFromPin();
	}
}

/// Pin getters
UEdGraphPin* UGOAPGetWorldStateValue::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UGOAPGetWorldStateValue::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_GetWorldState::GetTargetPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UGOAPGetWorldStateValue::GetTypePin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_GetWorldState::GetTypePinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UGOAPGetWorldStateValue::GetReturnValuePin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_GetWorldState::GetOutputValuePinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

/// Node type handling
void UGOAPGetWorldStateValue::CoerceTypeFromPin()
{
	UEdGraphPin* OutValuePin = GetReturnValuePin();
	const UEdGraphPin* TypePin = GetTypePin();
	const UClass* InputClass =  FindObject<UClass>(ANY_PACKAGE, *TypePin->DefaultValue);
	
	if (InputClass)
	{
		const FProperty* ValueFProperty = FindFProperty<FProperty>(InputClass, "Value");
		FName PinCategory, PinSubCategory;
		UObject* PinSubCategoryObject;
		bool PinIsWeakPtr;
		UEdGraphSchema_K2::GetPropertyCategoryInfo(ValueFProperty, PinCategory,
			PinSubCategory, PinSubCategoryObject, PinIsWeakPtr);
		
		OutValuePin->PinType.PinCategory = PinCategory;
		OutValuePin->PinType.PinSubCategory = PinSubCategory;
		OutValuePin->PinType.PinSubCategoryObject = nullptr;
		if(PinSubCategoryObject && (PinCategory == "struct" || PinCategory == "object" || PinCategory == "byte"))
		{
			OutValuePin->PinType.PinSubCategoryObject = PinSubCategoryObject;
		}
		OutValuePin->PinType.bIsReference = false;
	}
	else
	{
		OutValuePin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
		OutValuePin->PinType.PinSubCategory = NAME_None;
		OutValuePin->PinType.PinSubCategoryObject = nullptr;
		OutValuePin->BreakAllPinLinks();
	}
}

// Find setter
UFunction* UGOAPGetWorldStateValue::FindSetterFunctionByType(TSubclassOf<UGOAPWorldStatePayload> PayloadClass)
{
	if(!PayloadClass)
	{
		return nullptr;
	}

	return PayloadClass->FindFunctionByName("GetPayloadValue");
}

void UGOAPGetWorldStateValue::ReconstructNode()
{
	Super::ReconstructNode();

	CoerceTypeFromPin();
}

#undef LOCTEXT_NAMESPACE
