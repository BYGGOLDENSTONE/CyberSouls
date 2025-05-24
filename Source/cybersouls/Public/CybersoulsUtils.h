#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CybersoulsUtils.generated.h"

UCLASS()
class CYBERSOULS_API UCybersoulsUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    // Distance calculation utilities
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static float GetDistanceBetweenActors(const AActor* Actor1, const AActor* Actor2);
    
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static float GetDistanceBetweenLocations(const FVector& Location1, const FVector& Location2);
    
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static bool IsActorWithinRange(const AActor* Actor1, const AActor* Actor2, float Range);
    
    // Component utility functions
    template<typename T>
    static T* GetComponentSafe(AActor* Actor)
    {
        if (!IsValid(Actor))
        {
            return nullptr;
        }
        return Actor->FindComponentByClass<T>();
    }
    
    // Line trace utilities
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static bool CanSeeActor(const UWorld* World, const AActor* Observer, const AActor* Target, float EyeHeight = 80.0f);
};