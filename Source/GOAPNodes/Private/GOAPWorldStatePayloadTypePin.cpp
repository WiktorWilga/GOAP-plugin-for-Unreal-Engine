// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#include "GOAPWorldStatePayloadTypePin.h"

#include "GOAPWorldStatePayloads.h"
#include "EdGraph/EdGraphPin.h"
#include "EdGraph/EdGraphSchema.h"
#include "SNameComboBox.h"
#include "ScopedTransaction.h"

void SGOAPWorldStatePayloadTypePin::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
    SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

void SGOAPWorldStatePayloadTypePin::InitializeAttributeList()
{
    TArray<UClass*> PayloadsTypes;
    GetDerivedClasses(UGOAPWorldStatePayload::StaticClass(), PayloadsTypes);
    for(auto Type : PayloadsTypes)
    {
        TSharedPtr<FName> TypeName;
        TypeName = MakeShared<FName>(Type->GetFName());
        AttributesList.Add(TypeName);
    }
}

TSharedRef<SWidget> SGOAPWorldStatePayloadTypePin::GetDefaultValueWidget()
{
    InitializeAttributeList();

    // retrieve the previous value selected (or the first value as default)
    TSharedPtr<FName> InitialSelectedName = GetSelectedName();
    if (InitialSelectedName.IsValid())
    {
        SetPropertyWithName(*InitialSelectedName.Get());
    }

    return SAssignNew(NameComboBox, SNameComboBox)
        .ContentPadding(FMargin(6.0f, 2.0f))
        .OptionsSource(&AttributesList)
        .InitiallySelectedItem(InitialSelectedName)
        .OnComboBoxOpening(this, &SGOAPWorldStatePayloadTypePin::OnComboBoxOpening)
        .OnSelectionChanged(this, &SGOAPWorldStatePayloadTypePin::OnAttributeSelected);
}
void SGOAPWorldStatePayloadTypePin::OnAttributeSelected(TSharedPtr<FName> ItemSelected, ESelectInfo::Type SelectInfo)
{
    if (ItemSelected.IsValid())
    {
        SetPropertyWithName(*ItemSelected);
    }
}

void SGOAPWorldStatePayloadTypePin::OnComboBoxOpening()
{
    TSharedPtr<FName> SelectedName = GetSelectedName();
    if (SelectedName.IsValid())
    {
        check(NameComboBox.IsValid());
        NameComboBox->SetSelectedItem(SelectedName);
    }
}

void SGOAPWorldStatePayloadTypePin::SetPropertyWithName(const FName& Name)
{
    check(GraphPinObj);

    const FString PinString = *Name.ToString();
    const FString CurrentDefaultValue = GraphPinObj->GetDefaultAsString();

    if (CurrentDefaultValue != PinString)
    {
        const FScopedTransaction Transaction(
            NSLOCTEXT("GraphEditor", "ChangeListNameFromConfigPinValue", "Change List Name From Config Value"));
        GraphPinObj->Modify();

        if (PinString != GraphPinObj->GetDefaultAsString())
        {
            GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, PinString);
        }
    }
}

TSharedPtr<FName> SGOAPWorldStatePayloadTypePin::GetSelectedName() const
{
    const int32 NameCount = AttributesList.Num();
    if (NameCount <= 0)
    {
        return nullptr;
    }

    FName Name;
    GetPropertyAsName(Name);

    for (int32 NameIndex = 0; NameIndex < NameCount; ++NameIndex)
    {
        if (Name == *AttributesList[NameIndex].Get())
        {
            return AttributesList[NameIndex];
        }
    }
    // no value has been found, return a default value
    return AttributesList[0];
}

void SGOAPWorldStatePayloadTypePin::GetPropertyAsName(FName& OutName) const
{
    check(GraphPinObj);

    FString PinString = GraphPinObj->GetDefaultAsString();

    if (PinString.StartsWith(TEXT("(")) && PinString.EndsWith(TEXT(")")))
    {
        PinString = PinString.LeftChop(1);
        PinString = PinString.RightChop(1);
        PinString.Split("=", nullptr, &PinString);

        if (PinString.StartsWith(TEXT("\"")) && PinString.EndsWith(TEXT("\"")))
        {
            PinString = PinString.LeftChop(1);
            PinString = PinString.RightChop(1);
        }
    }

    if (!PinString.IsEmpty())
    {
        OutName = *PinString;
    }
}
