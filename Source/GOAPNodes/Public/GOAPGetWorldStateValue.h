// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "GOAPGetWorldStateValue.generated.h"

class UGOAPWorldStatePayload;

class UBHWorldStatePayload;
/**
 * Allow get data from FGOAPWorldStateValue variable in BP.
 */
UCLASS(BlueprintType, Blueprintable)
class GOAPNODES_API UGOAPGetWorldStateValue : public UK2Node
{
	GENERATED_BODY()

public:

	// UEdGraphNode implementation
	virtual void AllocateDefaultPins() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	// UEdGraphNode implementation

	// K2Node implementation
	virtual FText GetMenuCategory() const override;
	virtual void ExpandNode(class FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void ReconstructNode() override;
	// K2Node implementation

	// Helpers for getting pins
	UEdGraphPin* GetThenPin() const;
	UEdGraphPin* GetTargetPin() const;
	UEdGraphPin* GetTypePin() const;
	UEdGraphPin* GetReturnValuePin() const;
	
	/** Sets proper result pin type depending on input value */
	void CoerceTypeFromPin();

	/** Returns reference to value getter function from proper UGOAPWorldStatePayload subclass */
	static UFunction* FindSetterFunctionByType(TSubclassOf<UGOAPWorldStatePayload> PayloadClass);

};
