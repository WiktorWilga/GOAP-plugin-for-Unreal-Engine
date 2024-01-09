// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "EdGraphSchema_K2.h"
#include "EdGraphUtilities.h"
#include "GOAPGetWorldStateValue.h"
#include "GOAPIsWorldStateValueOfType.h"
#include "GOAPWorldStatePayloadTypePin.h"
#include "SlateBasics.h"

class GOAPNODES_API FGOAPPayloadTypePinFactory : public FGraphPanelPinFactory
{
	virtual TSharedPtr<class SGraphPin> CreatePin(class UEdGraphPin* InPin) const override
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		UObject* Outer = InPin->GetOuter();
		const UEdGraphPin* PayloadTypePin = nullptr;
		
		if (InPin->PinType.PinCategory == K2Schema->PC_Name)
		{
			if (Outer->IsA(UGOAPGetWorldStateValue::StaticClass()))
			{
				const UGOAPGetWorldStateValue* Node = CastChecked<UGOAPGetWorldStateValue>(Outer);
				PayloadTypePin = Node->GetTypePin();
			}
			else if (Outer->IsA(UGOAPIsWorldStateValueOfType::StaticClass()))
			{
				const UGOAPIsWorldStateValueOfType* Node = CastChecked<UGOAPIsWorldStateValueOfType>(Outer);
				PayloadTypePin = Node->GetTypePin();
			}
		}

		if(PayloadTypePin)
		{
			return SNew(SGOAPWorldStatePayloadTypePin, InPin);
		}
		
		return nullptr;
	}
};