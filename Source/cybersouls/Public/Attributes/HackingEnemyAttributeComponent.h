// HackingEnemyAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Attributes/EnemyAttributeComponent.h"
#include "HackingEnemyAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHackingEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHackingEnemyDamaged, float, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UHackingEnemyAttributeComponent : public UEnemyAttributeComponent
{
	GENERATED_BODY()

public:
	UHackingEnemyAttributeComponent();

	// Hacking enemy specific stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackRate = 2.0f; // Hack progress per second
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float HackRange = 1500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	bool bHasQuickHacks = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	float QuickHackChance = 0.3f; // 30% chance per second

	// Additional events specific to hacking enemies
	UPROPERTY(BlueprintAssignable)
	FOnHackingEnemyDeath OnHackingDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnHackingEnemyDamaged OnHackingDamaged;
};