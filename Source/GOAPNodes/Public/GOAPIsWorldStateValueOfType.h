// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "GOAPIsWorldStateValueOfType.generated.h"

class UGOAPWorldStatePayload;

/**
 * Allow check if given FGOAPWorldStateValue is of given type.
 */
UCLASS(BlueprintType, Blueprintable)
class GOAPNODES_API UGOAPIsWorldStateValueOfType : public UK2Node
{
	GENERATED_BODY()

public:

	// UEdGraphNode implementation
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// UEdGraphNode implementation

	// K2Node implementation
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	// K2Node implementation

	// Helpers for getting pins
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetTypePin() const;
	UEdGraphPin* GetReturnValuePin() const;

	/** Returns reference to type checker function from proper UGOAPWorldStatePayload subclass */
	static UFunction* FindSetterFunctionByType(TSubclassOf<UGOAPWorldStatePayload> PayloadClass);

};
