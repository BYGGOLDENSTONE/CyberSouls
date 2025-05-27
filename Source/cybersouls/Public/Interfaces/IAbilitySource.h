#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IAbilitySource.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UAbilitySource : public UInterface
{
    GENERATED_BODY()
};

/**
 * Interface for actors that can use abilities and have resources
 */
class CYBERSOULS_API IAbilitySource
{
    GENERATED_BODY()

public:
    /**
     * Check if the source has enough of a specific resource
     * @param ResourceType The type of resource to check
     * @param Amount The amount required
     * @return True if the source has enough resource
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resources")
    bool HasResource(const FName& ResourceType, float Amount) const;
    
    /**
     * Consume a specific amount of resource
     * @param ResourceType The type of resource to consume
     * @param Amount The amount to consume
     * @return True if the resource was successfully consumed
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resources")
    bool ConsumeResource(const FName& ResourceType, float Amount);
    
    /**
     * Get the current percentage of a resource (0-1)
     * @param ResourceType The type of resource
     * @return The resource percentage (0-1)
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resources")
    float GetResourcePercent(const FName& ResourceType) const;
    
    /**
     * Get the current value of a resource
     * @param ResourceType The type of resource
     * @return The current resource value
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resources")
    float GetResourceValue(const FName& ResourceType) const;
    
    /**
     * Get the maximum value of a resource
     * @param ResourceType The type of resource
     * @return The maximum resource value
     */
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Resources")
    float GetMaxResourceValue(const FName& ResourceType) const;
};