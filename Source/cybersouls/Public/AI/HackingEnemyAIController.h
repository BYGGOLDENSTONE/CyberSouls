// HackingEnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/AI/BaseEnemyAIController.h"
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
class CYBERSOULS_API AHackingEnemyAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	AHackingEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;
	
	// Override visibility handling from base class
	virtual void HandlePlayerVisibility() override;
	virtual void HandlePlayerLostVisibility() override;

private:
	// Range combat behavior parameters
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float HackRange = 1500.0f;
	
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
	void PerformHacking();
	void AttemptQuickHack();
	bool IsInHackRange() const;
	
	// New methods for improved QuickHack logic
	bool IsPlayerCastingQuickHack() const;
	EEnemyQuickHackType DecideQuickHackType() const;
	bool CanUseQuickHack(class UQuickHackComponent* QuickHack) const;
	void ExecuteQuickHack(EEnemyQuickHackType Type);
	void CacheQuickHackComponents();
};