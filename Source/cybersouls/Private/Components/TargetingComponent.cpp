#include "cybersouls/Public/Components/TargetingComponent.h"
#include "cybersouls/Public/Enemy/CybersoulsEnemyBase.h"
#include "cybersouls/Public/Interfaces/ITargetable.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

UTargetingComponent::UTargetingComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentTarget = nullptr;
    CameraComponent = nullptr;
    CurrentBodyPart = EBodyPart::None;
}

void UTargetingComponent::BeginPlay()
{
    Super::BeginPlay();

    // Try to find camera component if not set
    if (!CameraComponent && GetOwner())
    {
        CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
    }
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateTargeting();
}

void UTargetingComponent::UpdateTargeting()
{
    AActor* PreviousTarget = CurrentTarget;
    EBodyPart PreviousBodyPart = CurrentBodyPart;

    // Perform crosshair trace
    FHitResult HitResult = PerformCrosshairTrace();

    if (HitResult.bBlockingHit)
    {
        AActor* HitActor = HitResult.GetActor();
        
        // Check if the hit actor implements ITargetable
        if (HitActor && HitActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
        {
            if (ITargetable::Execute_CanBeTargeted(HitActor))
            {
                CurrentTarget = HitActor;
                DetermineBodyPart(HitResult);
            }
            else
            {
                CurrentTarget = nullptr;
                CurrentBodyPart = EBodyPart::None;
            }
        }
        // Legacy support for enemies without interface
        else if (ACybersoulsEnemyBase* Enemy = Cast<ACybersoulsEnemyBase>(HitActor))
        {
            CurrentTarget = Enemy;
            DetermineBodyPart(HitResult);
        }
        else
        {
            CurrentTarget = nullptr;
            CurrentBodyPart = EBodyPart::None;
        }
    }
    else
    {
        CurrentTarget = nullptr;
        CurrentBodyPart = EBodyPart::None;
    }

    // Fire events if changed
    if (CurrentTarget != PreviousTarget)
    {
        OnTargetChanged.Broadcast(CurrentTarget);
    }
    if (CurrentBodyPart != PreviousBodyPart)
    {
        OnBodyPartChanged.Broadcast(CurrentBodyPart);
    }
}

FHitResult UTargetingComponent::PerformCrosshairTrace() const
{
    FHitResult HitResult;

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn)
    {
        return HitResult;
    }

    APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
    if (!PC || !CameraComponent)
    {
        return HitResult;
    }

    // Get screen center
    FVector2D ScreenCenter = GetScreenCenter();

    // Convert screen position to world ray
    FVector WorldLocation, WorldDirection;
    if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
    {
        // Perform line trace from camera
        FVector TraceStart = CameraComponent->GetComponentLocation();
        FVector TraceEnd = TraceStart + (WorldDirection * MaxTargetingRange);

        FCollisionQueryParams TraceParams;
        TraceParams.AddIgnoredActor(GetOwner());
        TraceParams.bTraceComplex = true;

        GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, TraceChannel, TraceParams);

        // Debug visualization
        if (bDebugDrawTrace)
        {
            DrawDebugLine(GetWorld(), TraceStart, TraceEnd, 
                HitResult.bBlockingHit ? FColor::Red : FColor::Green, false, 0.1f);
            
            if (HitResult.bBlockingHit)
            {
                DrawDebugSphere(GetWorld(), HitResult.Location, 10.0f, 12, FColor::Red, false, 0.1f);
            }
        }
    }

    return HitResult;
}

bool UTargetingComponent::IsTargetValid(AActor* Target) const
{
    if (!Target)
    {
        return false;
    }

    // Check if implements ITargetable
    if (Target->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
    {
        return ITargetable::Execute_CanBeTargeted(Target);
    }

    // Legacy check for enemies
    return Cast<ACybersoulsEnemyBase>(Target) != nullptr;
}

void UTargetingComponent::DetermineBodyPart(const FHitResult& HitResult)
{
    AActor* HitActor = HitResult.GetActor();
    if (!HitActor)
    {
        CurrentBodyPart = EBodyPart::None;
        return;
    }

    // If actor implements ITargetable, use its method
    if (HitActor->GetClass()->ImplementsInterface(UTargetable::StaticClass()))
    {
        CurrentBodyPart = ITargetable::Execute_GetTargetBodyPart(HitActor, HitResult.Location);
        return;
    }

    // Legacy body part determination
    FVector HitLocation = HitResult.Location;
    FVector ActorLocation = HitActor->GetActorLocation();
    FVector Origin, BoxExtent;
    HitActor->GetActorBounds(false, Origin, BoxExtent);

    // Determine body part based on hit height relative to actor
    float RelativeHeight = HitLocation.Z - ActorLocation.Z;
    
    if (RelativeHeight > BoxExtent.Z * 0.3f)
    {
        CurrentBodyPart = EBodyPart::UpperBody;
    }
    else
    {
        // Determine left or right leg based on Y position
        float RelativeY = HitLocation.Y - ActorLocation.Y;
        if (RelativeY > 0)
        {
            CurrentBodyPart = EBodyPart::RightLeg;
        }
        else
        {
            CurrentBodyPart = EBodyPart::LeftLeg;
        }
    }
}

FVector2D UTargetingComponent::GetScreenCenter() const
{
    if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
    {
        int32 ViewportSizeX, ViewportSizeY;
        PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
        return FVector2D(ViewportSizeX * 0.5f, ViewportSizeY * 0.5f);
    }
    return FVector2D::ZeroVector;
}