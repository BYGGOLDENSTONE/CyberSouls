// BaseAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseAbilityComponent.generated.h"

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UBaseAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseAbilityComponent();

	// Basic ability properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FString AbilityName = "BaseAbility";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Cooldown = 0.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	float CurrentCooldown = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	bool bIsAbilityActive = false;

	// Virtual functions to override
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual bool CanActivateAbility();
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual void ActivateAbility();
	
	UFUNCTION(BlueprintCallable, Category = "Ability")
	virtual void DeactivateAbility();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};