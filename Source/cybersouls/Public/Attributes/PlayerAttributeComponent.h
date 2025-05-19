// PlayerAttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIntegrityChanged, float, NewIntegrity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHackProgressChanged, float, NewHackProgress);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPlayerAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttributeComponent();

	// Player attributes - different from enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Integrity = 100.0f; // Player doesn't die from integrity loss
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxIntegrity = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float HackProgress = 0.0f; // Player dies when this reaches 100
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHackProgress = 100.0f;
	
	// Ability availability (can be disabled by QuickHacks)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bCanUseAbilities = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bIsImmobilized = false;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Status")
	bool bHasFirewall = false;

	// Events
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnDeath;
	
	UPROPERTY(BlueprintAssignable)
	FOnIntegrityChanged OnIntegrityChanged;
	
	UPROPERTY(BlueprintAssignable)
	FOnHackProgressChanged OnHackProgressChanged;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void TakeDamage(float Damage);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void IncreaseHackProgress(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void RestoreIntegrity(float Amount);
	
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

protected:
	virtual void BeginPlay() override;
	
private:
	void CheckDeath();
};