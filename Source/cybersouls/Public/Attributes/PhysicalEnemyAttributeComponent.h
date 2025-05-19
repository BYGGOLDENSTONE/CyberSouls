// PhysicalEnemyAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicalEnemyAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhysicalEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhysicalEnemyDamaged, float, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPhysicalEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPhysicalEnemyAttributeComponent();

	// Physical enemy attributes - all die in one hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Integrity = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxIntegrity = 1.0f;
	
	// Physical enemy specific stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackSpeed = 2.0f; // Attacks per second
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 300.0f;

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDeath OnDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDamaged OnDamaged;

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