// HackingEnemyAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HackingEnemyAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHackingEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHackingEnemyDamaged, float, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UHackingEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHackingEnemyAttributeComponent();

	// Hacking enemy attributes - also die in one hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Integrity = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxIntegrity = 1.0f;
	
	// Hacking enemy specific stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackRate = 2.0f; // Hack progress per second
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackRange = 1500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	bool bHasQuickHacks = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float QuickHackChance = 0.3f; // 30% chance per second

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnHackingEnemyDeath OnDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnHackingEnemyDamaged OnDamaged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual void TakeDamage(float Damage);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

protected:
	virtual void BeginPlay() override;
	
private:
	void CheckDeath();
};