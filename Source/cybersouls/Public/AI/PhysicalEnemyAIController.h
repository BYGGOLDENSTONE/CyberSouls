// PhysicalEnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PhysicalEnemyAIController.generated.h"

UCLASS()
class CYBERSOULS_API APhysicalEnemyAIController : public AAIController
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
	UPROPERTY()
	class ACybersoulsEnemyBase* ControlledEnemy;
	
	UPROPERTY()
	class AActor* PlayerTarget;
	
	// Close combat behavior parameters
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AttackRange = 300.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ChaseSpeed = 400.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float AcceptanceRadius = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SightRange = 1500.0f;
	
	// Attack timing
	FTimerHandle AttackTimerHandle;
	
	// Search behavior
	FVector LastKnownPlayerLocation;
	bool bIsSearching = false;
	float SearchTimeRemaining = 0.0f;
	const float MaxSearchTime = 5.0f; // Search for 5 seconds before giving up
	
	// Communication system
	UPROPERTY(EditDefaultsOnly, Category = "AI|Communication")
	float AlertRadius = 2000.0f; // Radius to alert other enemies
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Communication")
	float AlertUpdateInterval = 1.0f; // Update allies every second
	
	bool bIsAlerted = false; // Whether this enemy has been alerted by another
	FVector AlertLocation; // Location to investigate when alerted
	FTimerHandle AlertUpdateTimerHandle;
	
	void UpdateCombatBehavior();
	bool CanSeeTarget(AActor* Target) const;
	float GetDistanceToTarget(AActor* Target) const;
	void MoveToTarget();
	void MoveToLocation(const FVector& Location);
	void PerformAttack();
	bool IsInAttackRange() const;
	void StartSearchBehavior();
	void UpdateSearchBehavior(float DeltaTime);
	
	// Communication methods
	void AlertNearbyEnemies(const FVector& PlayerLocation);
	void UpdateAlliesWithPlayerLocation();
	
	// Path following callback
	void OnMoveCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);
	
public:
	// Called by other AI controllers to alert this one
	void ReceiveAlert(const FVector& PlayerLocation);
};