#pragma once

#include "CoreMinimal.h"
#include "cybersouls/Public/Abilities/BaseAbilityComponent.h"
#include "DoubleJumpAbilityComponent.generated.h"

class UPlayerCyberStateAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UDoubleJumpAbilityComponent : public UBaseAbilityComponent
{
    GENERATED_BODY()

public:
    UDoubleJumpAbilityComponent();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float StaminaCost = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DoubleJumpVelocity = 600.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    int32 MaxJumpsInAir = 1;

    virtual bool CanActivateAbility() override;
    virtual void ActivateAbility() override;
    virtual void DeactivateAbility() override;
    
    void ResetJumpCount();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    class ACharacter* OwnerCharacter;

    UPROPERTY()
    UPlayerCyberStateAttributeComponent* AttributeComponent;

    int32 CurrentJumpsInAir;
    
    UFUNCTION()
    void OnMovementModeChanged(class ACharacter* Character, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);
};