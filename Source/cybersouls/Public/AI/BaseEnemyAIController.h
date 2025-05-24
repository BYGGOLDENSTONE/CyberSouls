#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

UCLASS()
class CYBERSOULS_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// Common functionality
	bool CanSeeTarget(AActor* Target) const;
	float GetDistanceToTarget(AActor* Target) const;
	void AlertNearbyEnemies(AActor* Target);
	void UpdateAlliesWithPlayerLocation();
	void StopAlertingAllies();
	
	// Alert receiving methods - to be overridden by derived classes
	virtual void ReceiveAlert(AActor* AlertTarget, const FVector& InAlertLocation) {}
	virtual void ReceivePlayerLocationUpdate(AActor* Player, const FVector& PlayerLocation) {}
	
	// Player visibility handling
	virtual void HandlePlayerVisibility() {}
	virtual void HandlePlayerLostVisibility() {}

	// Configuration properties
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float SightRange = 2000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception")
	float EyeHeight = 80.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Communication")
	float AlertRadius = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Communication")
	float AlertUpdateInterval = 1.0f;

	// Alert system
	FTimerHandle AlertUpdateTimerHandle;
	bool bIsAlertingAllies = false;
	AActor* CurrentTarget = nullptr;

	// Common member variables
	class ACybersoulsEnemyBase* ControlledEnemy = nullptr;
	AActor* PlayerTarget = nullptr;

	// Debug settings
	UPROPERTY(EditDefaultsOnly, Category = "AI|Debug")
	bool bDebugDrawSightLine = false;
};