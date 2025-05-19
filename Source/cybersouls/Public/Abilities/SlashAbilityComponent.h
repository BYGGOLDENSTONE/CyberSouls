// SlashAbilityComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "cybersouls/Public/Combat/BodyPartComponent.h"
#include "SlashAbilityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API USlashAbilityComponent : public UBaseAbilityComponent
{
	GENERATED_BODY()

public:
	USlashAbilityComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slash")
	float SlashDamage = 10.0f; // One-shots enemies unless they block/dodge
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slash")
	float SlashRange = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slash")
	float SlashAngle = 45.0f; // Cone angle for slash

	virtual void ActivateAbility() override;
	virtual bool CanActivateAbility() override;

protected:
	virtual void BeginPlay() override;
	
private:
	void PerformSlash();
	TArray<AActor*> GetTargetsInRange() const;
	EBodyPart GetTargetedBodyPart() const;
};