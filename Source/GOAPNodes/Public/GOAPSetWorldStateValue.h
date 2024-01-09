// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "GOAPSetWorldStateValue.generated.h"

class UGOAPWorldStatePayload;

/**
 * Allow set data in FGOAPWorldStateValue variable in BP.
 */
UCLASS(BlueprintType, Blueprintable)
class GOAPNODES_API UGOAPSetWorldStateValue : public UK2Node
{
	GENERATED_BODY()

public:

	// UEdGraphNode implementation
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual void PostReconstructNode() override;
	// UEdGraphNode implementation

	// K2Node implementation
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	// K2Node implementation

	// Helpers for getting pins
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetNewValuePin() const;
	UEdGraphPin* GetResultPin() const;

	/** Returns reference to value setter function from proper UGOAPWorldStatePayload subclass */
	static UFunction* FindSetterFunctionByType(TSubclassOf<UGOAPWorldStatePayload> PayloadClass);

};
