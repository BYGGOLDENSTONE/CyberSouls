// AttackAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "AttackAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UAttackAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UAttackAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDamage = 10.0f; // Physical enemies deal 10 damage to player integrity
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRange = 200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackCooldown = 2.0f; // 2 second cooldown between attacks

	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;

protected:
	virtual void BeginPlay() override;
	
private:
	void PerformAttack();
	AActor* GetTarget() const;
};