// CybersoulsBuffNetrunner.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsBuffNetrunner.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsBuffNetrunner : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsBuffNetrunner();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackingEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackAbilityComponent* HackAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UQuickHackComponent* FirewallAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "QuickHack")
	float FirewallChance = 0.3f; // Chance to cast Firewall per second
};