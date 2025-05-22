// CybersoulsEnemyBase.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "CybersoulsEnemyBase.generated.h"

UENUM(BlueprintType)
enum class EEnemyType : uint8
{
	Basic UMETA(DisplayName = "Basic"),
	Block UMETA(DisplayName = "Block"),
	Dodge UMETA(DisplayName = "Dodge"),
	Netrunner UMETA(DisplayName = "Netrunner"),
	BuffNetrunner UMETA(DisplayName = "Buff Netrunner"),
	DebuffNetrunner UMETA(DisplayName = "Debuff Netrunner")
};

UCLASS()
class CYBERSOULS_API ACybersoulsEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACybersoulsEnemyBase();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	EEnemyType EnemyType = EEnemyType::Basic;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBodyPartComponent* UpperBodyPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBodyPartComponent* LeftLegPart;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBodyPartComponent* RightLegPart;

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	virtual void InitializeEnemy();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool TryBlock(EBodyPart TargetedPart);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool TryDodge(EBodyPart TargetedPart);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PerformAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PerformHack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void OnReceivedDamage(float Damage, EBodyPart HitPart);

	// Check if enemy is dead
	bool IsDead() const { return bIsDead; }

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnDeath();

	// Handle ragdoll death sequence
	virtual void StartDeathSequence();
	
	// Shatter death effect components
	// Note: We create skeletal mesh pieces dynamically
	
	// Create shattered mesh pieces
	void CreateShatteredPieces();
	
	// Number of shatter pieces to create
	UPROPERTY(EditDefaultsOnly, Category = "Death Effect")
	int32 ShatterPieceCount = 50;

	// Default AI Controller class to use
	virtual TSubclassOf<AController> GetDefaultControllerClass() const;

	void SetupBodyParts();
	void SetupAttributesForType();
	void SetupAbilitiesForType();

protected:
	UPROPERTY()
	AActor* CurrentTarget = nullptr;
	
	// Helper functions
	bool IsTargetInRange(AActor* Target, float Range) const;

	// Track death state
	bool bIsDead = false;
	
private:
	FTimerHandle AttackTimerHandle;
	FTimerHandle HackTimerHandle;
	FTimerHandle DeathTimerHandle;
	
	// Timer callbacks
	void OnAttackTimer();
	void OnHackTimer();
	
	// Helper functions
	void StartBehaviorTimers();
};