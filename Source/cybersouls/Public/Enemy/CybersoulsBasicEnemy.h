// CybersoulsBasicEnemy.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsBasicEnemy.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsBasicEnemy : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsBasicEnemy();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPhysicalEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UAttackAbilityComponent* AttackAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
};