// HackAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "HackAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UHackAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	UHackAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hack")
	float HackRate = 2.0f; // Hack progress per second
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hack")
	float HackRange = 1500.0f;

	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	void PerformContinuousHack(float DeltaTime);
	AActor* GetTarget() const;
};