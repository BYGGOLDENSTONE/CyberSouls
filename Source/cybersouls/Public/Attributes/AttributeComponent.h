// AttributeComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChanged, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	// Player Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Integrity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxIntegrity = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float HackProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float MaxHackProgress = 100.0f;

	// Combat Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 BlockCharge = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxBlockCharge = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 DodgeCharge = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	int32 MaxDodgeCharge = 3;

	// Speed Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float AttackSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Speed")
	float SlashSpeed = 1.0f;

	// Delegates
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnIntegrityChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChanged OnHackProgressChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDeath OnDeath;

	// Functions
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyIntegrity(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ModifyHackProgress(float Delta);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool ConsumeBlockCharge();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool ConsumeDodgeCharge();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const { return Integrity > 0.0f; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsHacked() const { return HackProgress >= MaxHackProgress; }

protected:
	virtual void BeginPlay() override;

private:
	void CheckIntegrityStatus();
	void CheckHackStatus();
};