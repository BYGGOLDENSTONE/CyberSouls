// QuickHackComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "QuickHackComponent.generated.h"

UENUM(BlueprintType)
enum class EQuickHackType : uint8
{
	None UMETA(DisplayName = "None"),
	InterruptProtocol UMETA(DisplayName = "Interrupt Protocol"),
	SystemFreeze UMETA(DisplayName = "System Freeze"),
	Firewall UMETA(DisplayName = "Firewall")
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UQuickHackComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UQuickHackComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
	EQuickHackType QuickHackType = EQuickHackType::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
	float CastTime = 5.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickHack")
	float CurrentCastTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
	bool bIsSelfTargeted = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
	float EffectDuration = 3.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "QuickHack")
	float Range = 1000.0f;

	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void StartQuickHack(AActor* Target = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void InterruptQuickHack();
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	bool IsQuickHackActive() const { return bIsAbilityActive; }
	
	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	void CompleteQuickHack();
	void ApplyQuickHackEffect();
};