// Copyright Wiktor Wilga (wilgawiktor@gmail.com)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GOAPMemoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FMemoryChangedDelegate, AActor*, Actor, bool, bRegistered);

/**
 * This component is just list of known actors by component's owner actor.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GOAP_API UGOAPMemoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	/** Called every time when any actor is registered in memory or registered from memory. */
	UPROPERTY(BlueprintAssignable)
	FMemoryChangedDelegate OnMemoryChangedDelegate;

	/** Add new data (actor providing world state) to memory. */
	UFUNCTION(BlueprintCallable)
	void RegisterActorInMemory(AActor* Actor);
	/** Remove data (actor providing world state) from memory. */
	UFUNCTION(BlueprintCallable)
	void UnregisterActorFromMemory(AActor* Actor);

	/** Return all current memory data. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE TArray<AActor*> GetMemory() const { return Memory; }
	/** Return true if has specified actor in memory. */
	UFUNCTION(BlueprintCallable)
	bool IsActorInMemory(AActor* Actor) const;

private:

	/** Actual memory data. */
	UPROPERTY()
	TArray<AActor*> Memory;
};