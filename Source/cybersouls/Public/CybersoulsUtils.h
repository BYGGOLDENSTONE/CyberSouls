#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CybersoulsUtils.generated.h"

/**
 * Utility library for common Cybersouls gameplay functions
 * 
 * Provides static helper functions for distance calculations,
 * safe component access, and line-of-sight checks used throughout
 * the project.
 */
UCLASS()
class CYBERSOULS_API UCybersoulsUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
    
public:
    /**
     * Calculate the distance between two actors
     * 
     * @param Actor1 First actor (can be null)
     * @param Actor2 Second actor (can be null)
     * @return Distance between actors, or -1.0f if either actor is invalid
     */
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static float GetDistanceBetweenActors(const AActor* Actor1, const AActor* Actor2);
    
    /**
     * Calculate the distance between two world locations
     * 
     * @param Location1 First world location
     * @param Location2 Second world location
     * @return Euclidean distance between the two locations
     */
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static float GetDistanceBetweenLocations(const FVector& Location1, const FVector& Location2);
    
    /**
     * Check if two actors are within a specified range
     * 
     * @param Actor1 First actor to check
     * @param Actor2 Second actor to check
     * @param Range Maximum distance to consider "within range"
     * @return True if actors are valid and within range, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static bool IsActorWithinRange(const AActor* Actor1, const AActor* Actor2, float Range);
    
    /**
     * Safely get a component from an actor
     * 
     * Performs null checks before attempting to find the component.
     * 
     * @param Actor The actor to search for the component
     * @return The component if found, nullptr otherwise
     */
    template<typename T>
    static T* GetComponentSafe(AActor* Actor)
    {
        if (!IsValid(Actor))
        {
            return nullptr;
        }
        return Actor->FindComponentByClass<T>();
    }
    
    /**
     * Check if one actor can see another using a line trace
     * 
     * Performs a visibility line trace from observer to target, accounting
     * for eye height offset on the observer.
     * 
     * @param World The world context for the line trace
     * @param Observer The actor doing the looking
     * @param Target The actor being looked at
     * @param EyeHeight Vertical offset from observer's location for the trace start
     * @return True if there's a clear line of sight, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "Cybersouls|Utils")
    static bool CanSeeActor(const UWorld* World, const AActor* Observer, const AActor* Target, float EyeHeight = 80.0f);
};