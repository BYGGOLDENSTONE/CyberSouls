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

	// Physical enemy specific stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackDamage = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackSpeed = 2.0f; // Attacks per second
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRange = 300.0f;

	// Additional events specific to physical enemies
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDeath OnPhysicalDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnPhysicalEnemyDamaged OnPhysicalDamaged;
};