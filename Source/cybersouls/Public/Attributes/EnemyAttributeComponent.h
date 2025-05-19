// EnemyAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnemyDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDamaged, float, Damage);

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyAttributeComponent();

	// Enemy attributes - most enemies die in one hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Integrity = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxIntegrity = 1.0f;

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDeath OnDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnEnemyDamaged OnDamaged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	virtual void TakeDamage(float Damage);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

protected:
	virtual void BeginPlay() override;
	
private:
	void CheckDeath();
};