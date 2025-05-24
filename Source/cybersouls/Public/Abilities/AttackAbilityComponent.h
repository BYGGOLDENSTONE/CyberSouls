// AttackAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "AttackAbilityComponent.generated.h"

class UPhysicalEnemyAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UAttackAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UAttackAbilityComponent();

	// Attack properties - single source of truth for attack parameters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDamage = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackCooldown = 2.0f;

	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;
	
	// Get attack parameters from this component
	float GetAttackDamage() const;
	float GetAttackRange() const;
	float GetAttackCooldown() const;

protected:
	virtual void BeginPlay() override;
	
private:
	void PerformAttack();
	AActor* GetTarget() const;
	UPhysicalEnemyAttributeComponent* GetEnemyAttributes() const;
};