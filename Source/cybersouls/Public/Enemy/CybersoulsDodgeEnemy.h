// CybersoulsDodgeEnemy.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsDodgeEnemy.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsDodgeEnemy : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsDodgeEnemy();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPhysicalEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAttackAbilityComponent* AttackAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UDodgeAbilityComponent* DodgeAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
};