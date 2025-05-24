#include "cybersouls/Public/CybersoulsUtils.h"
#include "Engine/World.h"

float UCybersoulsUtils::GetDistanceBetweenActors(const AActor* Actor1, const AActor* Actor2)
{
    if (!IsValid(Actor1) || !IsValid(Actor2))
    {
        return 0.0f;
    }
    
    return FVector::Dist(Actor1->GetActorLocation(), Actor2->GetActorLocation());
}

float UCybersoulsUtils::GetDistanceBetweenLocations(const FVector& Location1, const FVector& Location2)
{
    return FVector::Dist(Location1, Location2);
}

bool UCybersoulsUtils::IsActorWithinRange(const AActor* Actor1, const AActor* Actor2, float Range)
{
    if (!IsValid(Actor1) || !IsValid(Actor2))
    {
        return false;
    }
    
    return GetDistanceBetweenActors(Actor1, Actor2) <= Range;
}

bool UCybersoulsUtils::CanSeeActor(const UWorld* World, const AActor* Observer, const AActor* Target, float EyeHeight)
{
    if (!IsValid(World) || !IsValid(Observer) || !IsValid(Target))
    {
        return false;
    }
    
    FVector StartLocation = Observer->GetActorLocation() + FVector(0, 0, EyeHeight);
    FVector EndLocation = Target->GetActorLocation();
    
    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(Observer);
    QueryParams.AddIgnoredActor(Target);
    
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );
    
    return !bHit;
}