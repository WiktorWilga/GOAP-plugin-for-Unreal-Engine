// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "SGraphPin.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SWidget.h"

class SNameComboBox;

class GOAPNODES_API SGOAPWorldStatePayloadTypePin : public SGraphPin
{
	
public:
	
	SLATE_BEGIN_ARGS(SGOAPWorldStatePayloadTypePin) {}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

protected:

	virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
	void OnAttributeSelected(TSharedPtr<FName> ItemSelected, ESelectInfo::Type SelectInfo);
	void OnComboBoxOpening();

	TSharedPtr<FName> GetSelectedName() const;

	void SetPropertyWithName(const FName& Name);
	void GetPropertyAsName(FName& OutName) const;

	void InitializeAttributeList();

private:
	
	TArray<TSharedPtr<FName>> AttributesList;
	TSharedPtr<SNameComboBox> NameComboBox;
	
};