#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoubleJumpAbilityComponent.generated.h"

class UPlayerCyberStateAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UDoubleJumpAbilityComponent : public UActorComponent
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

    UFUNCTION(BlueprintCallable, Category = "Ability")
    bool CanPerformAbility() const;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void PerformAbility();
    
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