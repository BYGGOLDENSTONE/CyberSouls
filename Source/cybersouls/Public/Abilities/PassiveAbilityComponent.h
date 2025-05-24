// PassiveAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "PassiveAbilityComponent.generated.h"

UENUM(BlueprintType)
enum class EPassiveAbilityType : uint8
{
	None UMETA(DisplayName = "None"),
	ExecutionChains UMETA(DisplayName = "Execution Chains"),
	SystemOvercharge UMETA(DisplayName = "System Overcharge")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPassiveAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UPassiveAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	EPassiveAbilityType PassiveType = EPassiveAbilityType::None;
	
	// Execution Chains tracking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Passive")
	bool bExecutionChainsActive = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float ExecutionChainsWindow = 5.0f;
	
	// System Overcharge tracking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Passive")
	bool bSystemOverchargeActive = false;

	UFUNCTION(BlueprintCallable, Category = "Passive")
	void OnEnemyKilled();
	
	UFUNCTION(BlueprintCallable, Category = "Passive")
	bool ShouldIgnoreBlock() const;
	
	UFUNCTION(BlueprintCallable, Category = "Passive")
	bool ShouldIgnoreAllDefenses() const;
	
	UFUNCTION(BlueprintCallable, Category = "Passive")
	bool CanUseQuickHacks() const;
	
	UFUNCTION(BlueprintCallable, Category = "Passive")
	EPassiveAbilityType GetPassiveType() const { return PassiveType; }

protected:
	virtual void BeginPlay() override;
	
private:
	FTimerHandle ExecutionChainsTimerHandle;
	
	void ActivateExecutionChains();
	void DeactivateExecutionChains();
};