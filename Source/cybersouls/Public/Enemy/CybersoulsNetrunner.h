// CybersoulsNetrunner.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsNetrunner.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsNetrunner : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsNetrunner();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackingEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackAbilityComponent* HackAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
	
	// Death handling
	UFUNCTION()
	void OnEnemyDeath();
};