#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseEnemyAIController.generated.h"

/**
 * Base AI controller for all enemy types in Cybersouls
 * 
 * Provides common functionality for enemy perception, target tracking,
 * and inter-enemy communication. Derived classes implement specific
 * behaviors for physical and hacking enemy types.
 */
UCLASS()
class CYBERSOULS_API ABaseEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseEnemyAIController();

	/**
	 * Update player target reference when character switches
	 * 
	 * Called when the player switches between characters to ensure
	 * AI controllers track the correct player pawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdatePlayerTarget();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	/**
	 * Check if this AI can see the specified target
	 * 
	 * Performs a line trace from the AI's eye position to the target.
	 * 
	 * @param Target The actor to check visibility for
	 * @return True if there's a clear line of sight to the target
	 */
	virtual bool CanSeeTarget(AActor* Target) const;
	
	/**
	 * Calculate distance to a target actor
	 * 
	 * @param Target The actor to measure distance to
	 * @return Distance in Unreal units, or -1.0f if invalid
	 */
	float GetDistanceToTarget(AActor* Target) const;
	
	/**
	 * Alert nearby enemies about a detected target
	 * 
	 * Starts continuous location updates to allies within AlertRadius.
	 * 
	 * @param Target The detected target (usually the player)
	 */
	void AlertNearbyEnemies(AActor* Target);
	
	/**
	 * Send player location update to all nearby allies
	 * 
	 * Called periodically while tracking a target to keep allies informed.
	 */
	void UpdateAlliesWithPlayerLocation();
	
	/**
	 * Stop sending alerts to allies
	 * 
	 * Clears the alert timer and resets alert state.
	 */
	void StopAlertingAllies();
	
	/**
	 * Handle receiving an alert from another enemy
	 * 
	 * Override in derived classes to implement alert response behavior.
	 * 
	 * @param AlertTarget The target that was spotted
	 * @param InAlertLocation Last known location of the target
	 */
	virtual void ReceiveAlert(AActor* AlertTarget, const FVector& InAlertLocation) {}
	
	/**
	 * Handle continuous player location updates from allies
	 * 
	 * Override in derived classes to implement tracking behavior.
	 * 
	 * @param Player The player being tracked
	 * @param PlayerLocation Current location of the player
	 */
	virtual void ReceivePlayerLocationUpdate(AActor* Player, const FVector& PlayerLocation) {}
	
	/**
	 * Called when this AI gains sight of the player
	 * 
	 * Override to implement visibility gained behavior.
	 */
	virtual void HandlePlayerVisibility() {}
	
	/**
	 * Called when this AI loses sight of the player
	 * 
	 * Override to implement visibility lost behavior.
	 */
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