// HackingEnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HackingEnemyAIController.generated.h"

UENUM(BlueprintType)
enum class EEnemyQuickHackType : uint8
{
	None UMETA(DisplayName = "None"),
	Firewall UMETA(DisplayName = "Firewall"),
	SystemFreeze UMETA(DisplayName = "System Freeze"),
	InterruptProtocol UMETA(DisplayName = "Interrupt Protocol")
};

UCLASS()
class CYBERSOULS_API AHackingEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHackingEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY()
	class ACybersoulsEnemyBase* ControlledEnemy;
	
	UPROPERTY()
	class AcybersoulsCharacter* PlayerTarget;
	
	// Range combat behavior parameters
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float HackRange = 1500.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SafeDistance = 800.0f; // Minimum distance to maintain
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RetreatSpeed = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SightRange = 2000.0f;
	
	// QuickHack timing
	FTimerHandle QuickHackTimerHandle;
	float QuickHackCooldown = 5.0f;
	
	// New QuickHack decision making
	UPROPERTY(EditDefaultsOnly, Category = "QuickHack")
	float QuickHackDecisionInterval = 1.0f;
	
	// Store references to QuickHack components
	UPROPERTY()
	class UQuickHackComponent* FirewallComponent = nullptr;
	
	UPROPERTY()
	class UQuickHackComponent* SystemFreezeComponent = nullptr;
	
	UPROPERTY()
	class UQuickHackComponent* InterruptProtocolComponent = nullptr;
	
	void UpdateHackingBehavior();
	bool CanSeeTarget(AActor* Target) const;
	float GetDistanceToTarget(AActor* Target) const;
	void MaintainDistance();
	void PerformHacking();
	void AttemptQuickHack();
	bool IsInHackRange() const;
	bool IsTooClose() const;
	
	// New methods for improved QuickHack logic
	bool IsPlayerCastingQuickHack() const;
	EEnemyQuickHackType DecideQuickHackType() const;
	bool CanUseQuickHack(class UQuickHackComponent* QuickHack) const;
	void ExecuteQuickHack(EEnemyQuickHackType Type);
	void CacheQuickHackComponents();
};