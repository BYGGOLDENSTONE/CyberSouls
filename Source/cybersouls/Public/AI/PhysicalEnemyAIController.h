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

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
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
	
	void UpdateCombatBehavior();
	bool CanSeeTarget(AActor* Target) const;
	float GetDistanceToTarget(AActor* Target) const;
	void MoveToTarget();
	void PerformAttack();
	bool IsInAttackRange() const;
};