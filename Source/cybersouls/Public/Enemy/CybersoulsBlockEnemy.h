// CybersoulsBlockEnemy.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsBlockEnemy.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsBlockEnemy : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsBlockEnemy();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPhysicalEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAttackAbilityComponent* AttackAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBlockAbilityComponent* BlockAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
	
	// Death handling
	UFUNCTION()
	void OnEnemyDeath();
};