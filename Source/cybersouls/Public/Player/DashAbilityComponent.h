#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashAbilityComponent.generated.h"

class UPlayerCyberStateAttributeComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UDashAbilityComponent : public UActorComponent
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    float DashCooldown = 0.5f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
    bool bCanDashInAir = true;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    bool CanPerformAbility() const;

    UFUNCTION(BlueprintCallable, Category = "Ability")
    void PerformAbility();

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
    float CooldownTimeRemaining;
    FVector DashDirection;
    FVector OriginalVelocity;

    void StartDash();
    void UpdateDash(float DeltaTime);
    void EndDash();
};