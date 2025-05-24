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
	Firewall UMETA(DisplayName = "Firewall"),
	Kill UMETA(DisplayName = "Kill"),
	CascadeVirus UMETA(DisplayName = "Cascade Virus"),
	GhostProtocol UMETA(DisplayName = "Ghost Protocol"),
	ChargeDrain UMETA(DisplayName = "Charge Drain"),
	GravityFlip UMETA(DisplayName = "Gravity Flip")
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

	// Cascade Virus tracking
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "QuickHack")
	TArray<AActor*> MarkedEnemies;

	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void StartQuickHack(AActor* Target = nullptr);
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void InterruptQuickHack();
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	bool IsQuickHackActive() const { return bIsAbilityActive; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void OnEnemyKilled(AActor* KilledEnemy);
	
	// Public methods for QuickHackManager
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void SetQuickHackType(EQuickHackType InType) { QuickHackType = InType; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	float GetCooldownRemaining() const { return CurrentCooldown; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	float GetCastTimeRemaining() const { return CurrentCastTime; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	bool IsCasting() const { return bIsAbilityActive && CurrentCastTime > 0.0f; }
	
	UFUNCTION(BlueprintCallable, Category = "QuickHack")
	void CancelQuickHack() { InterruptQuickHack(); }
	
	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
private:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	void CompleteQuickHack();
	void ApplyQuickHackEffect();
	
	// Cascade Virus helpers
	void MarkEnemyForCascade(AActor* Enemy);
	void RemoveMarkFromEnemy(AActor* Enemy);
	void TriggerCascadeEffect(AActor* KilledEnemy);
	TArray<AActor*> GetNearbyEnemies(AActor* CenterEnemy, float Radius = 800.0f);
};