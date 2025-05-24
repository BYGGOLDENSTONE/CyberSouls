// HackAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "HackAbilityComponent.generated.h"

class UHackingEnemyAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UHackAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UHackAbilityComponent();

	// Hack parameters are now retrieved from HackingEnemyAttributeComponent
	// to maintain single source of truth

	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;
	
	// Get hack parameters from enemy attributes
	float GetHackRate() const;
	float GetHackRange() const;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void PerformContinuousHack(float DeltaTime);
	AActor* GetTarget() const;
	UHackingEnemyAttributeComponent* GetEnemyAttributes() const;
};