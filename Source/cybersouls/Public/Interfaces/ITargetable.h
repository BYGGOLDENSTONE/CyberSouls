#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "cybersouls/Public/CybersoulsUtils.h"
#include "ITargetable.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UTargetable : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that can be targeted by abilities
 */
class CYBERSOULS_API ITargetable
{
    GENERATED_BODY()

public:
    /**
     * Check if this actor can be targeted
     * @return True if the actor can be targeted
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
    bool CanBeTargeted() const;
    
    /**
     * Get the body part that would be hit at the given location
     * @param HitLocation The world location of the potential hit
     * @return The body part that would be hit
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
    EBodyPart GetTargetBodyPart(const FVector& HitLocation) const;
    
    /**
     * Get the optimal targeting location for this actor
     * @return The world location to target
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
    FVector GetTargetLocation() const;
    
    /**
     * Check if this target is valid for a specific ability type
     * @param AbilityClass The ability class to check
     * @return True if valid for the ability
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Targeting")
    bool IsValidTargetForAbility(TSubclassOf<UObject> AbilityClass) const;
};