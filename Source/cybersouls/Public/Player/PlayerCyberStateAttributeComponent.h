#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCyberStateAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChanged, float, CurrentStamina, float, MaxStamina);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UPlayerCyberStateAttributeComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UPlayerCyberStateAttributeComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float MaxStamina = 100.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float StaminaRegenRate = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
    float StaminaRegenDelay = 1.0f;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnStaminaChanged OnStaminaChanged;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void UseStamina(float Amount);

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    bool HasEnoughStamina(float Amount) const;

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    float GetCurrentStamina() const { return CurrentStamina; }

    UFUNCTION(BlueprintCallable, Category = "Stamina")
    float GetStaminaPercentage() const { return CurrentStamina / MaxStamina; }

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    float CurrentStamina;
    float TimeSinceLastStaminaUse;
    void RegenerateStamina(float DeltaTime);
};