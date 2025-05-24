// PhysicalEnemyAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "PhysicalEnemyAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhysicalEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPhysicalEnemyDamaged, float, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPhysicalEnemyAttributeComponent : public UEnemyAttributeComponent
{
	GENERATED_BODY()

public:
	UPhysicalEnemyAttributeComponent();

	// Additional events specific to physical enemies
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDeath OnPhysicalDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDamaged OnPhysicalDamaged;
};