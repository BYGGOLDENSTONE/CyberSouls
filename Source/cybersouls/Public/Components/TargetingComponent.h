#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "cybersouls/Public/CybersoulsUtils.h"
#include "TargetingComponent.generated.h"

// Forward declarations
class ACybersoulsEnemyBase;
class UCameraComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetChanged, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBodyPartChanged, EBodyPart);

/**
 * Component that handles crosshair targeting and body part detection
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CYBERSOULS_API UTargetingComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTargetingComponent();

    // Configuration
    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float MaxTargetingRange = 5000.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    bool bDebugDrawTrace = false;

    // Core functionality
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    EBodyPart GetTargetBodyPart() const { return CurrentBodyPart; }

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    FHitResult PerformCrosshairTrace() const;

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    bool IsTargetValid(AActor* Target) const;

    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void SetCameraComponent(UCameraComponent* Camera) { CameraComponent = Camera; }

    // Events
    FOnTargetChanged OnTargetChanged;
    FOnBodyPartChanged OnBodyPartChanged;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
    AActor* CurrentTarget;

    UPROPERTY()
    UCameraComponent* CameraComponent;

    EBodyPart CurrentBodyPart;

    void UpdateTargeting();
    void DetermineBodyPart(const FHitResult& HitResult);
    FVector2D GetScreenCenter() const;
};