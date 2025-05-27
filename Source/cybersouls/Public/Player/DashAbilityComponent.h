#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "DashAbilityComponent.generated.h"

class UPlayerCyberStateAttributeComponent;

/**
 * Dash ability component for CyberState character
 * 
 * Provides a quick dash movement ability that consumes stamina.
 * The dash moves the character rapidly in the input direction
 * or forward if no input is provided.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UDashAbilityComponent : public UBaseAbilityComponent
{
    GENERATED_BODY()

public:
    UDashAbilityComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float StaminaCost = 25.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DashDistance = 1000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DashDuration = 0.2f;

    // DashCooldown moved to base class Cooldown property

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bCanDashInAir = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charges")
    int32 MaxCharges = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Charges")
    float ChargeRegenTime = 3.0f;

    UPROPERTY(BlueprintReadOnly, Category = "Charges")
    int32 CurrentCharges;

    UPROPERTY(BlueprintReadOnly, Category = "Charges")
    float ChargeRegenTimer;
    
    /**
     * Check if the dash ability can be performed
     * 
     * Validates stamina requirements, cooldown status, and air dash permissions.
     * 
     * @return True if all conditions are met for dashing
     */
    virtual bool CanActivateAbility() override;

    /**
     * Execute the dash ability
     * 
     * Consumes stamina and initiates the dash movement if conditions are met.
     * The dash direction is based on current movement input or facing direction.
     */
    virtual void ActivateAbility() override;
    
    /**
     * End the dash ability
     */
    virtual void DeactivateAbility() override;

    UFUNCTION(BlueprintCallable, Category = "Charges")
    int32 GetCurrentCharges() const { return CurrentCharges; }

    UFUNCTION(BlueprintCallable, Category = "Charges")
    int32 GetMaxCharges() const { return MaxCharges; }

    UFUNCTION(BlueprintCallable, Category = "Charges")
    float GetChargeRegenProgress() const { return ChargeRegenTimer / ChargeRegenTime; }

    void ResetCharges();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
    class ACharacter* OwnerCharacter;

    UPROPERTY()
    UPlayerCyberStateAttributeComponent* AttributeComponent;

    bool bIsDashing;
    float DashTimeRemaining;
    FVector DashDirection;
    FVector OriginalVelocity;

    /**
     * Initialize dash state and calculate direction
     */
    void StartDash();
    
    /**
     * Update dash movement each frame while dashing
     * 
     * @param DeltaTime Time since last update
     */
    void UpdateDash(float DeltaTime);
    
    /**
     * Clean up dash state and apply exit velocity
     */
    void EndDash();
};