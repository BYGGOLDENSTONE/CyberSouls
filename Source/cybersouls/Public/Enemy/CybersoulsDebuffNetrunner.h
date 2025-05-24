// CybersoulsDebuffNetrunner.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "CybersoulsDebuffNetrunner.generated.h"

UCLASS()
class CYBERSOULS_API ACybersoulsDebuffNetrunner : public ACybersoulsEnemyBase
{
	GENERATED_BODY()

public:
	ACybersoulsDebuffNetrunner();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackingEnemyAttributeComponent* EnemyAttributes;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UHackAbilityComponent* HackAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UQuickHackComponent* SystemFreezeAbility;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UQuickHackComponent* InterruptProtocolAbility;

protected:
	virtual void BeginPlay() override;
	virtual void InitializeEnemy() override;
	virtual void TickActor(float DeltaTime, ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
private:
	UPROPERTY(EditAnywhere, Category = "QuickHack")
	float QuickHackChance = 0.4f; // Chance to cast QuickHack per second
	
	UPROPERTY(EditAnywhere, Category = "QuickHack")
	float SystemFreezePreference = 0.6f; // Preference for SystemFreeze over InterruptProtocol
};