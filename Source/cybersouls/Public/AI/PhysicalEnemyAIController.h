// PhysicalEnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/AI/BaseEnemyAIController.h"
#include "PhysicalEnemyAIController.generated.h"

UCLASS()
class CYBERSOULS_API APhysicalEnemyAIController : public ABaseEnemyAIController
{
	GENERATED_BODY()

public:
	APhysicalEnemyAIController();
	virtual ~APhysicalEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void Tick(float DeltaTime) override;

private:
	// Close combat behavior parameters
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackRange = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ChaseSpeed = 400.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 50.0f;
	
	// Attack timing
	FTimerHandle AttackTimerHandle;
	
	// Search behavior
	FVector LastKnownPlayerLocation;
	bool bIsSearching = false;
	float SearchTimeRemaining = 0.0f;
	const float MaxSearchTime = 5.0f; // Search for 5 seconds before giving up
	
	// Alert handling
	bool bIsAlerted = false; // Whether this enemy has been alerted by another
	FVector AlertLocation; // Location to investigate when alerted
	
	void UpdateCombatBehavior();
	void MoveToTarget();
	void MoveToLocation(const FVector& Location);
	void PerformAttack();
	bool IsInAttackRange() const;
	void StartSearchBehavior();
	void UpdateSearchBehavior(float DeltaTime);
	
	// Path following callback
	void OnMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);
	
	// Override visibility handling from base class
	virtual void HandlePlayerVisibility() override;
	virtual void HandlePlayerLostVisibility() override;
	
public:
	// Override from base class
	virtual void ReceiveAlert(AActor* AlertTarget, const FVector& InAlertLocation) override;
	virtual void ReceivePlayerLocationUpdate(AActor* Player, const FVector& PlayerLocation) override;
};