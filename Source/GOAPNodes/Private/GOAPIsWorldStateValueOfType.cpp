// Copyright Wiktor Wilga (wilgawiktor@gmail.com)


#include "GOAPIsWorldStateValueOfType.h"

#include "EdGraphSchema_K2.h"
#include "KismetCompiler.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "GOAPTypes.h"
#include "GOAPWorldStatePayloads.h"
#include "K2Node_CallFunction.h"
#include "UObject/UnrealTypePrivate.h"

#define LOCTEXT_NAMESPACE "GOAPIsWorldStateValueOfType"

struct FGetPinName_IsWorldStateOfType
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

void UGOAPIsWorldStateValueOfType::AllocateDefaultPins()
{
	// Execution pins
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);

	// Input
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, FGOAPWorldStateValue::StaticStruct(),
		FGetPinName_IsWorldStateOfType::GetTargetPinName());
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, FGetPinName_IsWorldStateOfType::GetTypePinName());
	
	// Output
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, FGetPinName_IsWorldStateOfType::GetOutputValuePinName());

	Super::AllocateDefaultPins();
}

/// These three functions make the node appear as an option;
FText UGOAPIsWorldStateValueOfType::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("GOAIsWorldStateValueOfType_Title", "Is World State Value Of Type");
}

FText UGOAPIsWorldStateValueOfType::GetTooltipText() const
{
	return LOCTEXT("GOAPIsWorldStateValueOfType_Tooltip", "Checks if given world state value is of given type.");
}

FText UGOAPIsWorldStateValueOfType::GetMenuCategory() const
{
	return LOCTEXT("GOAPIsWorldStateValueOfType_MenuCategory", "GOAP");
}
/// end

void UGOAPIsWorldStateValueOfType::ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
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
	CompilerContext.MovePinLinksToIntermediate(*FindPin(FGetPinName_IsWorldStateOfType::GetTargetPinName()), *CallFunction->FindPinChecked(TEXT("WorldStateValue")));
	CompilerContext.MovePinLinksToIntermediate(*FindPin(FGetPinName_IsWorldStateOfType::GetTypePinName()), *CallFunction->FindPinChecked(TEXT("TypeName")));
	
	// Output
	CompilerContext.MovePinLinksToIntermediate(*FindPin(FGetPinName_IsWorldStateOfType::GetOutputValuePinName()), *CallFunction->GetReturnValuePin());
	
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
void UGOAPIsWorldStateValueOfType::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
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

/// Pin getters
UEdGraphPin* UGOAPIsWorldStateValueOfType::GetThenPin() const
{
	UEdGraphPin* Pin = FindPinChecked(UEdGraphSchema_K2::PN_Then);
	check(Pin->Direction == EGPD_Output);
	return Pin;
}

UEdGraphPin* UGOAPIsWorldStateValueOfType::GetTargetPin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_IsWorldStateOfType::GetTargetPinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UGOAPIsWorldStateValueOfType::GetTypePin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_IsWorldStateOfType::GetTypePinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Input);
	return Pin;
}

UEdGraphPin* UGOAPIsWorldStateValueOfType::GetReturnValuePin() const
{
	UEdGraphPin* Pin = FindPin(FGetPinName_IsWorldStateOfType::GetOutputValuePinName());
	check(Pin == nullptr || Pin->Direction == EGPD_Output);
	return Pin;
}

// Find setter
UFunction* UGOAPIsWorldStateValueOfType::FindSetterFunctionByType(TSubclassOf<UGOAPWorldStatePayload> PayloadClass)
{
	if(!PayloadClass)
	{
		return nullptr;
	}

	return PayloadClass->FindFunctionByName("IsWorldStateValueOfThisType");
}

#undef LOCTEXT_NAMESPACE
